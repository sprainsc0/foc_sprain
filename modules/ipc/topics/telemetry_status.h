#pragma once


#include "IPC.h"


#ifndef __cplusplus

#endif


struct telemetry_status_s {
    uint64_t timestamp;
    uint64_t heartbeat_time;			// Time of last received heartbeat from remote system
    uint64_t telem_time;			// Time of last received telemetry status packet, 0 for none
    uint8_t type;				// type of the radio hardware
    uint8_t rssi;				// local signal strength
    uint8_t remote_rssi;			// remote signal strength
    uint16_t rxerrors;				// receive errors
    uint16_t fixed;				// count of error corrected packets
    uint8_t noise;				// background noise level
    uint8_t remote_noise;			// remote background noise level
    uint8_t txbuf;				// how full the tx buffer is as a percentage
    uint8_t system_id;				// system id of the remote system
    uint8_t component_id;			// component id of the remote system
#ifdef __cplusplus

#endif
};

/* register this as object request broker structure */
IPC_DECLARE(telemetry_status);

