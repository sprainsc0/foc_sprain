#pragma once
// MESSAGE COMPENSATION PACKING

#define MAVLINK_MSG_ID_COMPENSATION 55

MAVPACKED(
typedef struct __mavlink_compensation_t {
 uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.*/
 float mag_x; /*<  mag X-axis*/
 float mag_y; /*<  mag Y-axis*/
 float mag_z; /*<  mag Z-axis*/
 float acc_x; /*<  acc X-axis*/
 float acc_y; /*<  acc Y-axis*/
 float acc_z; /*<  acc Z-axis*/
 float decl; /*<  decl*/
}) mavlink_compensation_t;

#define MAVLINK_MSG_ID_COMPENSATION_LEN 36
#define MAVLINK_MSG_ID_COMPENSATION_MIN_LEN 36
#define MAVLINK_MSG_ID_55_LEN 36
#define MAVLINK_MSG_ID_55_MIN_LEN 36

#define MAVLINK_MSG_ID_COMPENSATION_CRC 88
#define MAVLINK_MSG_ID_55_CRC 88



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_COMPENSATION { \
    55, \
    "COMPENSATION", \
    8, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_compensation_t, time_usec) }, \
         { "mag_x", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_compensation_t, mag_x) }, \
         { "mag_y", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_compensation_t, mag_y) }, \
         { "mag_z", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_compensation_t, mag_z) }, \
         { "acc_x", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_compensation_t, acc_x) }, \
         { "acc_y", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_compensation_t, acc_y) }, \
         { "acc_z", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_compensation_t, acc_z) }, \
         { "decl", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_compensation_t, decl) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_COMPENSATION { \
    "COMPENSATION", \
    8, \
    {  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_compensation_t, time_usec) }, \
         { "mag_x", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_compensation_t, mag_x) }, \
         { "mag_y", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_compensation_t, mag_y) }, \
         { "mag_z", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_compensation_t, mag_z) }, \
         { "acc_x", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_compensation_t, acc_x) }, \
         { "acc_y", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_compensation_t, acc_y) }, \
         { "acc_z", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_compensation_t, acc_z) }, \
         { "decl", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_compensation_t, decl) }, \
         } \
}
#endif

/**
 * @brief Pack a compensation message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param mag_x  mag X-axis
 * @param mag_y  mag Y-axis
 * @param mag_z  mag Z-axis
 * @param acc_x  acc X-axis
 * @param acc_y  acc Y-axis
 * @param acc_z  acc Z-axis
 * @param decl  decl
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_compensation_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_usec, float mag_x, float mag_y, float mag_z, float acc_x, float acc_y, float acc_z, float decl)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_COMPENSATION_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, mag_x);
    _mav_put_float(buf, 12, mag_y);
    _mav_put_float(buf, 16, mag_z);
    _mav_put_float(buf, 20, acc_x);
    _mav_put_float(buf, 24, acc_y);
    _mav_put_float(buf, 28, acc_z);
    _mav_put_float(buf, 32, decl);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_COMPENSATION_LEN);
#else
    mavlink_compensation_t packet;
    packet.time_usec = time_usec;
    packet.mag_x = mag_x;
    packet.mag_y = mag_y;
    packet.mag_z = mag_z;
    packet.acc_x = acc_x;
    packet.acc_y = acc_y;
    packet.acc_z = acc_z;
    packet.decl = decl;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_COMPENSATION_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_COMPENSATION;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_COMPENSATION_MIN_LEN, MAVLINK_MSG_ID_COMPENSATION_LEN, MAVLINK_MSG_ID_COMPENSATION_CRC);
}

/**
 * @brief Pack a compensation message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param mag_x  mag X-axis
 * @param mag_y  mag Y-axis
 * @param mag_z  mag Z-axis
 * @param acc_x  acc X-axis
 * @param acc_y  acc Y-axis
 * @param acc_z  acc Z-axis
 * @param decl  decl
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_compensation_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_usec,float mag_x,float mag_y,float mag_z,float acc_x,float acc_y,float acc_z,float decl)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_COMPENSATION_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, mag_x);
    _mav_put_float(buf, 12, mag_y);
    _mav_put_float(buf, 16, mag_z);
    _mav_put_float(buf, 20, acc_x);
    _mav_put_float(buf, 24, acc_y);
    _mav_put_float(buf, 28, acc_z);
    _mav_put_float(buf, 32, decl);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_COMPENSATION_LEN);
#else
    mavlink_compensation_t packet;
    packet.time_usec = time_usec;
    packet.mag_x = mag_x;
    packet.mag_y = mag_y;
    packet.mag_z = mag_z;
    packet.acc_x = acc_x;
    packet.acc_y = acc_y;
    packet.acc_z = acc_z;
    packet.decl = decl;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_COMPENSATION_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_COMPENSATION;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_COMPENSATION_MIN_LEN, MAVLINK_MSG_ID_COMPENSATION_LEN, MAVLINK_MSG_ID_COMPENSATION_CRC);
}

/**
 * @brief Encode a compensation struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param compensation C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_compensation_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_compensation_t* compensation)
{
    return mavlink_msg_compensation_pack(system_id, component_id, msg, (uint64_t )(compensation->time_usec), (float )(compensation->mag_x), (float )(compensation->mag_y), (float )(compensation->mag_z), (float )(compensation->acc_x), (float )(compensation->acc_y), (float )(compensation->acc_z), (float )(compensation->decl));
}

/**
 * @brief Encode a compensation struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param compensation C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_compensation_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_compensation_t* compensation)
{
    return mavlink_msg_compensation_pack_chan(system_id, component_id, chan, msg, (uint64_t )(compensation->time_usec), (float )(compensation->mag_x), (float )(compensation->mag_y), (float )(compensation->mag_z), (float )(compensation->acc_x), (float )(compensation->acc_y), (float )(compensation->acc_z), (float )(compensation->decl));
}

/**
 * @brief Send a compensation message
 * @param chan MAVLink channel to send the message
 *
 * @param time_usec [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 * @param mag_x  mag X-axis
 * @param mag_y  mag Y-axis
 * @param mag_z  mag Z-axis
 * @param acc_x  acc X-axis
 * @param acc_y  acc Y-axis
 * @param acc_z  acc Z-axis
 * @param decl  decl
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_compensation_send(mavlink_channel_t chan, uint64_t time_usec, float mag_x, float mag_y, float mag_z, float acc_x, float acc_y, float acc_z, float decl)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_COMPENSATION_LEN];
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, mag_x);
    _mav_put_float(buf, 12, mag_y);
    _mav_put_float(buf, 16, mag_z);
    _mav_put_float(buf, 20, acc_x);
    _mav_put_float(buf, 24, acc_y);
    _mav_put_float(buf, 28, acc_z);
    _mav_put_float(buf, 32, decl);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMPENSATION, buf, MAVLINK_MSG_ID_COMPENSATION_MIN_LEN, MAVLINK_MSG_ID_COMPENSATION_LEN, MAVLINK_MSG_ID_COMPENSATION_CRC);
#else
    mavlink_compensation_t packet;
    packet.time_usec = time_usec;
    packet.mag_x = mag_x;
    packet.mag_y = mag_y;
    packet.mag_z = mag_z;
    packet.acc_x = acc_x;
    packet.acc_y = acc_y;
    packet.acc_z = acc_z;
    packet.decl = decl;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMPENSATION, (const char *)&packet, MAVLINK_MSG_ID_COMPENSATION_MIN_LEN, MAVLINK_MSG_ID_COMPENSATION_LEN, MAVLINK_MSG_ID_COMPENSATION_CRC);
#endif
}

/**
 * @brief Send a compensation message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_compensation_send_struct(mavlink_channel_t chan, const mavlink_compensation_t* compensation)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_compensation_send(chan, compensation->time_usec, compensation->mag_x, compensation->mag_y, compensation->mag_z, compensation->acc_x, compensation->acc_y, compensation->acc_z, compensation->decl);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMPENSATION, (const char *)compensation, MAVLINK_MSG_ID_COMPENSATION_MIN_LEN, MAVLINK_MSG_ID_COMPENSATION_LEN, MAVLINK_MSG_ID_COMPENSATION_CRC);
#endif
}

#if MAVLINK_MSG_ID_COMPENSATION_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_compensation_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_usec, float mag_x, float mag_y, float mag_z, float acc_x, float acc_y, float acc_z, float decl)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_usec);
    _mav_put_float(buf, 8, mag_x);
    _mav_put_float(buf, 12, mag_y);
    _mav_put_float(buf, 16, mag_z);
    _mav_put_float(buf, 20, acc_x);
    _mav_put_float(buf, 24, acc_y);
    _mav_put_float(buf, 28, acc_z);
    _mav_put_float(buf, 32, decl);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMPENSATION, buf, MAVLINK_MSG_ID_COMPENSATION_MIN_LEN, MAVLINK_MSG_ID_COMPENSATION_LEN, MAVLINK_MSG_ID_COMPENSATION_CRC);
#else
    mavlink_compensation_t *packet = (mavlink_compensation_t *)msgbuf;
    packet->time_usec = time_usec;
    packet->mag_x = mag_x;
    packet->mag_y = mag_y;
    packet->mag_z = mag_z;
    packet->acc_x = acc_x;
    packet->acc_y = acc_y;
    packet->acc_z = acc_z;
    packet->decl = decl;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COMPENSATION, (const char *)packet, MAVLINK_MSG_ID_COMPENSATION_MIN_LEN, MAVLINK_MSG_ID_COMPENSATION_LEN, MAVLINK_MSG_ID_COMPENSATION_CRC);
#endif
}
#endif

#endif

// MESSAGE COMPENSATION UNPACKING


/**
 * @brief Get field time_usec from compensation message
 *
 * @return [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number.
 */
static inline uint64_t mavlink_msg_compensation_get_time_usec(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field mag_x from compensation message
 *
 * @return  mag X-axis
 */
static inline float mavlink_msg_compensation_get_mag_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field mag_y from compensation message
 *
 * @return  mag Y-axis
 */
static inline float mavlink_msg_compensation_get_mag_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field mag_z from compensation message
 *
 * @return  mag Z-axis
 */
static inline float mavlink_msg_compensation_get_mag_z(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field acc_x from compensation message
 *
 * @return  acc X-axis
 */
static inline float mavlink_msg_compensation_get_acc_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field acc_y from compensation message
 *
 * @return  acc Y-axis
 */
static inline float mavlink_msg_compensation_get_acc_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Get field acc_z from compensation message
 *
 * @return  acc Z-axis
 */
static inline float mavlink_msg_compensation_get_acc_z(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  28);
}

/**
 * @brief Get field decl from compensation message
 *
 * @return  decl
 */
static inline float mavlink_msg_compensation_get_decl(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  32);
}

/**
 * @brief Decode a compensation message into a struct
 *
 * @param msg The message to decode
 * @param compensation C-struct to decode the message contents into
 */
static inline void mavlink_msg_compensation_decode(const mavlink_message_t* msg, mavlink_compensation_t* compensation)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    compensation->time_usec = mavlink_msg_compensation_get_time_usec(msg);
    compensation->mag_x = mavlink_msg_compensation_get_mag_x(msg);
    compensation->mag_y = mavlink_msg_compensation_get_mag_y(msg);
    compensation->mag_z = mavlink_msg_compensation_get_mag_z(msg);
    compensation->acc_x = mavlink_msg_compensation_get_acc_x(msg);
    compensation->acc_y = mavlink_msg_compensation_get_acc_y(msg);
    compensation->acc_z = mavlink_msg_compensation_get_acc_z(msg);
    compensation->decl = mavlink_msg_compensation_get_decl(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_COMPENSATION_LEN? msg->len : MAVLINK_MSG_ID_COMPENSATION_LEN;
        memset(compensation, 0, MAVLINK_MSG_ID_COMPENSATION_LEN);
    memcpy(compensation, _MAV_PAYLOAD(msg), len);
#endif
}
