#include "mavlink_orb_subscription.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <bgc_defines.h>
#include "IPC.h"

MavlinkOrbSubscription::MavlinkOrbSubscription(const ipc_id_t topic, int instance) :
	next(nullptr),
	_topic(topic),
	_fd(-1),
	_instance(instance),
	_published(false),
	_last_pub_check(0)
{
}

MavlinkOrbSubscription::~MavlinkOrbSubscription()
{
	if (_fd >= 0) {
		ipc_unsubscibe(_fd);
	}
}

ipc_id_t
MavlinkOrbSubscription::get_topic() const
{
	return _topic;
}

int
MavlinkOrbSubscription::get_instance() const
{
	return _instance;
}

bool
MavlinkOrbSubscription::update(void *data)
{
	if (!is_published()) {
		return false;
	}

	if (ipc_pull(_topic, _fd, data) != B_EOK) {
		return false;
	}

	return true;
}

bool
MavlinkOrbSubscription::update_if_changed(void *data)
{
	bool prevpub = _published;

	if (!is_published()) {
		return false;
	}

	bool updated;

	if (ipc_check(_fd, &updated) != B_EOK) {
		return false;
	}

	// If we didn't update and this topic did not change
	// its publication status then nothing really changed
	if (!updated && prevpub == _published) {
		return false;
	}

	return update(data);
}

bool
MavlinkOrbSubscription::is_published()
{
	// If we marked it as published no need to check again
	if (_published) {
		return true;
	}

	hrt_abstime now = hrt_absolute_time();

	if (now - _last_pub_check < 300000) {
		return false;
	}

	// We are checking now
	_last_pub_check = now;

	if (_fd < 0) {
		_fd = ipc_subscibe_multi(_topic, _instance);
	}

	bool updated = false;
	ipc_check(_fd, &updated);

	if (updated) {
		_published = true;
	}

	return _published;
}

