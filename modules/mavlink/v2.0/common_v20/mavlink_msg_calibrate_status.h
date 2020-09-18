#pragma once
// MESSAGE CALIBRATE_STATUS PACKING

#define MAVLINK_MSG_ID_CALIBRATE_STATUS 53

MAVPACKED(
typedef struct __mavlink_calibrate_status_t {
 float param1; /*<  Parameter1.*/
 float param2; /*<  Parameter2.*/
 float param3; /*<  Parameter3.*/
 uint8_t type; /*<  The type of calibration.*/
 uint8_t status; /*<  The status of calibration.*/
}) mavlink_calibrate_status_t;

#define MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN 14
#define MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN 14
#define MAVLINK_MSG_ID_53_LEN 14
#define MAVLINK_MSG_ID_53_MIN_LEN 14

#define MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC 78
#define MAVLINK_MSG_ID_53_CRC 78



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_CALIBRATE_STATUS { \
    53, \
    "CALIBRATE_STATUS", \
    5, \
    {  { "type", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_calibrate_status_t, type) }, \
         { "status", NULL, MAVLINK_TYPE_UINT8_T, 0, 13, offsetof(mavlink_calibrate_status_t, status) }, \
         { "param1", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_calibrate_status_t, param1) }, \
         { "param2", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_calibrate_status_t, param2) }, \
         { "param3", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_calibrate_status_t, param3) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_CALIBRATE_STATUS { \
    "CALIBRATE_STATUS", \
    5, \
    {  { "type", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_calibrate_status_t, type) }, \
         { "status", NULL, MAVLINK_TYPE_UINT8_T, 0, 13, offsetof(mavlink_calibrate_status_t, status) }, \
         { "param1", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_calibrate_status_t, param1) }, \
         { "param2", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_calibrate_status_t, param2) }, \
         { "param3", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_calibrate_status_t, param3) }, \
         } \
}
#endif

/**
 * @brief Pack a calibrate_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param type  The type of calibration.
 * @param status  The status of calibration.
 * @param param1  Parameter1.
 * @param param2  Parameter2.
 * @param param3  Parameter3.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_calibrate_status_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t type, uint8_t status, float param1, float param2, float param3)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN];
    _mav_put_float(buf, 0, param1);
    _mav_put_float(buf, 4, param2);
    _mav_put_float(buf, 8, param3);
    _mav_put_uint8_t(buf, 12, type);
    _mav_put_uint8_t(buf, 13, status);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN);
#else
    mavlink_calibrate_status_t packet;
    packet.param1 = param1;
    packet.param2 = param2;
    packet.param3 = param3;
    packet.type = type;
    packet.status = status;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CALIBRATE_STATUS;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC);
}

/**
 * @brief Pack a calibrate_status message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param type  The type of calibration.
 * @param status  The status of calibration.
 * @param param1  Parameter1.
 * @param param2  Parameter2.
 * @param param3  Parameter3.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_calibrate_status_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t type,uint8_t status,float param1,float param2,float param3)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN];
    _mav_put_float(buf, 0, param1);
    _mav_put_float(buf, 4, param2);
    _mav_put_float(buf, 8, param3);
    _mav_put_uint8_t(buf, 12, type);
    _mav_put_uint8_t(buf, 13, status);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN);
#else
    mavlink_calibrate_status_t packet;
    packet.param1 = param1;
    packet.param2 = param2;
    packet.param3 = param3;
    packet.type = type;
    packet.status = status;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CALIBRATE_STATUS;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC);
}

/**
 * @brief Encode a calibrate_status struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param calibrate_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_calibrate_status_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_calibrate_status_t* calibrate_status)
{
    return mavlink_msg_calibrate_status_pack(system_id, component_id, msg, (uint8_t )(calibrate_status->type), (uint8_t )(calibrate_status->status), (float )(calibrate_status->param1), (float )(calibrate_status->param2), (float )(calibrate_status->param3));
}

/**
 * @brief Encode a calibrate_status struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param calibrate_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_calibrate_status_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_calibrate_status_t* calibrate_status)
{
    return mavlink_msg_calibrate_status_pack_chan(system_id, component_id, chan, msg, (uint8_t )(calibrate_status->type), (uint8_t )(calibrate_status->status), (float )(calibrate_status->param1), (float )(calibrate_status->param2), (float )(calibrate_status->param3));
}

/**
 * @brief Send a calibrate_status message
 * @param chan MAVLink channel to send the message
 *
 * @param type  The type of calibration.
 * @param status  The status of calibration.
 * @param param1  Parameter1.
 * @param param2  Parameter2.
 * @param param3  Parameter3.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_calibrate_status_send(mavlink_channel_t chan, uint8_t type, uint8_t status, float param1, float param2, float param3)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN];
    _mav_put_float(buf, 0, param1);
    _mav_put_float(buf, 4, param2);
    _mav_put_float(buf, 8, param3);
    _mav_put_uint8_t(buf, 12, type);
    _mav_put_uint8_t(buf, 13, status);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATE_STATUS, buf, MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC);
#else
    mavlink_calibrate_status_t packet;
    packet.param1 = param1;
    packet.param2 = param2;
    packet.param3 = param3;
    packet.type = type;
    packet.status = status;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATE_STATUS, (const char *)&packet, MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC);
#endif
}

/**
 * @brief Send a calibrate_status message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_calibrate_status_send_struct(mavlink_channel_t chan, const mavlink_calibrate_status_t* calibrate_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_calibrate_status_send(chan, calibrate_status->type, calibrate_status->status, calibrate_status->param1, calibrate_status->param2, calibrate_status->param3);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATE_STATUS, (const char *)calibrate_status, MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC);
#endif
}

#if MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_calibrate_status_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t type, uint8_t status, float param1, float param2, float param3)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_float(buf, 0, param1);
    _mav_put_float(buf, 4, param2);
    _mav_put_float(buf, 8, param3);
    _mav_put_uint8_t(buf, 12, type);
    _mav_put_uint8_t(buf, 13, status);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATE_STATUS, buf, MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC);
#else
    mavlink_calibrate_status_t *packet = (mavlink_calibrate_status_t *)msgbuf;
    packet->param1 = param1;
    packet->param2 = param2;
    packet->param3 = param3;
    packet->type = type;
    packet->status = status;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATE_STATUS, (const char *)packet, MAVLINK_MSG_ID_CALIBRATE_STATUS_MIN_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN, MAVLINK_MSG_ID_CALIBRATE_STATUS_CRC);
#endif
}
#endif

#endif

// MESSAGE CALIBRATE_STATUS UNPACKING


/**
 * @brief Get field type from calibrate_status message
 *
 * @return  The type of calibration.
 */
static inline uint8_t mavlink_msg_calibrate_status_get_type(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  12);
}

/**
 * @brief Get field status from calibrate_status message
 *
 * @return  The status of calibration.
 */
static inline uint8_t mavlink_msg_calibrate_status_get_status(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  13);
}

/**
 * @brief Get field param1 from calibrate_status message
 *
 * @return  Parameter1.
 */
static inline float mavlink_msg_calibrate_status_get_param1(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field param2 from calibrate_status message
 *
 * @return  Parameter2.
 */
static inline float mavlink_msg_calibrate_status_get_param2(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field param3 from calibrate_status message
 *
 * @return  Parameter3.
 */
static inline float mavlink_msg_calibrate_status_get_param3(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Decode a calibrate_status message into a struct
 *
 * @param msg The message to decode
 * @param calibrate_status C-struct to decode the message contents into
 */
static inline void mavlink_msg_calibrate_status_decode(const mavlink_message_t* msg, mavlink_calibrate_status_t* calibrate_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    calibrate_status->param1 = mavlink_msg_calibrate_status_get_param1(msg);
    calibrate_status->param2 = mavlink_msg_calibrate_status_get_param2(msg);
    calibrate_status->param3 = mavlink_msg_calibrate_status_get_param3(msg);
    calibrate_status->type = mavlink_msg_calibrate_status_get_type(msg);
    calibrate_status->status = mavlink_msg_calibrate_status_get_status(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN? msg->len : MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN;
        memset(calibrate_status, 0, MAVLINK_MSG_ID_CALIBRATE_STATUS_LEN);
    memcpy(calibrate_status, _MAV_PAYLOAD(msg), len);
#endif
}
