#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "hrt_timer.h"
#include "mavlink_logs.h"

#define MAVLINK_LOG_QUEUE_SIZE 5

void mavlink_vasprintf(int severity, orb_advert_t *mavlink_log_pub, const char *fmt, ...)
{
	// TODO: add compile check for maxlen

//	if (!fmt) {
//		return;
//	}
//
//	if (mavlink_log_pub == NULL) {
//		return;
//	}
//
//	struct mavlink_log_s log_msg;
//
//	log_msg.severity = severity;
//
//	log_msg.timestamp = hrt_absolute_time();
//
//	va_list ap;
//
//	va_start(ap, fmt);
//
//	vsnprintf((char *)log_msg.text, sizeof(log_msg.text), fmt, ap);
//
//	va_end(ap);
//    
//    if(*mavlink_log_pub == NULL) {
//		*mavlink_log_pub = ipc_active(IPC_ID(mavlink_log), &log_msg);
//	}
//
//	if(*mavlink_log_pub != NULL) {
//		ipc_push(IPC_ID(mavlink_log), *mavlink_log_pub, &log_msg);
//
//	}
}

