#pragma once


#include "ipc.h"

#define CMD_PREFLIGHT_CALIBRATION        1
#define CMD_TEMPERATURE_CALIBRATION      2
#define CMD_TURN_OFF_MOTOR_OUTPUT        3
#define CMD_TURN_ON_MOTOR_OUTPUT         4
#define CMD_SETTING_MOTOR_MODE           5
#define CMD_MOTOR_AXIS_LOCK              6
#define CMD_MOTOR_AXIS_UNLOCK            7

struct foc_command_s {
	uint64_t timestamp;
    uint16_t command;
	float param1;
	float param2;
	float param3;
	float param4;
	float param5;
	bool from_external;
};

/* register this as object request broker structure */
IPC_DECLARE(foc_command);

