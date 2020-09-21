
/* XXX trim includes */
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <stdlib.h>
#include "param.h"
#include "foc_function.h"
#include "serial.h"
#include "debug.h"

#include "mavlink_messages.h"
#include "mavlink_bridge_header.h"
#include "mavlink_receiver.h"
#include "mavlink_main.h"

const osThreadAttr_t telr_attributes = {
		.name = "telr",
		.priority = (osPriority_t)osPriorityHigh,
		.stack_size = 1024};

namespace MavR {
	static MavlinkReceiver *gRecv;
}

static void mavrecv_func(MavlinkReceiver *pThis)
{
    pThis->run(pThis->_param);
}

MavlinkReceiver::MavlinkReceiver(Mavlink *parent) :
	_mavlink(parent),
	_status{},
	_telemetry_status_pub(nullptr),
    _parameters_manager(parent),
	_param(nullptr)
{
}

MavlinkReceiver::~MavlinkReceiver()
{

}


void
MavlinkReceiver::handle_message(mavlink_message_t *msg)
{
	switch (msg->msgid) {
	case MAVLINK_MSG_ID_HEARTBEAT:
		handle_message_heartbeat(msg);
		break;
	default:
		break;
	}

	/* If we've received a valid message, mark the flag indicating so.
	   This is used in the '-w' command-line flag. */
	_mavlink->set_has_received_messages(true);
}

void MavlinkReceiver::handle_message_heartbeat(mavlink_message_t *msg)
{
	/* telemetry status supported only on first TELEMETRY_STATUS_ORB_ID_NUM mavlink channels */
    mavlink_heartbeat_t hb;
    mavlink_msg_heartbeat_decode(msg, &hb);

    /* ignore own heartbeats, accept only heartbeats from GCS */
    if (msg->sysid != mavlink_system.sysid && hb.type == MAV_TYPE_GCS) {

        struct telemetry_status_s &tstatus = _mavlink->get_rx_status();

        /* set heartbeat time and topic time and publish -
         * the telem status also gets updated on telemetry events
         */
        tstatus.timestamp = micros();
        tstatus.heartbeat_time = tstatus.timestamp;

        if (_telemetry_status_pub == nullptr) {
            _telemetry_status_pub = ipc_active(IPC_ID(telemetry_status), &tstatus);

        } else {
            ipc_push(IPC_ID(telemetry_status), _telemetry_status_pub, &tstatus);
        }
    }
}

/**
 * Receive data from UART.
 */
void MavlinkReceiver::run(void *parameter)
{
	// poll timeout in ms. Also defines the max update frequency of the mission & param manager, etc.
	const int timeout = 10;

	/* the serial port buffers internally as well, we just need to fit a small chunk */
	uint8_t buf[64];

	mavlink_message_t msg;

	uint16_t nread = 0;
	uint64_t last_send_update = 0;

	pref_receive_interval = perf_alloc(PC_INTERVAL, "recv_int");

	while (1) {

		perf_count(pref_receive_interval);
		/*
			* to avoid reading very small chunks wait for data before reading
			* this is designed to target one message, so >20 bytes at a time
			*/
		const unsigned character_count = 20;

		nread = hal_uart_read(buf, sizeof(buf));

		/* non-blocking read. read may return negative values */
		if (nread < (uint16_t)character_count) {
			unsigned sleeptime = (unsigned)((1.0f / (_mavlink->get_baudrate() / 10)) * character_count * 1000);
			osDelay(sleeptime);
		}
		// only start accepting messages once we're sure who we talk to

		/* if read failed, this loop won't execute */
		for (uint16_t i = 0; i < nread; i++) {
			if (mavlink_parse_char(_mavlink->get_channel(), buf[i], &msg, &_status)) {

				/* handle generic messages and commands */
				handle_message(&msg);

				/* handle packet with parameter component */
				_parameters_manager.handle_message(&msg);
			}
		}

		/* count received bytes (nread will be -1 on read error) */
		if (nread > 0) {
			_mavlink->count_rxbytes(nread);
		}

		uint64_t t = micros();

		if (t - last_send_update > timeout * 1000) {

			_parameters_manager.send(t);

			last_send_update = t;
		}

		// 100Hz loop
        osDelay(10);
	}
}

bool MavlinkReceiver::init()
{
	_handle = osThreadNew((osThreadFunc_t)mavrecv_func, this, &telr_attributes);

    if (_handle == nullptr) {
		Info_Debug("telemetry task start error!\n");
        return false;
    }

	return true;
}

bool
MavlinkReceiver::receive_start(Mavlink *parent)
{
	MavR::gRecv = new MavlinkReceiver(parent);
	if (MavR::gRecv == nullptr) {
		Info_Debug("alloc failed");
		return false;
	}

	if(!MavR::gRecv->init()) {
		delete MavR::gRecv;
		return false;
	}
	return true;
}
