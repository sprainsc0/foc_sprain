
/* XXX trim includes */
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <errno.h>
#include <stdlib.h>
#include <param.h>
#include <bgc_function.h>

#include "mavlink_messages.h"
#include "mavlink_bridge_header.h"
#include "mavlink_receiver.h"
#include "mavlink_main.h"


#define  APP_CFG_TASK_RECV_STK_SIZE            1024u

static  OS_TCB       AppTaskRECVTCB[2];
static  CPU_STK      AppTaskRECVStk[2][APP_CFG_TASK_RECV_STK_SIZE];

MavlinkReceiver::MavlinkReceiver(Mavlink *parent) :
	_mavlink(parent),
	_status{},
	_telemetry_status_pub(nullptr),
    _parameters_manager(parent)
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
	case MAVLINK_MSG_ID_CONTROL:
		handle_message_control(msg);
		break;
	case MAVLINK_MSG_ID_FCINFO:
		handle_message_info(msg);
		break;
	case MAVLINK_MSG_ID_COMPENSATION:
		handle_message_compensation(msg);
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
	if (_mavlink->get_channel() < (mavlink_channel_t)IPC_MULTI_MAX_INSTANCES) {
		mavlink_heartbeat_t hb;
		mavlink_msg_heartbeat_decode(msg, &hb);

		/* ignore own heartbeats, accept only heartbeats from GCS */
		if (msg->sysid != mavlink_system.sysid && hb.type == MAV_TYPE_GCS) {

			struct telemetry_status_s &tstatus = _mavlink->get_rx_status();

			/* set heartbeat time and topic time and publish -
			 * the telem status also gets updated on telemetry events
			 */
			tstatus.timestamp = hrt_absolute_time();
			tstatus.heartbeat_time = tstatus.timestamp;

			if (_telemetry_status_pub == nullptr) {
				_telemetry_status_pub = ipc_active(IPC_ID(telemetry_status), &tstatus);

			} else {
				ipc_push(IPC_ID(telemetry_status), _telemetry_status_pub, &tstatus);
			}
		}
	}
}

void MavlinkReceiver::handle_message_control(mavlink_message_t *msg)
{
	/* telemetry status supported only on first TELEMETRY_STATUS_ORB_ID_NUM mavlink channels */
	if (_mavlink->get_channel() < (mavlink_channel_t)IPC_MULTI_MAX_INSTANCES) {
		mavlink_control_t hb;
		mavlink_msg_control_decode(msg, &hb);

		/* ignore own heartbeats, accept only heartbeats from GCS */
		if (msg->sysid != mavlink_system.sysid) {
			control_raw.timestamp = hrt_absolute_time();
			control_raw.rate[0] = hb.rate_x * M_DEG_TO_RAD_F;
			control_raw.rate[1] = hb.rate_y * M_DEG_TO_RAD_F;
			control_raw.rate[2] = hb.rate_z * M_DEG_TO_RAD_F;
			control_raw.angle[0] = hb.angle_x * M_DEG_TO_RAD_F;
			control_raw.angle[1] = hb.angle_y * M_DEG_TO_RAD_F;
			control_raw.angle[2] = hb.angle_z * M_DEG_TO_RAD_F;
			control_raw.mode = hb.mode;
			ipc_push(IPC_ID(gimbal_control), _control_pub, &control_raw);
		}
	}
}

void MavlinkReceiver::handle_message_info(mavlink_message_t *msg)
{
	/* telemetry status supported only on first TELEMETRY_STATUS_ORB_ID_NUM mavlink channels */
	if (_mavlink->get_channel() < (mavlink_channel_t)IPC_MULTI_MAX_INSTANCES) {
		mavlink_fcinfo_t hb;
		mavlink_msg_fcinfo_decode(msg, &hb);

		/* ignore own heartbeats, accept only heartbeats from GCS */
		if (msg->sysid != mavlink_system.sysid) {
			info_raw.timestamp = hrt_absolute_time();
			info_raw.eular[0] = hb.eular_x;
			info_raw.eular[1] = hb.eular_y;
			info_raw.eular[2] = hb.eular_z;
			info_raw.target_eular[0] = hb.target_eular_x / 100.0f * M_DEG_TO_RAD_F;
			info_raw.target_eular[1] = hb.target_eular_y / 100.0f * M_DEG_TO_RAD_F;
			info_raw.target_eular[2] = hb.target_eular_z / 100.0f * M_DEG_TO_RAD_F;
			info_raw.target_yaw_rate = hb.target_yaw_rate;
			info_raw.flighting = hb.flighting;
			ipc_push(IPC_ID(fc_information), _info_pub, &info_raw);
		}
	}
}

void MavlinkReceiver::handle_message_compensation(mavlink_message_t *msg)
{
	/* telemetry status supported only on first TELEMETRY_STATUS_ORB_ID_NUM mavlink channels */
	if (_mavlink->get_channel() < (mavlink_channel_t)IPC_MULTI_MAX_INSTANCES) {
		mavlink_compensation_t hb;
		mavlink_msg_compensation_decode(msg, &hb);

		/* ignore own heartbeats, accept only heartbeats from GCS */
		if (msg->sysid != mavlink_system.sysid) {
			mag_raw.timestamp = hrt_absolute_time();
			vel_raw.timestamp = mag_raw.timestamp;
			mag_raw.field[0] = hb.mag_x;
			mag_raw.field[1] = hb.mag_y;
			mag_raw.field[2] = hb.mag_z;
			mag_raw.decl = hb.decl;
			vel_raw.vel_ned_m_s[0] = hb.acc_x;
			vel_raw.vel_ned_m_s[1] = hb.acc_y;
			vel_raw.vel_ned_m_s[2] = hb.acc_z;

			ipc_push(IPC_ID(sensor_mag), _mag_pub, &mag_raw);
			ipc_push(IPC_ID(sensor_gps), _vel_pub, &vel_raw);
		}
	}
}

/**
 * Receive data from UART.
 */
void MavlinkReceiver::receive_thread(void *arg)
{
	// poll timeout in ms. Also defines the max update frequency of the mission & param manager, etc.
	const int timeout = 10;
	OS_ERR   err;

	/* the serial port buffers internally as well, we just need to fit a small chunk */
	uint8_t buf[64];

	mavlink_message_t msg;

	uint16_t nread = 0;
	hrt_abstime last_send_update = 0;

	_info_pub    = ipc_active(IPC_ID(fc_information), &info_raw);
	_control_pub = ipc_active(IPC_ID(gimbal_control), &control_raw);
	_mag_pub     = ipc_active(IPC_ID(sensor_mag), &mag_raw);
	_vel_pub     = ipc_active(IPC_ID(sensor_gps), &vel_raw);

	pref_receive_interval = perf_alloc(PC_INTERVAL, "recv_int");

	while (DEF_TRUE) {

		perf_count(pref_receive_interval);
		/*
			* to avoid reading very small chunks wait for data before reading
			* this is designed to target one message, so >20 bytes at a time
			*/
		const unsigned character_count = 20;

		nread = uart_read(_mavlink->telemetry_port, buf, sizeof(buf));

		/* non-blocking read. read may return negative values */
		if (nread < (uint16_t)character_count) {
			unsigned sleeptime = (unsigned)((1.0f / (_mavlink->get_baudrate() / 10)) * character_count * 1000);
			OSTimeDly((OS_TICK)(TICK_PER_MS * sleeptime), OS_OPT_TIME_DLY, &err);
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

		hrt_abstime t = hrt_absolute_time();

		if (t - last_send_update > timeout * 1000) {

			_parameters_manager.send(t);

			last_send_update = t;
		}

		// 100Hz loop
        OSTimeDly((OS_TICK)(TICK_PER_MS * 10), OS_OPT_TIME_PERIODIC, &err);
	}
}

void MavlinkReceiver::start_helper(void *context)
{
	MavlinkReceiver *rcv = new MavlinkReceiver((Mavlink *)context);

	if (!rcv) {
		Info_Debug("alloc failed");
		return;
	}
	rcv->receive_thread(nullptr);

	delete rcv;
}

void
MavlinkReceiver::receive_start(Mavlink *parent)
{
	OS_ERR   err;
    
	int ic = parent->get_instance_id();

	OSTaskCreate(&AppTaskRECVTCB[ic],
                  (char*)"TaskRecv",
                  MavlinkReceiver::start_helper,
                  (void *)parent,
                  APP_CFG_TASK_RECV_PRIO,
                  &AppTaskRECVStk[ic][0u],
                  AppTaskRECVStk[ic][APP_CFG_TASK_RECV_STK_SIZE / 10u],
                  APP_CFG_TASK_RECV_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);
    if(err != OS_ERR_NONE) {
        Info_Debug("telemetry task start error!\n");
    }
}
