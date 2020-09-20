#include "mavlink_main.h"
#include "mavlink_messages.h"
#include <mavlink_logs.h>
#include "mavlink_receiver.h"
#include <string>
#include "hrt_timer.h"
#include <utlist.h>
#include "foc_function.h"
#include "serial.h"
#include "debug.h"

const osThreadAttr_t tels_attributes = {
		.name = "tels",
		.priority = (osPriority_t)osPriorityHigh,
		.stack_size = 1024};

namespace Mav {
	static Mavlink *gMav;
}

static void mavsend_func(Mavlink *pThis)
{
    pThis->run(pThis->_param);
}

#define MAX_DATA_RATE				10000000	///< max data rate in bytes/s
#define MAIN_LOOP_DELAY 			10000	///< 100 Hz @ 1000 bytes/s data rate

mavlink_system_t mavlink_system = {
	0x17,
	0x00
}; // System ID, 1-255, Component/Subsystem ID, 1-255

void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length)
{
	if (Mav::gMav != nullptr) {
		Mav::gMav->send_bytes(ch, length);
	}
}

void mavlink_start_uart_send(mavlink_channel_t chan, int length)
{
	if (Mav::gMav != nullptr) {
		Mav::gMav->begin_send();
	}
}

void mavlink_end_uart_send(mavlink_channel_t chan, int length)
{
	if (Mav::gMav != nullptr) {
		Mav::gMav->send_packet();
	}
}

/*
 * Internal function to give access to the channel status for each channel
 */
mavlink_status_t *mavlink_get_channel_status(uint8_t channel)
{
	if (Mav::gMav != nullptr) {
		return Mav::gMav->get_status();

	} else {
		return nullptr;
	}
}

/*
 * Internal function to give access to the channel buffer for each channel
 */
mavlink_message_t *mavlink_get_channel_buffer(uint8_t channel)
{
	if (Mav::gMav != nullptr) {
		return Mav::gMav->get_buffer();

	} else {
		return nullptr;
	}
}

Mavlink::Mavlink(void):
	_mavlink_log_pub(nullptr),
	_transmitting_enabled(true),
	_mavlink_buffer{},
	_mavlink_status{},
	_wait_to_transmit(false),
	_received_messages(false),
	_main_loop_delay(1000),
	_subscriptions(nullptr),
	_streams(nullptr),
	_channel(MAVLINK_COMM_0),
	_rate_mult(1.0f),
	_last_hw_rate_timestamp(0),
	_mavlink_param_queue_index(0),
    _system_type(1),
	_baudrate(115200),
	_param(nullptr)
{

}

Mavlink::~Mavlink(void)
{

}

mavlink_channel_t Mavlink::get_channel()
{
	return _channel;
}

float Mavlink::get_rate_mult()
{
	return _rate_mult;
}

int Mavlink::interval_from_rate(float rate)
{
	if (rate > 0.000001f) {
		return (int)(1000000.0f / rate);

	} else if (rate < 0.0f) {
		return -1;

	} else {
		return 0;
	}
}

unsigned Mavlink::get_free_tx_buf()
{
	/*
	 * Check if the OS buffer is full and disable HW
	 * flow control if it continues to be full
	 */
	int buf_free = 255;

	return buf_free;
}

void Mavlink::begin_send()
{
	xSemaphoreTake(_send_mutex, portMAX_DELAY);
}

int Mavlink::send_packet()
{
	xSemaphoreGive(_send_mutex);
	return 1;
}

void Mavlink::send_bytes(const uint8_t *buf, unsigned packet_len)
{
	/* If the wait until transmit flag is on, only transmit after we've received messages.
	   Otherwise, transmit all the time. */
	if (!should_transmit()) {
		return;
	}

	_last_write_try_time = micros();

	if (_mavlink_start_time == 0) {
		_mavlink_start_time = _last_write_try_time;
	}

	/* check if there is space in the buffer, let it overflow else */
	unsigned buf_free = get_free_tx_buf();

	if (buf_free < packet_len) {
		/* not enough space in buffer to send */
		count_txerrbytes(packet_len);
		return;
	}

	size_t ret = -1;

	/* send message to UART */
	ret = hal_uart_write((void *)buf, packet_len);

	if (ret != (size_t) packet_len) {
		count_txerrbytes(packet_len);

	} else {
		_last_write_success_time = _last_write_try_time;
		count_txbytes(packet_len);
	}
}

MavlinkOrbSubscription *Mavlink::add_orb_subscription(const ipc_id_t topic, int instance)
{
	/* check if already subscribed to this topic */
	MavlinkOrbSubscription *sub;

	LL_FOREACH(_subscriptions, sub) {
		if (sub->get_topic() == topic && sub->get_instance() == instance) {
			/* already subscribed */
			return sub;
		}
	}

	/* add new subscription */
	MavlinkOrbSubscription *sub_new = new MavlinkOrbSubscription(topic, instance);

	LL_APPEND(_subscriptions, sub_new);

	return sub_new;
}


void Mavlink::send_statustext_info(const char *string)
{
	mavlink_log_info(&_mavlink_log_pub, string);
}

void Mavlink::send_statustext_critical(const char *string)
{
	mavlink_log_critical(&_mavlink_log_pub, string);
}

void Mavlink::send_statustext_emergency(const char *string)
{
	mavlink_log_emergency(&_mavlink_log_pub, string);
}

int Mavlink::configure_stream(const char *stream_name, const float rate)
{
	/* calculate interval in us, -1 means unlimited stream, 0 means disabled */
	int interval = interval_from_rate(rate);

	/* search if stream exists */
	MavlinkStream *stream;
	LL_FOREACH(_streams, stream) {
		if (strcmp(stream_name, stream->get_name()) == 0) {
			if (interval != 0) {
				/* set new interval */
				stream->set_interval(interval);

			} else {
				/* delete stream */
				LL_DELETE(_streams, stream);
				delete stream;
			}

			return 1;
		}
	}

	if (interval == 0) {
		/* stream was not active and is requested to be disabled, do nothing */
		return 1;
	}

	// search for stream with specified name in supported streams list
	// create new instance if found
	stream = create_mavlink_stream(stream_name, this);

	if (stream != nullptr) {
		stream->set_interval(interval);
		LL_APPEND(_streams, stream);

		return 1;
	}

	/* if we reach here, the stream list does not contain the stream */
	Info_Debug("stream %s not found\n", stream_name);

	return 0;
}

void Mavlink::adjust_stream_rates(const float multiplier)
{
	/* do not allow to push us to zero */
	if (multiplier < MAVLINK_MIN_MULTIPLIER) {
		return;
	}

	/* search if stream exists */
	MavlinkStream *stream;
	LL_FOREACH(_streams, stream) {
		/* set new interval */
		int interval = stream->get_interval();

		if (interval > 0) {
			interval /= multiplier;

			/* limit min / max interval */
			if (interval < MAVLINK_MIN_INTERVAL) {
				interval = MAVLINK_MIN_INTERVAL;
			}

			if (interval > MAVLINK_MAX_INTERVAL) {
				interval = MAVLINK_MAX_INTERVAL;
			}

			/* set new interval */
			stream->set_interval(interval);
		}
	}
}


void Mavlink::update_rate_mult()
{
	float const_rate = 0.0f;
	float rate = 0.0f;

	/* scale down rates if their theoretical bandwidth is exceeding the link bandwidth */
	MavlinkStream *stream;
	LL_FOREACH(_streams, stream) {
		if (stream->const_rate()) {
			const_rate += (stream->get_interval() > 0) ? stream->get_size_avg() * 1000000.0f / stream->get_interval() : 0;

		} else {
			rate += (stream->get_interval() > 0) ? stream->get_size_avg() * 1000000.0f / stream->get_interval() : 0;
		}
	}

	/* scale up and down as the link permits */
	float bandwidth_mult = (float)(_datarate - const_rate) / rate;

	/* if we do not have flow control, limit to the set data rate */
	bandwidth_mult = fminf(1.0f, bandwidth_mult);

	/* check if we have radio feedback */
	struct telemetry_status_s &tstatus = get_rx_status();

	bool radio_critical = false;
	bool radio_found = false;

	float hardware_mult = _rate_mult;

	/* scale down if we have a TX err rate suggesting link congestion */
	if (_rate_txerr > 0.0f && !radio_critical) {
		hardware_mult = (_rate_tx) / (_rate_tx + _rate_txerr);

	} else if (radio_found && tstatus.telem_time != _last_hw_rate_timestamp) {

		if (tstatus.txbuf < RADIO_BUFFER_CRITICAL_LOW_PERCENTAGE) {
			/* this indicates link congestion, reduce rate by 20% */
			hardware_mult *= 0.80f;

		} else if (tstatus.txbuf < RADIO_BUFFER_LOW_PERCENTAGE) {
			/* this indicates link congestion, reduce rate by 2.5% */
			hardware_mult *= 0.975f;

		} else if (tstatus.txbuf > RADIO_BUFFER_HALF_PERCENTAGE) {
			/* this indicates spare bandwidth, increase by 2.5% */
			hardware_mult *= 1.025f;
			/* limit to a max multiplier of 1 */
			hardware_mult = fminf(1.0f, hardware_mult);
		}

	} else if (!radio_found) {
		/* no limitation, set hardware to 1 */
		hardware_mult = 1.0f;
	}

	_last_hw_rate_timestamp = tstatus.telem_time;

	/* pick the minimum from bandwidth mult and hardware mult as limit */
	_rate_mult = fminf(bandwidth_mult, hardware_mult);

	/* ensure the rate multiplier never drops below 5% so that something is always sent */
	_rate_mult = fmaxf(0.05f, _rate_mult);
}

bool Mavlink::init(void)
{
	_send_mutex = xSemaphoreCreateMutex();
    if(_send_mutex == nullptr) {
        Info_Debug("telemetry mutex init error!\n");
        return false;
    }

    pref_telemetry_interval = perf_alloc(PC_INTERVAL, "tel_int");

	_handle = osThreadNew((osThreadFunc_t)mavsend_func, this, &tels_attributes);

    if (_handle == nullptr) {
		Info_Debug("telemetry task start error!\n");
		return false;
    }
	return true;
}

void Mavlink::run(void *parameter)
{
	_baudrate = 115200;
	_datarate = 0;

	/* convert bits to bytes and use 1/2 of bandwidth by default */
	_datarate = _baudrate / 20;

	if (_datarate > MAX_DATA_RATE) {
		_datarate = MAX_DATA_RATE;
	}

	/* HEARTBEAT is constant rate stream, rate never adjusted */
	configure_stream("HEARTBEAT", 1.0f);

	/* set main loop delay depending on data rate to minimize CPU overhead */
	_main_loop_delay = (MAIN_LOOP_DELAY * 1000) / _datarate;
    
	if(!MavlinkReceiver::receive_start(this)) {
		return;
	}

	_transmitting_enabled = true;

    while (1)
    {
        perf_count(pref_telemetry_interval);

		uint64_t t = micros();

		update_rate_mult();

		/* update streams */
		MavlinkStream *stream;
		LL_FOREACH(_streams, stream) {
			stream->update(t);
		}
        
        // 50Hz loop
        osDelay(20);
    }
}

/* ---------------------------------------namespace define----------------------------------------- */
int mavlink_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
	for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {
			if (Mav::gMav != nullptr) {
                Info_Debug("already running\n");
                return 0;
            }

            Mav::gMav = new Mavlink();
            
            if (Mav::gMav == NULL) {
                Info_Debug("alloc failed\n");
                return 0;
            }

            Mav::gMav->init();
        }
    }
    return 1;
}
