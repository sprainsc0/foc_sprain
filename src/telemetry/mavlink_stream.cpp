#include <stdlib.h>

#include "mavlink_stream.h"
#include "mavlink_main.h"

MavlinkStream::MavlinkStream(Mavlink *mavlink) :
	next(nullptr),
	_mavlink(mavlink),
	_interval(1000000),
	_last_sent(0 /* 0 means unlimited - updates on every iteration */)
{
}

MavlinkStream::~MavlinkStream()
{
}

/**
 * Set messages interval in ms
 */
void
MavlinkStream::set_interval(const int interval)
{
	_interval = interval;
}

/**
 * Update subscriptions and send message if necessary
 */
int
MavlinkStream::update(const hrt_abstime t)
{
	// If the message has never been sent before we want
	// to send it immediately and can return right away
	if (_last_sent == 0) {
		// this will give different messages on the same run a different
		// initial timestamp which will help spacing them out
		// on the link scheduling

		if (send(t)) {
			_last_sent = hrt_absolute_time();
		}
		return 0;
	}

	// One of the previous iterations sent the update
	// already before the deadline
	if (_last_sent > t) {
		return -1;
	}

	int64_t dt = t - _last_sent;
	int interval = (_interval > 0) ? _interval : 0;

	if (!const_rate()) {
		interval /= _mavlink->get_rate_mult();
	}

	// Send the message if it is due or
	// if it will overrun the next scheduled send interval
	// by 30% of the interval time. This helps to avoid
	// sending a scheduled message on average slower than
	// scheduled. Doing this at 50% would risk sending
	// the message too often as the loop runtime of the app
	// needs to be accounted for as well.
	// This method is not theoretically optimal but a suitable
	// stopgap as it hits its deadlines well (0.5 Hz, 50 Hz and 250 Hz)

	if (interval == 0 || (dt > (interval - (_mavlink->get_main_loop_delay() / 10) * 3))) {
		// interval expired, send message
		bool sent = true;

		sent = send(t);

		// If the interval is non-zero and dt is smaller than 1.5 times the interval
		// do not use the actual time but increment at a fixed rate, so that processing delays do not
		// distort the average rate. The check of the maximum interval is done to ensure that after a
		// long time not sending anything, sending multiple messages in a short time is avoided.
		if (sent) {
			_last_sent = ((interval > 0) && ((int64_t)(1.5f * interval) > dt)) ? _last_sent + interval : t;
			return 0;

		} else {
			return -1;
		}
	}

	return -1;
}
