#ifndef __MAVLINK_H__
#define __MAVLINK_H__

#include "cmsis_os.h"
#include <semphr.h>
#include "ipc.h"
#include "uPerf.h"
#include <param.h>
#include <ringbuffer_cpp.h>

#include "mavlink_bridge_header.h"
#include "mavlink_stream.h"
#include "mavlink_orb_subscription.h"

#include <topics/parameter_update.h>
#include <topics/telemetry_status.h>

#define HASH_PARAM "_HASH_CHECK"

class Mavlink
{
public:
    Mavlink(void);
    ~Mavlink(void);

    bool init(void);
	
	void *_param;
    void run(void *parameter);

    mavlink_channel_t	get_channel();

    MavlinkStream 		*get_streams() const { return _streams; }

    int			    get_data_rate()		{ return _datarate; }
	void			set_data_rate(int rate) { if (rate > 0) { _datarate = rate; } }

	float			get_baudrate() { return _baudrate; }

	float           get_rate_mult();
    unsigned		get_main_loop_delay() const { return _main_loop_delay; }

    MavlinkOrbSubscription *add_orb_subscription(const ipc_id_t topic);

    unsigned		get_system_type() { return _system_type; }

    bool			is_connected() { return ((_rstatus.heartbeat_time > 0) && (micros() - _rstatus.heartbeat_time < 3000000)); }

    /**
	 * Get the receive status of this MAVLink link
	 */
	struct telemetry_status_s	&get_rx_status() { return _rstatus; }

	orb_advert_t		*get_mavlink_log_pub() { return &_mavlink_log_pub; }

	/**
	 * This is the beginning of a MAVLINK_START_UART_SEND/MAVLINK_END_UART_SEND transaction
	 */
	void 			begin_send();

    /**
	 * Send bytes out on the link.
	 *
	 * On a network port these might actually get buffered to form a packet.
	 */
	void			send_bytes(const uint8_t *buf, unsigned packet_len);

    /**
	 * Flush the transmit buffer and send one MAVLink packet
	 *
	 * @return the number of bytes sent or -1 in case of error
	 */
	int             	send_packet();

    mavlink_message_t 	*get_buffer() { return &_mavlink_buffer; }

	mavlink_status_t 	*get_status() { return &_mavlink_status; }

    /* Functions for waiting to start transmission until message received. */
	void			set_has_received_messages(bool received_messages) { _received_messages = received_messages; }
	bool			get_has_received_messages() { return _received_messages; }
	void			set_wait_to_transmit(bool wait) { _wait_to_transmit = wait; }
	bool			get_wait_to_transmit() { return _wait_to_transmit; }
	bool			should_transmit() { return (_transmitting_enabled && (!_wait_to_transmit || (_wait_to_transmit && _received_messages))); }

	int configure_stream(const char *stream_name, const float rate = -1.0f);

    /**
	 * Get the free space in the transmit buffer
	 *
	 * @return free space in the UART TX buffer
	 */
	unsigned		get_free_tx_buf();

    /**
	 * Count bytes not transmitted because of errors
	 */
	void			count_txerrbytes(unsigned n) { _bytes_txerr += n; };

    /**
	 * Count transmitted bytes
	 */
	void			count_txbytes(unsigned n) { _bytes_tx += n; };

    /**
	 * Count received bytes
	 */
	void			count_rxbytes(unsigned n) { _bytes_rx += n; };

	/**
	 * Send a status text with loglevel INFO
	 *
	 * @param string the message to send (will be capped by mavlink max string length)
	 */
	void			send_statustext_info(const char *string);

	/**
	 * Send a status text with loglevel CRITICAL
	 *
	 * @param string the message to send (will be capped by mavlink max string length)
	 */
	void			send_statustext_critical(const char *string);

	/**
	 * Send a status text with loglevel EMERGENCY
	 *
	 * @param string the message to send (will be capped by mavlink max string length)
	 */
	void			send_statustext_emergency(const char *string);

	/**
	 * Send a status text with loglevel, the difference from mavlink_log_xxx() is that message sent
	 * only on this mavlink connection. Useful for reporting communication specific, not system-wide info
	 * only to client interested in it. Message will be not sent immediately but queued in buffer as
	 * for mavlink_log_xxx().
	 *
	 * @param string the message to send (will be capped by mavlink max string length)
	 * @param severity the log level
	 */
	void			send_statustext(unsigned char severity, const char *string);

protected:
    osThreadId_t    _handle;

private:

    SemaphoreHandle_t   _send_mutex;

    mavlink_channel_t	_channel;

	int			    _baudrate;
    int			    _datarate;		///< data rate for normal streams (attitude, position, etc.)
	int			    _datarate_events;	///< data rate for params, waypoints, text messages
	float			_rate_mult;
	uint64_t		_last_hw_rate_timestamp;

    struct telemetry_status_s	_rstatus;			///< receive status

	orb_advert_t		_mavlink_log_pub;

	/**
	 * If the queue index is not at 0, the queue sending
	 * logic will send parameters from the current index
	 * to len - 1, the end of the param list.
	 */
	unsigned int		_mavlink_param_queue_index;

    static constexpr unsigned MAVLINK_MAX_INSTANCES = 2;
	static constexpr unsigned MAVLINK_MIN_INTERVAL = 1500;
	static constexpr unsigned MAVLINK_MAX_INTERVAL = 10000;
	static constexpr float MAVLINK_MIN_MULTIPLIER = 0.0005f;

    mavlink_message_t _mavlink_buffer;
	mavlink_status_t _mavlink_status;

    bool			_transmitting_enabled;
	bool			_wait_to_transmit;  	/**< Wait to transmit until received messages. */
	bool			_received_messages;	/**< Whether we've received valid mavlink messages. */

    static int		interval_from_rate(float rate);

    uint64_t		_last_write_success_time;
	uint64_t		_last_write_try_time;
	uint64_t		_mavlink_start_time;

    unsigned		_bytes_tx;
	unsigned		_bytes_txerr;
	unsigned		_bytes_rx;
	uint64_t		_bytes_timestamp;
	float			_rate_tx;
	float			_rate_txerr;
	float			_rate_rx;

	static constexpr unsigned RADIO_BUFFER_CRITICAL_LOW_PERCENTAGE = 25;
	static constexpr unsigned RADIO_BUFFER_LOW_PERCENTAGE = 35;
	static constexpr unsigned RADIO_BUFFER_HALF_PERCENTAGE = 50;

    unsigned		        _main_loop_delay;	/**< mainloop delay, depends on data rate */

	MavlinkOrbSubscription	*_subscriptions;
    MavlinkStream		    *_streams;

	/**
	 * Adjust the stream rates based on the current rate
	 *
	 * @param multiplier if greater than 1, the transmission rate will increase, if smaller than one decrease
	 */
	void adjust_stream_rates(const float multiplier);

	/**
	 * Update rate mult so total bitrate will be equal to _datarate.
	 */
	void update_rate_mult();

    unsigned		_system_type;
    
    perf_counter_t pref_telemetry_interval;
    
};

#endif