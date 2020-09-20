#ifndef MAVLINK_STREAM_H_
#define MAVLINK_STREAM_H_

#include "hrt_timer.h"

class Mavlink;

class MavlinkStream
{

public:
	MavlinkStream *next;

	MavlinkStream(Mavlink *mavlink);
	virtual ~MavlinkStream();

	/**
	 * Get the interval
	 *
	 * @param interval the interval in microseconds (us) between messages
	 */
	void set_interval(const int interval);

	/**
	 * Get the interval
	 *
	 * @return the inveral in microseconds (us) between messages
	 */
	int get_interval() { return _interval; }

	/**
	 * @return 0 if updated / sent, -1 if unchanged
	 */
	int update(const uint64_t t);
	virtual const char *get_name() const = 0;
	virtual uint16_t get_id() = 0;

	/**
	 * @return true if steam rate shouldn't be adjusted
	 */
	virtual bool const_rate() { return true; }

	/**
	 * Get maximal total messages size on update
	 */
	virtual unsigned get_size() = 0;

	/**
	 * Get the average message size
	 *
	 * For a normal stream this equals the message size,
	 * for something like a parameter or mission message
	 * this equals usually zero, as no bandwidth
	 * needs to be reserved
	 */
	virtual unsigned get_size_avg() { return get_size(); }

protected:
	Mavlink     *_mavlink;
	int _interval;		///< if set to negative value = unlimited rate

	virtual bool send(const uint64_t t) = 0;

private:
	uint64_t _last_sent;

	/* do not allow top copying this class */
	MavlinkStream(const MavlinkStream &);
	MavlinkStream &operator=(const MavlinkStream &);
};


#endif /* MAVLINK_STREAM_H_ */
