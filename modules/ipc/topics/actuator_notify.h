#pragma once

#include "ipc.h"

#define LED_PATTERN_BGC_OFF             0x0000      /**< turn off led */
#define LED_PATTERN_BGC_ON              0xFFFF      /**< turn on led */
#define LED_PATTERN_BGC_DISARM    		0x0003		/**< slow blinking */
#define LED_PATTERN_BGC_ERROR        	0x5555		/**< fast blinking */
#define LED_PATTERN_BGC_ARMED 			0x0500		/**< long off, then quad blink */
#define LED_PATTERN_BGC_CAL 			0x0550
#define LED_PATTERN_BGC_INIT 			0x050F

struct actuator_notify_s {
	uint64_t timestamp; // required for logger
	uint32_t led_status;

#ifdef __cplusplus

#endif
};

/* register this as object request broker structure */
IPC_DECLARE(actuator_notify);

