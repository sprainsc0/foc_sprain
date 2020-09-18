#pragma once

#include "ipc.h"

/**
 * The maximum string length supported.
 */
#define MAVLINK_LOG_MAXLEN			50

extern void mavlink_vasprintf(int severity, orb_advert_t *mavlink_log_pub, const char *fmt, ...);

#define _MSG_PRIO_DEBUG		7
#define _MSG_PRIO_INFO		6
#define _MSG_PRIO_NOTICE	5
#define _MSG_PRIO_WARNING	4
#define _MSG_PRIO_ERROR		3
#define _MSG_PRIO_CRITICAL	2
#define _MSG_PRIO_ALERT		1
#define _MSG_PRIO_EMERGENCY	0

/*
 * The va_args implementation here is not beautiful, but obviously we run into the same issues
 * the GCC devs saw, and are using their solution:
 *
 * http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */

/**
 * Send a mavlink info message (not printed to console).
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_info(_pub, _text, ...)	mavlink_vasprintf(_MSG_PRIO_INFO, _pub, _text, ##__VA_ARGS__);

/**
 * Send a mavlink emergency message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_emergency(_pub, _text, ...) \
	do { \
		mavlink_vasprintf(_MSG_PRIO_EMERGENCY, _pub, _text, ##__VA_ARGS__); \
	} while(0);

/**
 * Send a mavlink critical message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_critical(_pub, _text, ...) \
	do { \
		mavlink_vasprintf(_MSG_PRIO_CRITICAL, _pub, _text, ##__VA_ARGS__); \
	} while(0);

/**
 * Send a mavlink emergency message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_and_console_log_info(_pub, _text, ...)			\
	do { \
		mavlink_log_info(_pub, _text, ##__VA_ARGS__); \
	} while(0);

struct mavlink_logmessage {
	char text[MAVLINK_LOG_MAXLEN + 1];
	unsigned char severity;
};

struct mavlink_logbuffer {
	unsigned int start;
	unsigned int size;
	int count;
	struct mavlink_logmessage *elems;
};

