#ifndef MAVLINK_MESSAGES_H_
#define MAVLINK_MESSAGES_H_

#include "mavlink_stream.h"

class StreamListItem
{

public:
	MavlinkStream *(*new_instance)(Mavlink *mavlink);
	const char *(*get_name)();
	uint16_t (*get_id)();

	StreamListItem(MavlinkStream * (*inst)(Mavlink *mavlink), const char *(*name)(), uint16_t (*id)()) :
		new_instance(inst),
		get_name(name),
		get_id(id) {}

};

const char *get_stream_name(const uint16_t msg_id);
MavlinkStream *create_mavlink_stream(const char *stream_name, Mavlink *mavlink);

void get_mavlink_navigation_mode(const struct vehicle_status_s *const status, uint8_t *mavlink_base_mode,
				 union px4_custom_mode *custom_mode);

#endif /* MAVLINK_MESSAGES_H_ */
