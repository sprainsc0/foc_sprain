#include <stdio.h>

#include "mavlink_parameters.h"
#include "mavlink_main.h"

MavlinkParametersManager::MavlinkParametersManager(Mavlink *mavlink) :
	_send_all_index(-1),
	_mavlink_parameter_sub(-1),
	_param_update_time(0),
	_param_update_index(0),
	_mavlink(mavlink)
{
}
MavlinkParametersManager::~MavlinkParametersManager()
{
	
}

unsigned
MavlinkParametersManager::get_size()
{
	return MAVLINK_MSG_ID_PARAM_VALUE_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
}

void
MavlinkParametersManager::handle_message(const mavlink_message_t *msg)
{
	switch (msg->msgid) {
	case MAVLINK_MSG_ID_PARAM_REQUEST_LIST: {
			/* request all parameters */
			mavlink_param_request_list_t req_list;
			mavlink_msg_param_request_list_decode(msg, &req_list);

			if (req_list.target_system == mavlink_system.sysid &&
			    (req_list.target_component == mavlink_system.compid || req_list.target_component == 0)) {
				if (_send_all_index < 0) {
					_send_all_index = PARAM_HASH;

				} else {
					/* a restart should skip the hash check on the ground */
					_send_all_index = 0;
				}
			}
			break;
		}

	case MAVLINK_MSG_ID_PARAM_SET: {
			/* set parameter */
			mavlink_param_set_t set;
			mavlink_msg_param_set_decode(msg, &set);

			if (set.target_system == mavlink_system.sysid &&
			    (set.target_component == mavlink_system.compid || set.target_component == 0)) {

				/* local name buffer to enforce null-terminated string */
				char name[MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN + 1];
				strncpy(name, set.param_id, MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
				/* enforce null termination */
				name[MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN] = '\0';

				/* Whatever the value is, we're being told to stop sending */
				if (strncmp(name, "_HASH_CHECK", sizeof(name)) == 0) {
					_send_all_index = -1;
					/* No other action taken, return */
					return;
				}

				/* attempt to find parameter, set and send it */
				param_t param = param_find_no_notification(name);

				if (param == PARAM_INVALID) {
					char buf[MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN];
					sprintf(buf, "[pm] unknown param: %s", name);
					_mavlink->send_statustext_info(buf);

				} else {
					// Load current value before setting it
					float curr_val;
					param_get(param, &curr_val);
					param_set(param, &(set.param_value));

					// Check if the parameter changed. If it didn't change, send current value back
					if (!(fabsf(curr_val - set.param_value) > 0.0f)) {
						send_param(param);
					}
				}
			}

			break;
		}

	case MAVLINK_MSG_ID_PARAM_REQUEST_READ: {
			/* request one parameter */
			mavlink_param_request_read_t req_read;
			mavlink_msg_param_request_read_decode(msg, &req_read);

			if (req_read.target_system == mavlink_system.sysid &&
			    (req_read.target_component == mavlink_system.compid || req_read.target_component == 0)) {

				/* when no index is given, loop through string ids and compare them */
				if (req_read.param_index < 0) {
					/* XXX: I left this in so older versions of QGC wouldn't break */
					if (strncmp(req_read.param_id, HASH_PARAM, MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN) == 0) {
						/* return hash check for cached params */
						uint32_t hash = param_hash_check();

						/* build the one-off response message */
						mavlink_param_value_t param_value;
						param_value.param_count = param_count_used();
						param_value.param_index = -1;
						strncpy(param_value.param_id, HASH_PARAM, MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
						param_value.param_type = MAV_PARAM_TYPE_INT32;
						memcpy(&param_value.param_value, &hash, sizeof(hash));
						mavlink_msg_param_value_send_struct(_mavlink->get_channel(), &param_value);

					} else {
						/* local name buffer to enforce null-terminated string */
						char name[MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN + 1];
						strncpy(name, req_read.param_id, MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
						/* enforce null termination */
						name[MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN] = '\0';
						/* attempt to find parameter and send it */
						send_param(param_find_no_notification(name));
					}

				} else {
					/* when index is >= 0, send this parameter again */
					int ret = send_param(param_for_used_index(req_read.param_index));

					if (ret == 1) {
						char buf[MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN];
						sprintf(buf, "[pm] unknown param ID: %u", req_read.param_index);
						_mavlink->send_statustext_info(buf);

					} else if (ret == 2) {
						char buf[MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN];
						sprintf(buf, "[pm] failed loading param from storage ID: %u", req_read.param_index);
						_mavlink->send_statustext_info(buf);
					}
				}
			}

			break;
		}

	default:
		break;
	}
}

void
MavlinkParametersManager::send(const uint64_t t)
{
	int max_num_to_send;

	if (_mavlink->get_channel() == MAVLINK_COMM_0) {
		max_num_to_send = 3;

	} else {
		// speed up parameter loading via UDP, TCP or USB: try to send 20 at once
		max_num_to_send = 20;
	}

	int i = 0;

	// Send while burst is not exceeded, we still have buffer space and still something to send
	while ((i++ < max_num_to_send) && (_mavlink->get_free_tx_buf() >= get_size()) && send_params());
}

bool
MavlinkParametersManager::send_params()
{
	if (send_one()) {
		return true;

	} else if (send_untransmitted()) {
		return true;

	} else {
		return false;
	}
}

bool
MavlinkParametersManager::send_untransmitted()
{
	bool sent_one = false;

	// Check for untransmitted system parameters
	if (_mavlink_parameter_sub < 0) {
		_mavlink_parameter_sub = ipc_subscibe(IPC_ID(parameter_update));
	}

	bool param_ready;
	ipc_check(_mavlink_parameter_sub, &param_ready);

	if (param_ready) {
		// Clear the ready flag
		struct parameter_update_s value;
		ipc_pull(IPC_ID(parameter_update), _mavlink_parameter_sub, &value);

		// Schedule an update if not already the case
		if (_param_update_time == 0) {
			_param_update_time = value.timestamp;
			_param_update_index = 0;
		}
	}

	if ((_param_update_time != 0) && ((_param_update_time + 5 * 1000) < micros())) {

		param_t param = 0;

		// send out all changed values
		do {
			// skip over all parameters which are not invalid and not used
			do {
				param = param_for_index(_param_update_index);
				++_param_update_index;
			} while (param != PARAM_INVALID && !param_used(param));

			// send parameters which are untransmitted while there is
			// space in the TX buffer
			if ((param != PARAM_INVALID) && param_value_unsaved(param)) {
				int ret = send_param(param);
				char buf[100];
				strncpy(&buf[0], param_name(param), MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
				sent_one = true;

				if (ret != 1) {
					break;
				}
			}
		} while ((_mavlink->get_free_tx_buf() >= get_size()) && (_param_update_index < (int) param_count()));

		// Flag work as done once all params have been sent
		if (_param_update_index >= (int) param_count()) {
			_param_update_time = 0;
		}
	}

	return sent_one;
}

bool
MavlinkParametersManager::send_one()
{
	if (_send_all_index >= 0) {
		/* send all parameters if requested, but only after the system has booted */

		/* The first thing we send is a hash of all values for the ground
		 * station to try and quickly load a cached copy of our params
		 */
		if (_send_all_index == PARAM_HASH) {
			/* return hash check for cached params */
			uint32_t hash = param_hash_check();

			/* build the one-off response message */
			mavlink_param_value_t msg;
			msg.param_count = param_count_used();
			msg.param_index = -1;
			strncpy(msg.param_id, HASH_PARAM, MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
			msg.param_type = MAV_PARAM_TYPE_UINT32;
			memcpy(&msg.param_value, &hash, sizeof(hash));
			mavlink_msg_param_value_send_struct(_mavlink->get_channel(), &msg);

			/* after this we should start sending all params */
			_send_all_index = 0;

			/* No further action, return now */
			return true;
		}

		/* look for the first parameter which is used */
		param_t p;

		do {
			/* walk through all parameters, including unused ones */
			p = param_for_index(_send_all_index);
			_send_all_index++;
		} while (p != PARAM_INVALID && !param_used(p));

		if (p != PARAM_INVALID) {
			send_param(p);
		}

		if ((p == PARAM_INVALID) || (_send_all_index >= (int) param_count())) {
			_send_all_index = -1;
			return false;

		} else {
			return true;
		}

	} else if (_send_all_index == PARAM_HASH && micros() > 20 * 1000 * 1000) {
		/* the boot did not seem to ever complete, warn user and set boot complete */
		_mavlink->send_statustext_critical("WARNING: SYSTEM BOOT INCOMPLETE. CHECK CONFIG.");
	}

	return false;
}

int
MavlinkParametersManager::send_param(param_t param, int component_id)
{
	if (param == PARAM_INVALID) {
		return 0;
	}

	/* no free TX buf to send this param */
	if (_mavlink->get_free_tx_buf() < MAVLINK_MSG_ID_PARAM_VALUE_LEN) {
		return 0;
	}

	mavlink_param_value_t msg;

	/*
	 * get param value, since MAVLink encodes float and int params in the same
	 * space during transmission, copy param onto float val_buf
	 */
	if (param_get(param, &msg.param_value) != 1) {
		return 0;
	}

	msg.param_count = param_count_used();
	msg.param_index = param_get_used_index(param);

#if defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
	/*
	 * coverity[buffer_size_warning : FALSE]
	 *
	 * The MAVLink spec does not require the string to be NUL-terminated if it
	 * has length 16. In this case the receiving end needs to terminate it
	 * when copying it.
	 */
	strncpy(msg.param_id, param_name(param), MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
#if defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic pop
#endif

	/* query parameter type */
	param_type_t type = param_type(param);

	/*
	 * Map onboard parameter type to MAVLink type,
	 * endianess matches (both little endian)
	 */
	if (type == PARAM_TYPE_INT32) {
		msg.param_type = MAVLINK_TYPE_INT32_T;

	} else if (type == PARAM_TYPE_FLOAT) {
		msg.param_type = MAVLINK_TYPE_FLOAT;

	} else {
		msg.param_type = MAVLINK_TYPE_FLOAT;
	}

	/* default component ID */
	if (component_id < 0) {
		mavlink_msg_param_value_send_struct(_mavlink->get_channel(), &msg);

	} else {
		// Re-pack the message with a different component ID
		mavlink_message_t mavlink_packet;
		mavlink_msg_param_value_encode_chan(mavlink_system.sysid, component_id, _mavlink->get_channel(), &mavlink_packet, &msg);
		_mavlink_resend_uart(_mavlink->get_channel(), &mavlink_packet);
	}

	return 1;
}

