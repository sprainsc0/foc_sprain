#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <B_Math.h>
#include "mavlink_main.h"
#include "mavlink_messages.h"

#include <topics/vehicle_attitude.h>
#include <topics/sensor_imu.h>
#include <topics/sensor_bias.h>
#include "IPC.h"

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
	MavlinkOrbSubscription *_status_sub;

	/* do not allow top copying this class */
	MavlinkStreamHeartbeat(MavlinkStreamHeartbeat &);
	MavlinkStreamHeartbeat &operator = (const MavlinkStreamHeartbeat &);

protected:
	explicit MavlinkStreamHeartbeat(Mavlink *mavlink) : MavlinkStream(mavlink),
		_status_sub(_mavlink->add_orb_subscription(IPC_ID(vehicle_status)))
	{}

	bool send(const hrt_abstime t)
	{
		struct vehicle_status_s status = {};

		/* always send the heartbeat, independent of the update status of the topics */
		if (!_status_sub->update(&status)) {
			/* if topic update failed fill it with defaults */
			memset(&status, 0, sizeof(status));
		}
		
		uint8_t base_mode = 0;
		uint32_t custom_mode = 0;
		uint8_t system_status = 0;

		mavlink_msg_heartbeat_send(_mavlink->get_channel(), _mavlink->get_system_type(), 0,
					   base_mode, custom_mode, system_status);

		return true;
	}
};


class MavlinkStreamAttitude : public MavlinkStream
{
public:
	const char *get_name() const
	{
		return MavlinkStreamAttitude::get_name_static();
	}

	static const char *get_name_static()
	{
		return "ATTITUDE";
	}

	static uint16_t get_id_static()
	{
		return MAVLINK_MSG_ID_ATTITUDE;
	}

	uint16_t get_id()
	{
		return get_id_static();
	}

	static MavlinkStream *new_instance(Mavlink *mavlink)
	{
		return new MavlinkStreamAttitude(mavlink);
	}

	unsigned get_size()
	{
		return MAVLINK_MSG_ID_ATTITUDE_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
	}

private:
	MavlinkOrbSubscription *_att_sub;

	/* do not allow top copying this class */
	MavlinkStreamAttitude(MavlinkStreamAttitude &);
	MavlinkStreamAttitude &operator = (const MavlinkStreamAttitude &);


protected:
	explicit MavlinkStreamAttitude(Mavlink *mavlink) : MavlinkStream(mavlink),
		_att_sub(_mavlink->add_orb_subscription(IPC_ID(vehicle_attitude)))
	{}

	bool send(const hrt_abstime t)
	{
		vehicle_attitude_s att;

		if (_att_sub->update(&att) && _mavlink->is_connected()) {
			mavlink_attitude_t msg = {};
			Quaternion qut;
            qut = Quaternion(att.q[0],att.q[1],att.q[2],att.q[3]);
              
			msg.time_boot_ms = att.timestamp / 1000;
			msg.roll = qut.get_euler_roll() * RAD_TO_DEG;
			msg.pitch = qut.get_euler_pitch() * RAD_TO_DEG;
			msg.yaw = qut.get_euler_yaw() * RAD_TO_DEG;
			msg.rollspeed = 0;
			msg.pitchspeed = 0;
			msg.yawspeed = 0;

			mavlink_msg_attitude_send_struct(_mavlink->get_channel(), &msg);

			return true;
		}

		return false;
	}
};

class MavlinkStreamIMU : public MavlinkStream
{
public:
	const char *get_name() const
	{
		return MavlinkStreamIMU::get_name_static();
	}

	static const char *get_name_static()
	{
		return "IMU";
	}

	static uint16_t get_id_static()
	{
		return MAVLINK_MSG_ID_SCALED_IMU;
	}

	uint16_t get_id()
	{
		return get_id_static();
	}

	static MavlinkStream *new_instance(Mavlink *mavlink)
	{
		return new MavlinkStreamIMU(mavlink);
	}

	unsigned get_size()
	{
		return MAVLINK_MSG_ID_SCALED_IMU_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
	}

private:
	MavlinkOrbSubscription *_imu_sub;
	MavlinkOrbSubscription *_bais_sub;

	/* do not allow top copying this class */
	MavlinkStreamIMU(MavlinkStreamIMU &);
	MavlinkStreamIMU &operator = (const MavlinkStreamIMU &);


protected:
	explicit MavlinkStreamIMU(Mavlink *mavlink) : MavlinkStream(mavlink),
		_imu_sub(_mavlink->add_orb_subscription(IPC_ID(sensor_imu))),
		_bais_sub(_mavlink->add_orb_subscription(IPC_ID(sensor_bias)))
	{}

	bool send(const hrt_abstime t)
	{
		sensor_imu_s imu;
		sensor_bias_s bais;

		if (_bais_sub->update(&bais) && _imu_sub->update(&imu) && _mavlink->is_connected()) {
			mavlink_scaled_imu_t msg = {};
			
			msg.time_boot_ms = imu.timestamp / 1000;
			msg.xacc = (int16_t)(imu.accel[0] * 1000);
			msg.yacc = (int16_t)(imu.accel[1] * 1000);
			msg.zacc = (int16_t)(imu.accel[2] * 1000);

			msg.xgyro = (int16_t)(imu.gyro[0] * 1000);
			msg.ygyro = (int16_t)(imu.gyro[1] * 1000);
			msg.zgyro = (int16_t)(imu.gyro[2] * 1000);

			msg.xmag  = (int16_t)(bais.gyro_bias[0] * 10000);
			msg.ymag  = (int16_t)(bais.gyro_bias[1] * 10000);
			msg.zmag  = (int16_t)(bais.gyro_bias[2] * 10000);

			mavlink_msg_scaled_imu_send_struct(_mavlink->get_channel(), &msg);

			return true;
		}

		return false;
	}
};


static const StreamListItem streams_list[] = {
	StreamListItem(&MavlinkStreamHeartbeat::new_instance, &MavlinkStreamHeartbeat::get_name_static, &MavlinkStreamHeartbeat::get_id_static),
	StreamListItem(&MavlinkStreamAttitude::new_instance, &MavlinkStreamAttitude::get_name_static, &MavlinkStreamAttitude::get_id_static),
	StreamListItem(&MavlinkStreamIMU::new_instance, &MavlinkStreamIMU::get_name_static, &MavlinkStreamIMU::get_id_static),
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
