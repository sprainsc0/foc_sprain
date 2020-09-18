#pragma once
// MESSAGE FCINFO PACKING

#define MAVLINK_MSG_ID_FCINFO 56

MAVPACKED(
typedef struct __mavlink_fcinfo_t {
 uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.*/
 float eular_x; /*<  eular X-axis*/
 float eular_y; /*<  eular Y-axis*/
 float eular_z; /*<  eular Z-axis*/
 float target_eular_x; /*<  target eular X-axis*/
 float target_eular_y; /*<  target eular Y-axis*/
 float target_eular_z; /*<  target eular Z-axis*/
 float target_yaw_rate; /*<  RC input yaw rate*/
 uint8_t flighting; /*<  armed flag*/
}) mavlink_fcinfo_t;

#define MAVLINK_MSG_ID_FCINFO_LEN 37
#define MAVLINK_MSG_ID_FCINFO_MIN_LEN 37
#define MAVLINK_MSG_ID_56_LEN 37
#define MAVLINK_MSG_ID_56_MIN_LEN 37

#define MAVLINK_MSG_ID_FCINFO_CRC 146
#define MAVLINK_MSG_ID_56_CRC 146



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_FCINFO { \
    56, \
    "FCINFO", \
    9, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_fcinfo_t, time_usec) }, \
         { "eular_x", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_fcinfo_t, eular_x) }, \
         { "eular_y", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_fcinfo_t, eular_y) }, \
         { "eular_z", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_fcinfo_t, eular_z) }, \
         { "target_eular_x", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_fcinfo_t, target_eular_x) }, \
         { "target_eular_y", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_fcinfo_t, target_eular_y) }, \
         { "target_eular_z", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_fcinfo_t, target_eular_z) }, \
         { "target_yaw_rate", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_fcinfo_t, target_yaw_rate) }, \
         { "flighting", NULL, MAVLINK_TYPE_UINT8_T, 0, 36, offsetof(mavlink_fcinfo_t, flighting) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_FCINFO { \
    "FCINFO", \
    9, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_fcinfo_t, time_usec) }, \
         { "eular_x", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_fcinfo_t, eular_x) }, \
         { "eular_y", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_fcinfo_t, eular_y) }, \
         { "eular_z", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_fcinfo_t, eular_z) }, \
         { "target_eular_x", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_fcinfo_t, target_eular_x) }, \
         { "target_eular_y", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_fcinfo_t, target_eular_y) }, \
         { "target_eular_z", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_fcinfo_t, target_eular_z) }, \
         { "target_yaw_rate", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_fcinfo_t, target_yaw_rate) }, \
         { "flighting", NULL, MAVLINK_TYPE_UINT8_T, 0, 36, offsetof(mavlink_fcinfo_t, flighting) }, \
         } \
}
#endif

/**
 * @brief Pack a fcinfo message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param eular_x  eular X-axis
 * @param eular_y  eular Y-axis
 * @param eular_z  eular Z-axis
 * @param target_eular_x  target eular X-axis
 * @param target_eular_y  target eular Y-axis
 * @param target_eular_z  target eular Z-axis
 * @param target_yaw_rate  RC input yaw rate
 * @param flighting  armed flag
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_fcinfo_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_usec, float eular_x, float eular_y, float eular_z, float target_eular_x, float target_eular_y, float target_eular_z, float target_yaw_rate, uint8_t flighting)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_FCINFO_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, eular_x);
    _mav_put_float(buf, 12, eular_y);
    _mav_put_float(buf, 16, eular_z);
    _mav_put_float(buf, 20, target_eular_x);
    _mav_put_float(buf, 24, target_eular_y);
    _mav_put_float(buf, 28, target_eular_z);
    _mav_put_float(buf, 32, target_yaw_rate);
    _mav_put_uint8_t(buf, 36, flighting);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_FCINFO_LEN);
#else
    mavlink_fcinfo_t packet;
    packet.time_usec = time_usec;
    packet.eular_x = eular_x;
    packet.eular_y = eular_y;
    packet.eular_z = eular_z;
    packet.target_eular_x = target_eular_x;
    packet.target_eular_y = target_eular_y;
    packet.target_eular_z = target_eular_z;
    packet.target_yaw_rate = target_yaw_rate;
    packet.flighting = flighting;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_FCINFO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_FCINFO;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_FCINFO_MIN_LEN, MAVLINK_MSG_ID_FCINFO_LEN, MAVLINK_MSG_ID_FCINFO_CRC);
}

/**
 * @brief Pack a fcinfo message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param eular_x  eular X-axis
 * @param eular_y  eular Y-axis
 * @param eular_z  eular Z-axis
 * @param target_eular_x  target eular X-axis
 * @param target_eular_y  target eular Y-axis
 * @param target_eular_z  target eular Z-axis
 * @param target_yaw_rate  RC input yaw rate
 * @param flighting  armed flag
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_fcinfo_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_usec,float eular_x,float eular_y,float eular_z,float target_eular_x,float target_eular_y,float target_eular_z,float target_yaw_rate,uint8_t flighting)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_FCINFO_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, eular_x);
    _mav_put_float(buf, 12, eular_y);
    _mav_put_float(buf, 16, eular_z);
    _mav_put_float(buf, 20, target_eular_x);
    _mav_put_float(buf, 24, target_eular_y);
    _mav_put_float(buf, 28, target_eular_z);
    _mav_put_float(buf, 32, target_yaw_rate);
    _mav_put_uint8_t(buf, 36, flighting);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_FCINFO_LEN);
#else
    mavlink_fcinfo_t packet;
    packet.time_usec = time_usec;
    packet.eular_x = eular_x;
    packet.eular_y = eular_y;
    packet.eular_z = eular_z;
    packet.target_eular_x = target_eular_x;
    packet.target_eular_y = target_eular_y;
    packet.target_eular_z = target_eular_z;
    packet.target_yaw_rate = target_yaw_rate;
    packet.flighting = flighting;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_FCINFO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_FCINFO;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_FCINFO_MIN_LEN, MAVLINK_MSG_ID_FCINFO_LEN, MAVLINK_MSG_ID_FCINFO_CRC);
}

/**
 * @brief Encode a fcinfo struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param fcinfo C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_fcinfo_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_fcinfo_t* fcinfo)
{
    return mavlink_msg_fcinfo_pack(system_id, component_id, msg, (uint64_t )(fcinfo->time_usec), (float )(fcinfo->eular_x), (float )(fcinfo->eular_y), (float )(fcinfo->eular_z), (float )(fcinfo->target_eular_x), (float )(fcinfo->target_eular_y), (float )(fcinfo->target_eular_z), (float )(fcinfo->target_yaw_rate), (uint8_t )(fcinfo->flighting));
}

/**
 * @brief Encode a fcinfo struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param fcinfo C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_fcinfo_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_fcinfo_t* fcinfo)
{
    return mavlink_msg_fcinfo_pack_chan(system_id, component_id, chan, msg, (uint64_t )(fcinfo->time_usec), (float )(fcinfo->eular_x), (float )(fcinfo->eular_y), (float )(fcinfo->eular_z), (float )(fcinfo->target_eular_x), (float )(fcinfo->target_eular_y), (float )(fcinfo->target_eular_z), (float )(fcinfo->target_yaw_rate), (uint8_t )(fcinfo->flighting));
}

/**
 * @brief Send a fcinfo message
 * @param chan MAVLink channel to send the message
 *
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param eular_x  eular X-axis
 * @param eular_y  eular Y-axis
 * @param eular_z  eular Z-axis
 * @param target_eular_x  target eular X-axis
 * @param target_eular_y  target eular Y-axis
 * @param target_eular_z  target eular Z-axis
 * @param target_yaw_rate  RC input yaw rate
 * @param flighting  armed flag
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_fcinfo_send(mavlink_channel_t chan, uint64_t time_usec, float eular_x, float eular_y, float eular_z, float target_eular_x, float target_eular_y, float target_eular_z, float target_yaw_rate, uint8_t flighting)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_FCINFO_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, eular_x);
    _mav_put_float(buf, 12, eular_y);
    _mav_put_float(buf, 16, eular_z);
    _mav_put_float(buf, 20, target_eular_x);
    _mav_put_float(buf, 24, target_eular_y);
    _mav_put_float(buf, 28, target_eular_z);
    _mav_put_float(buf, 32, target_yaw_rate);
    _mav_put_uint8_t(buf, 36, flighting);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FCINFO, buf, MAVLINK_MSG_ID_FCINFO_MIN_LEN, MAVLINK_MSG_ID_FCINFO_LEN, MAVLINK_MSG_ID_FCINFO_CRC);
#else
    mavlink_fcinfo_t packet;
    packet.time_usec = time_usec;
    packet.eular_x = eular_x;
    packet.eular_y = eular_y;
    packet.eular_z = eular_z;
    packet.target_eular_x = target_eular_x;
    packet.target_eular_y = target_eular_y;
    packet.target_eular_z = target_eular_z;
    packet.target_yaw_rate = target_yaw_rate;
    packet.flighting = flighting;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FCINFO, (const char *)&packet, MAVLINK_MSG_ID_FCINFO_MIN_LEN, MAVLINK_MSG_ID_FCINFO_LEN, MAVLINK_MSG_ID_FCINFO_CRC);
#endif
}

/**
 * @brief Send a fcinfo message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_fcinfo_send_struct(mavlink_channel_t chan, const mavlink_fcinfo_t* fcinfo)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_fcinfo_send(chan, fcinfo->time_usec, fcinfo->eular_x, fcinfo->eular_y, fcinfo->eular_z, fcinfo->target_eular_x, fcinfo->target_eular_y, fcinfo->target_eular_z, fcinfo->target_yaw_rate, fcinfo->flighting);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FCINFO, (const char *)fcinfo, MAVLINK_MSG_ID_FCINFO_MIN_LEN, MAVLINK_MSG_ID_FCINFO_LEN, MAVLINK_MSG_ID_FCINFO_CRC);
#endif
}

#if MAVLINK_MSG_ID_FCINFO_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_fcinfo_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_usec, float eular_x, float eular_y, float eular_z, float target_eular_x, float target_eular_y, float target_eular_z, float target_yaw_rate, uint8_t flighting)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, eular_x);
    _mav_put_float(buf, 12, eular_y);
    _mav_put_float(buf, 16, eular_z);
    _mav_put_float(buf, 20, target_eular_x);
    _mav_put_float(buf, 24, target_eular_y);
    _mav_put_float(buf, 28, target_eular_z);
    _mav_put_float(buf, 32, target_yaw_rate);
    _mav_put_uint8_t(buf, 36, flighting);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FCINFO, buf, MAVLINK_MSG_ID_FCINFO_MIN_LEN, MAVLINK_MSG_ID_FCINFO_LEN, MAVLINK_MSG_ID_FCINFO_CRC);
#else
    mavlink_fcinfo_t *packet = (mavlink_fcinfo_t *)msgbuf;
    packet->time_usec = time_usec;
    packet->eular_x = eular_x;
    packet->eular_y = eular_y;
    packet->eular_z = eular_z;
    packet->target_eular_x = target_eular_x;
    packet->target_eular_y = target_eular_y;
    packet->target_eular_z = target_eular_z;
    packet->target_yaw_rate = target_yaw_rate;
    packet->flighting = flighting;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FCINFO, (const char *)packet, MAVLINK_MSG_ID_FCINFO_MIN_LEN, MAVLINK_MSG_ID_FCINFO_LEN, MAVLINK_MSG_ID_FCINFO_CRC);
#endif
}
#endif

#endif

// MESSAGE FCINFO UNPACKING


/**
 * @brief Get field time_usec from fcinfo message
 *
 * @return [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 */
static inline uint64_t mavlink_msg_fcinfo_get_time_usec(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field eular_x from fcinfo message
 *
 * @return  eular X-axis
 */
static inline float mavlink_msg_fcinfo_get_eular_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field eular_y from fcinfo message
 *
 * @return  eular Y-axis
 */
static inline float mavlink_msg_fcinfo_get_eular_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field eular_z from fcinfo message
 *
 * @return  eular Z-axis
 */
static inline float mavlink_msg_fcinfo_get_eular_z(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field target_eular_x from fcinfo message
 *
 * @return  target eular X-axis
 */
static inline float mavlink_msg_fcinfo_get_target_eular_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field target_eular_y from fcinfo message
 *
 * @return  target eular Y-axis
 */
static inline float mavlink_msg_fcinfo_get_target_eular_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Get field target_eular_z from fcinfo message
 *
 * @return  target eular Z-axis
 */
static inline float mavlink_msg_fcinfo_get_target_eular_z(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  28);
}

/**
 * @brief Get field target_yaw_rate from fcinfo message
 *
 * @return  RC input yaw rate
 */
static inline float mavlink_msg_fcinfo_get_target_yaw_rate(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  32);
}

/**
 * @brief Get field flighting from fcinfo message
 *
 * @return  armed flag
 */
static inline uint8_t mavlink_msg_fcinfo_get_flighting(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  36);
}

/**
 * @brief Decode a fcinfo message into a struct
 *
 * @param msg The message to decode
 * @param fcinfo C-struct to decode the message contents into
 */
static inline void mavlink_msg_fcinfo_decode(const mavlink_message_t* msg, mavlink_fcinfo_t* fcinfo)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    fcinfo->time_usec = mavlink_msg_fcinfo_get_time_usec(msg);
    fcinfo->eular_x = mavlink_msg_fcinfo_get_eular_x(msg);
    fcinfo->eular_y = mavlink_msg_fcinfo_get_eular_y(msg);
    fcinfo->eular_z = mavlink_msg_fcinfo_get_eular_z(msg);
    fcinfo->target_eular_x = mavlink_msg_fcinfo_get_target_eular_x(msg);
    fcinfo->target_eular_y = mavlink_msg_fcinfo_get_target_eular_y(msg);
    fcinfo->target_eular_z = mavlink_msg_fcinfo_get_target_eular_z(msg);
    fcinfo->target_yaw_rate = mavlink_msg_fcinfo_get_target_yaw_rate(msg);
    fcinfo->flighting = mavlink_msg_fcinfo_get_flighting(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_FCINFO_LEN? msg->len : MAVLINK_MSG_ID_FCINFO_LEN;
        memset(fcinfo, 0, MAVLINK_MSG_ID_FCINFO_LEN);
    memcpy(fcinfo, _MAV_PAYLOAD(msg), len);
#endif
}
