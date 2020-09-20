#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include "mavlink_main.h"
#include "mavlink_messages.h"

#include "ipc.h"

class MavlinkStreamHeartbeat : public MavlinkStream
{
public:
	const char *get_name() const
	{
		return MavlinkStreamHeartbeat::get_name_static();
	}

	static const char *get_name_static()
	{
		return "HEARTBEAT";
	}

	static uint16_t get_id_static()
	{
		return MAVLINK_MSG_ID_HEARTBEAT;
	}

	uint16_t get_id()
	{
		return get_id_static();
	}

	static MavlinkStream *new_instance(Mavlink *mavlink)
	{
		return new MavlinkStreamHeartbeat(mavlink);
	}

	unsigned get_size()
	{
		return MAVLINK_MSG_ID_HEARTBEAT_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
	}

	bool const_rate()
	{
		return true;
	}

private:

	/* do not allow top copying this class */
	MavlinkStreamHeartbeat(MavlinkStreamHeartbeat &);
	MavlinkStreamHeartbeat &operator = (const MavlinkStreamHeartbeat &);

protected:
	explicit MavlinkStreamHeartbeat(Mavlink *mavlink) : MavlinkStream(mavlink)
	{}

	bool send(const uint64_t t)
	{
		uint8_t base_mode = 0;
		uint32_t custom_mode = 0;
		uint8_t system_status = 0;

		mavlink_msg_heartbeat_send(_mavlink->get_channel(), _mavlink->get_system_type(), 0,
					   base_mode, custom_mode, system_status);

		return true;
	}
};


static const StreamListItem streams_list[] = {
	StreamListItem(&MavlinkStreamHeartbeat::new_instance, &MavlinkStreamHeartbeat::get_name_static, &MavlinkStreamHeartbeat::get_id_static)
};

const char *get_stream_name(const uint16_t msg_id)
{
	// search for stream with specified msg id in supported streams list
	for (const auto &stream : streams_list) {
		if (msg_id == stream.get_id()) {
			return stream.get_name();
		}
	}

	return nullptr;
}

MavlinkStream *create_mavlink_stream(const char *stream_name, Mavlink *mavlink)
{
	// search for stream with specified name in supported streams list
	if (stream_name != nullptr) {
		for (const auto &stream : streams_list) {
			if (strcmp(stream_name, stream.get_name()) == 0) {
				return stream.new_instance(mavlink);
			}
		}
	}

	return nullptr;
}
