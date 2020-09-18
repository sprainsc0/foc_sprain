#pragma once

#include <param.h>
#include "hrt_timer.h"
#include "IPC.h"
#include "mavlink_bridge_header.h"
#include <topics/parameter_update.h>

class Mavlink;

class MavlinkParametersManager
{
public:
	explicit MavlinkParametersManager(Mavlink *mavlink);
	~MavlinkParametersManager();

	/**
	 * Handle sending of messages. Call this regularly at a fixed frequency.
	 * @param t current time
	 */
	void send(const hrt_abstime t);

	unsigned get_size();

	void handle_message(const mavlink_message_t *msg);

private:
	int		_send_all_index;

	/* do not allow top copying this class */
	MavlinkParametersManager(MavlinkParametersManager &);
	MavlinkParametersManager &operator = (const MavlinkParametersManager &);

protected:
	/// send a single param if a PARAM_REQUEST_LIST is in progress
	/// @return true if a parameter was sent
	bool send_one();

	/**
	 * Handle any open param send transfer
	 */
	bool send_params();

	/**
	 * Send untransmitted params
	 */
	bool send_untransmitted();

	int send_param(param_t param, int component_id = -1);

	int _mavlink_parameter_sub;
	hrt_abstime _param_update_time;
	int _param_update_index;

	Mavlink *_mavlink;
};
