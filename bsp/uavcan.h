#ifndef __UAVCAN_H
#define __UAVCAN_H

#include "datatypes.h"
#include <uavcan/protocol/NodeStatus.h>
#include <uavcan/protocol/param/GetSet.h>
#include <uavcan/equipment/esc/Command.h>
#include <uavcan/equipment/esc/Encoder.h>
#include <uavcan/equipment/esc/Status.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GET_BROADCAST_ID(priority, data_type_id, local_id)                  (((uint32_t) priority << 24U) | ((uint32_t) data_type_id << 8U) | (uint32_t) local_id)
#define GET_REQU_RESP_ID(priority, data_type_id, kind, dest_id, local_id)   (((uint32_t) priority << 24U) | ((uint32_t) data_type_id << 16U) | ((uint32_t) kind << 15U) | ((uint32_t) dest_id << 8U) | (1U << 7U) | (uint32_t) local_id)

#define GIMBAL_ID        (1 << 3)   // b1000
#define ROLL_MOTOR_ID    (1u)       // b1
#define PITH_MOTOR_ID    (4u)       // b100
#define YAW_MOTOR_ID     (9u)       // b1001
#define AUX_MOTOR_ID     (1 << 4)   // b10000

#define LOCAL_MOTOR_ID   ROLL_MOTOR_ID

#define MOTOR_CHG_MODE   (1u)
#define MOTOR_CTL_MODE   (2u)

#define BGC_CTRL_ID      0x626763

typedef struct
{
    uint32_t id;
    uint32_t mask;
} CanardSTM32AcceptanceFilterConfiguration;

void uavcan_init(void);
int CAN_Transmit(const void* txframe);
int CAN_Receive(void* frame);
void foc_can_mode(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif
