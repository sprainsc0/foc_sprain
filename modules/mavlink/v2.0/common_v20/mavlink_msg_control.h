#pragma once
// MESSAGE CONTROL PACKING

#define MAVLINK_MSG_ID_CONTROL 54

MAVPACKED(
typedef struct __mavlink_control_t {
 uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.*/
 float rate_x; /*<  rate control on X-axis*/
 float rate_y; /*<  rate control on Y-axis*/
 float rate_z; /*<  rate control on Z-axis*/
 float angle_x; /*<  angle control on X-axis*/
 float angle_y; /*<  angle control on Y-axis*/
 float angle_z; /*<  angle control on Z-axis*/
 uint8_t mode; /*<  control mode*/
}) mavlink_control_t;

#define MAVLINK_MSG_ID_CONTROL_LEN 33
#define MAVLINK_MSG_ID_CONTROL_MIN_LEN 33
#define MAVLINK_MSG_ID_54_LEN 33
#define MAVLINK_MSG_ID_54_MIN_LEN 33

#define MAVLINK_MSG_ID_CONTROL_CRC 86
#define MAVLINK_MSG_ID_54_CRC 86



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_CONTROL { \
    54, \
    "CONTROL", \
    8, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_control_t, time_usec) }, \
         { "rate_x", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_control_t, rate_x) }, \
         { "rate_y", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_control_t, rate_y) }, \
         { "rate_z", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_control_t, rate_z) }, \
         { "angle_x", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_control_t, angle_x) }, \
         { "angle_y", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_control_t, angle_y) }, \
         { "angle_z", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_control_t, angle_z) }, \
         { "mode", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_control_t, mode) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_CONTROL { \
    "CONTROL", \
    8, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_control_t, time_usec) }, \
         { "rate_x", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_control_t, rate_x) }, \
         { "rate_y", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_control_t, rate_y) }, \
         { "rate_z", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_control_t, rate_z) }, \
         { "angle_x", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_control_t, angle_x) }, \
         { "angle_y", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_control_t, angle_y) }, \
         { "angle_z", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_control_t, angle_z) }, \
         { "mode", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_control_t, mode) }, \
         } \
}
#endif

/**
 * @brief Pack a control message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param rate_x  rate control on X-axis
 * @param rate_y  rate control on Y-axis
 * @param rate_z  rate control on Z-axis
 * @param angle_x  angle control on X-axis
 * @param angle_y  angle control on Y-axis
 * @param angle_z  angle control on Z-axis
 * @param mode  control mode
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_control_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_usec, float rate_x, float rate_y, float rate_z, float angle_x, float angle_y, float angle_z, uint8_t mode)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONTROL_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, rate_x);
    _mav_put_float(buf, 12, rate_y);
    _mav_put_float(buf, 16, rate_z);
    _mav_put_float(buf, 20, angle_x);
    _mav_put_float(buf, 24, angle_y);
    _mav_put_float(buf, 28, angle_z);
    _mav_put_uint8_t(buf, 32, mode);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONTROL_LEN);
#else
    mavlink_control_t packet;
    packet.time_usec = time_usec;
    packet.rate_x = rate_x;
    packet.rate_y = rate_y;
    packet.rate_z = rate_z;
    packet.angle_x = angle_x;
    packet.angle_y = angle_y;
    packet.angle_z = angle_z;
    packet.mode = mode;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONTROL_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CONTROL;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
}

/**
 * @brief Pack a control message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param rate_x  rate control on X-axis
 * @param rate_y  rate control on Y-axis
 * @param rate_z  rate control on Z-axis
 * @param angle_x  angle control on X-axis
 * @param angle_y  angle control on Y-axis
 * @param angle_z  angle control on Z-axis
 * @param mode  control mode
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_control_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_usec,float rate_x,float rate_y,float rate_z,float angle_x,float angle_y,float angle_z,uint8_t mode)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONTROL_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, rate_x);
    _mav_put_float(buf, 12, rate_y);
    _mav_put_float(buf, 16, rate_z);
    _mav_put_float(buf, 20, angle_x);
    _mav_put_float(buf, 24, angle_y);
    _mav_put_float(buf, 28, angle_z);
    _mav_put_uint8_t(buf, 32, mode);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONTROL_LEN);
#else
    mavlink_control_t packet;
    packet.time_usec = time_usec;
    packet.rate_x = rate_x;
    packet.rate_y = rate_y;
    packet.rate_z = rate_z;
    packet.angle_x = angle_x;
    packet.angle_y = angle_y;
    packet.angle_z = angle_z;
    packet.mode = mode;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONTROL_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CONTROL;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
}

/**
 * @brief Encode a control struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param control C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_control_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_control_t* control)
{
    return mavlink_msg_control_pack(system_id, component_id, msg, (uint64_t )(control->time_usec), (float )(control->rate_x), (float )(control->rate_y), (float )(control->rate_z), (float )(control->angle_x), (float )(control->angle_y), (float )(control->angle_z), (uint8_t )(control->mode));
}

/**
 * @brief Encode a control struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param control C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_control_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_control_t* control)
{
    return mavlink_msg_control_pack_chan(system_id, component_id, chan, msg, (uint64_t )(control->time_usec), (float )(control->rate_x), (float )(control->rate_y), (float )(control->rate_z), (float )(control->angle_x), (float )(control->angle_y), (float )(control->angle_z), (uint8_t )(control->mode));
}

/**
 * @brief Send a control message
 * @param chan MAVLink channel to send the message
 *
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param rate_x  rate control on X-axis
 * @param rate_y  rate control on Y-axis
 * @param rate_z  rate control on Z-axis
 * @param angle_x  angle control on X-axis
 * @param angle_y  angle control on Y-axis
 * @param angle_z  angle control on Z-axis
 * @param mode  control mode
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_control_send(mavlink_channel_t chan, uint64_t time_usec, float rate_x, float rate_y, float rate_z, float angle_x, float angle_y, float angle_z, uint8_t mode)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONTROL_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, rate_x);
    _mav_put_float(buf, 12, rate_y);
    _mav_put_float(buf, 16, rate_z);
    _mav_put_float(buf, 20, angle_x);
    _mav_put_float(buf, 24, angle_y);
    _mav_put_float(buf, 28, angle_z);
    _mav_put_uint8_t(buf, 32, mode);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, buf, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#else
    mavlink_control_t packet;
    packet.time_usec = time_usec;
    packet.rate_x = rate_x;
    packet.rate_y = rate_y;
    packet.rate_z = rate_z;
    packet.angle_x = angle_x;
    packet.angle_y = angle_y;
    packet.angle_z = angle_z;
    packet.mode = mode;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, (const char *)&packet, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#endif
}

/**
 * @brief Send a control message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_control_send_struct(mavlink_channel_t chan, const mavlink_control_t* control)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_control_send(chan, control->time_usec, control->rate_x, control->rate_y, control->rate_z, control->angle_x, control->angle_y, control->angle_z, control->mode);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, (const char *)control, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#endif
}

#if MAVLINK_MSG_ID_CONTROL_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_control_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_usec, float rate_x, float rate_y, float rate_z, float angle_x, float angle_y, float angle_z, uint8_t mode)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, rate_x);
    _mav_put_float(buf, 12, rate_y);
    _mav_put_float(buf, 16, rate_z);
    _mav_put_float(buf, 20, angle_x);
    _mav_put_float(buf, 24, angle_y);
    _mav_put_float(buf, 28, angle_z);
    _mav_put_uint8_t(buf, 32, mode);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, buf, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#else
    mavlink_control_t *packet = (mavlink_control_t *)msgbuf;
    packet->time_usec = time_usec;
    packet->rate_x = rate_x;
    packet->rate_y = rate_y;
    packet->rate_z = rate_z;
    packet->angle_x = angle_x;
    packet->angle_y = angle_y;
    packet->angle_z = angle_z;
    packet->mode = mode;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, (const char *)packet, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#endif
}
#endif

#endif

// MESSAGE CONTROL UNPACKING


/**
 * @brief Get field time_usec from control message
 *
 * @return [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 */
static inline uint64_t mavlink_msg_control_get_time_usec(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field rate_x from control message
 *
 * @return  rate control on X-axis
 */
static inline float mavlink_msg_control_get_rate_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field rate_y from control message
 *
 * @return  rate control on Y-axis
 */
static inline float mavlink_msg_control_get_rate_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field rate_z from control message
 *
 * @return  rate control on Z-axis
 */
static inline float mavlink_msg_control_get_rate_z(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field angle_x from control message
 *
 * @return  angle control on X-axis
 */
static inline float mavlink_msg_control_get_angle_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field angle_y from control message
 *
 * @return  angle control on Y-axis
 */
static inline float mavlink_msg_control_get_angle_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Get field angle_z from control message
 *
 * @return  angle control on Z-axis
 */
static inline float mavlink_msg_control_get_angle_z(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  28);
}

/**
 * @brief Get field mode from control message
 *
 * @return  control mode
 */
static inline uint8_t mavlink_msg_control_get_mode(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  32);
}

/**
 * @brief Decode a control message into a struct
 *
 * @param msg The message to decode
 * @param control C-struct to decode the message contents into
 */
static inline void mavlink_msg_control_decode(const mavlink_message_t* msg, mavlink_control_t* control)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    control->time_usec = mavlink_msg_control_get_time_usec(msg);
    control->rate_x = mavlink_msg_control_get_rate_x(msg);
    control->rate_y = mavlink_msg_control_get_rate_y(msg);
    control->rate_z = mavlink_msg_control_get_rate_z(msg);
    control->angle_x = mavlink_msg_control_get_angle_x(msg);
    control->angle_y = mavlink_msg_control_get_angle_y(msg);
    control->angle_z = mavlink_msg_control_get_angle_z(msg);
    control->mode = mavlink_msg_control_get_mode(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_CONTROL_LEN? msg->len : MAVLINK_MSG_ID_CONTROL_LEN;
        memset(control, 0, MAVLINK_MSG_ID_CONTROL_LEN);
    memcpy(control, _MAV_PAYLOAD(msg), len);
#endif
}
