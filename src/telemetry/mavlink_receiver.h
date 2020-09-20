/****************************************************************************
 *
 *   Copyright (c) 2012-2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mavlink_receiver.h
 * MAVLink receiver thread
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Anton Babushkin <anton@px4.io>
 */

#pragma once

#include <ipc.h>
#include "uPerf.h"
#include "mavlink_parameters.h"

class Mavlink;

class MavlinkReceiver
{
public:
	/**
	 * Constructor
	 */
	MavlinkReceiver(Mavlink *parent);

	/**
	 * Destructor, also kills the mavlinks task.
	 */
	~MavlinkReceiver();

	/**
	 * Start the receiver thread
	 */
	static bool receive_start(Mavlink *parent);

	bool init(void);
	void *_param;
    void run(void *parameter);
protected:
    osThreadId_t    _handle;

private:

	void handle_message(mavlink_message_t *msg);
	/**
	 * common method to handle both mavlink command types. T is one of mavlink_command_int_t or mavlink_command_long_t
	 */
	void handle_message_heartbeat(mavlink_message_t *msg);

	orb_advert_t _telemetry_status_pub;


	void receive_thread(void *arg);


	Mavlink	*_mavlink;

	MavlinkParametersManager	_parameters_manager;

	mavlink_status_t _status; ///< receiver status, used for mavlink_parse_char()

	perf_counter_t pref_receive_interval;

	MavlinkReceiver(const MavlinkReceiver &) = delete;
	MavlinkReceiver operator=(const MavlinkReceiver &) = delete;
};
