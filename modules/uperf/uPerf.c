#include "uPerf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dq_queue.h>
#include "hrt_timer.h"
#include "debug.h"
#include "cmsis_os.h"
#include "task.h"

/**
 * Header common to all counters.
 */
struct perf_ctr_header {
	sq_entry_t		link;	/**< list linkage */
	enum perf_counter_type	type;	/**< counter type */
	const char		*name;	/**< counter name */
};

/**
 * PC_EVENT counter.
 */
struct perf_ctr_count {
	struct perf_ctr_header	hdr;
	uint64_t		event_count;
};

/**
 * PC_ELAPSED counter.
 */
struct perf_ctr_elapsed {
	struct perf_ctr_header	hdr;
	uint64_t		event_count;
	uint64_t		time_start;
	uint64_t		time_total;
	uint32_t		time_least;
	uint32_t		time_most;
};

/**
 * PC_INTERVAL counter.
 */
struct perf_ctr_interval {
	struct perf_ctr_header	hdr;
	uint64_t		event_count;
	uint64_t		time_event;
	uint64_t		time_first;
	uint64_t		time_last;
	uint32_t		time_least;
	uint32_t		time_most;
};

/**
 * List of all known counters.
 */
static sq_queue_t	perf_counters = { NULL, NULL };

perf_counter_t
perf_alloc(enum perf_counter_type type, const char *name)
{
	perf_counter_t ctr = NULL;
	taskENTER_CRITICAL();
	switch (type) {
	case PC_COUNT:
		ctr = (perf_counter_t)malloc(sizeof(struct perf_ctr_count));
        memset(ctr, 0, sizeof(struct perf_ctr_count));
		break;

	case PC_ELAPSED:
		ctr = (perf_counter_t)malloc(sizeof(struct perf_ctr_elapsed));
        memset(ctr, 0, sizeof(struct perf_ctr_elapsed));
		break;

	case PC_INTERVAL:
		ctr = (perf_counter_t)malloc(sizeof(struct perf_ctr_interval));
        memset(ctr, 0, sizeof(struct perf_ctr_interval));
		break;

	default:
		break;
	}

	if (ctr != NULL) {
		ctr->type = type;
		ctr->name = name;
		sq_addfirst(&ctr->link, &perf_counters);
	}
	taskEXIT_CRITICAL();
	return ctr;
}

perf_counter_t
perf_alloc_once(enum perf_counter_type type, const char *name)
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	while (handle != NULL) {
		if (!strcmp(handle->name, name)) {
			if (type == handle->type) {
				/* they are the same counter */
				return handle;

			} else {
				/* same name but different type, assuming this is an error and not intended */
				return NULL;
			}
		}
		handle = (perf_counter_t)sq_next(&handle->link);
	}

	/* if the execution reaches here, no existing counter of that name was found */
	return perf_alloc(type, name);
}

void
perf_free(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}

	taskENTER_CRITICAL();
	sq_rem(&handle->link, &perf_counters);
	vPortFree(handle);
	taskEXIT_CRITICAL();
}

float
perf_count(perf_counter_t handle)
{
	if (handle == NULL) {
		return 0.0f;
	}

    taskENTER_CRITICAL();
	volatile float dt = 0.0f;

	switch (handle->type) {
	case PC_COUNT:
		((struct perf_ctr_count *)handle)->event_count++;
		break;

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;
			const uint64_t now = micros();

			switch (pci->event_count) {
			case 0:
				pci->time_first = now;
				break;

			case 1:
				pci->time_least = (uint32_t)(now - pci->time_last);
				pci->time_most = (uint32_t)(now - pci->time_last);
				break;

			default: {
					const uint64_t interval = now - pci->time_last;

					if ((uint32_t)interval < pci->time_least) {
						pci->time_least = (uint32_t)interval;
					}

					if ((uint32_t)interval > pci->time_most) {
						pci->time_most = (uint32_t)interval;
					}

					dt = (float)(interval / 1e6f);
					break;
				}
			}

			pci->time_last = now;
			pci->event_count++;
			break;
		}

	default:
		break;
	}
	taskEXIT_CRITICAL();
	return dt;
}

float
perf_count_isr(perf_counter_t handle)
{
	if (handle == NULL) {
		return 0.0f;
	}
    int ret = taskENTER_CRITICAL_FROM_ISR();
	volatile float dt = 0.0f;

	switch (handle->type) {
	case PC_COUNT:
		((struct perf_ctr_count *)handle)->event_count++;
		break;

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;
			const uint64_t now = micros();

			switch (pci->event_count) {
			case 0:
				pci->time_first = now;
				break;

			case 1:
				pci->time_least = (uint32_t)(now - pci->time_last);
				pci->time_most = (uint32_t)(now - pci->time_last);
				break;

			default: {
					const uint64_t interval = now - pci->time_last;

					if ((uint32_t)interval < pci->time_least) {
						pci->time_least = (uint32_t)interval;
					}

					if ((uint32_t)interval > pci->time_most) {
						pci->time_most = (uint32_t)interval;
					}

					dt = (float)(interval / 1e6f);
					break;
				}
			}

			pci->time_last = now;
			pci->event_count++;
			break;
		}

	default:
		break;
	}
	taskEXIT_CRITICAL_FROM_ISR(ret);
	return dt;
}

void
perf_begin(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}
	taskENTER_CRITICAL();
	switch (handle->type) {
	case PC_ELAPSED:
		((struct perf_ctr_elapsed *)handle)->time_start = micros();
		break;

	default:
		break;
	}
	taskEXIT_CRITICAL();
}

void
perf_begin_isr(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}
	int ret = taskENTER_CRITICAL_FROM_ISR();
	switch (handle->type) {
	case PC_ELAPSED:
		((struct perf_ctr_elapsed *)handle)->time_start = micros();
		break;

	default:
		break;
	}
	taskEXIT_CRITICAL_FROM_ISR(ret);
}

void
perf_end(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}
	taskENTER_CRITICAL();
	switch (handle->type) {
	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;

			if (pce->time_start != 0) {
				volatile int64_t elapsed = micros() - pce->time_start;

				if (elapsed >= 0) {

					pce->event_count++;
					pce->time_total += elapsed;

					if ((pce->time_least > (uint32_t)elapsed) || (pce->time_least == 0)) {
						pce->time_least = elapsed;
					}

					if (pce->time_most < (uint32_t)elapsed) {
						pce->time_most = elapsed;
					}

					pce->time_start = 0;
				}
			}
		}
		break;

	default:
		break;
	}
	taskEXIT_CRITICAL();
}

void
perf_end_isr(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}
	int ret = taskENTER_CRITICAL_FROM_ISR();
	switch (handle->type) {
	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;

			if (pce->time_start != 0) {
				volatile int64_t elapsed = micros() - pce->time_start;

				if (elapsed >= 0) {

					pce->event_count++;
					pce->time_total += elapsed;

					if ((pce->time_least > (uint32_t)elapsed) || (pce->time_least == 0)) {
						pce->time_least = elapsed;
					}

					if (pce->time_most < (uint32_t)elapsed) {
						pce->time_most = elapsed;
					}

					pce->time_start = 0;
				}
			}
		}
		break;

	default:
		break;
	}
	taskEXIT_CRITICAL_FROM_ISR(ret);
}

void
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	if (handle == NULL) {
		return;
	}

	int ret = taskENTER_CRITICAL_FROM_ISR();
	switch (handle->type) {
	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;

			if (elapsed >= 0) {

				pce->event_count++;
				pce->time_total += elapsed;

				if ((pce->time_least > (uint32_t)elapsed) || (pce->time_least == 0)) {
					pce->time_least = elapsed;
				}

				if (pce->time_most < (uint32_t)elapsed) {
					pce->time_most = elapsed;
				}

				pce->time_start = 0;
			}
		}
		break;

	default:
		break;
	}
	taskEXIT_CRITICAL_FROM_ISR(ret);
}

void
perf_set_count(perf_counter_t handle, uint64_t count)
{
	if (handle == NULL) {
		return;
	}

	taskENTER_CRITICAL();
	switch (handle->type) {
	case PC_COUNT: {
			((struct perf_ctr_count *)handle)->event_count = count;
		}
		break;

	default:
		break;
	}
	taskEXIT_CRITICAL();
}

void
perf_cancel(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}

	taskENTER_CRITICAL();
	switch (handle->type) {
	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;

			pce->time_start = 0;
		}
		break;

	default:
		break;
	}
	taskEXIT_CRITICAL();
}



void
perf_reset(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}

	int ret = taskENTER_CRITICAL_FROM_ISR();
	switch (handle->type) {
	case PC_COUNT:
		((struct perf_ctr_count *)handle)->event_count = 0;
		break;

	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;
			pce->event_count = 0;
			pce->time_start = 0;
			pce->time_total = 0;
			pce->time_least = 0;
			pce->time_most = 0;
			break;
		}

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;
			pci->event_count = 0;
			pci->time_event = 0;
			pci->time_first = 0;
			pci->time_last = 0;
			pci->time_least = 0;
			pci->time_most = 0;
			break;
		}
	}
	taskEXIT_CRITICAL_FROM_ISR(ret);
}

void
perf_print_counter(perf_counter_t handle)
{
    char print[160];
    
	if (handle == NULL) {
		return;
	}

	int ret = taskENTER_CRITICAL_FROM_ISR();
	switch (handle->type) {
	case PC_COUNT:
        sprintf(print, "%10lu events|",
			(unsigned long)((struct perf_ctr_count *)handle)->event_count);
		Info_Debug("%16s:\t %s\n", handle->name, print);
		break;

	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;
            
			sprintf(print, "%10lu events| %10lu avg| min %10lu| max %10lu",
				(unsigned long)pce->event_count,
				(pce->event_count == 0) ? 0 : (unsigned long)pce->time_total / (unsigned long)pce->event_count,
				(unsigned long)pce->time_least,
				(unsigned long)pce->time_most);
            Info_Debug("%16s:\t %s\n", handle->name, print);
			break;
		}

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;

			sprintf(print, "%10lu events| %10lu avg| min %10lu| max %10lu",
				(unsigned long)pci->event_count,
				(pci->event_count == 0) ? 0 : (unsigned long)(pci->time_last - pci->time_first) / (unsigned long)pci->event_count,
				(unsigned long)pci->time_least,
				(unsigned long)pci->time_most);
            Info_Debug("%16s:\t %s\n", handle->name, print);
			break;
		}

	default:
		break;
	}
	taskEXIT_CRITICAL_FROM_ISR(ret);
}


int
perf_print_counter_buffer(char *buffer, int length, perf_counter_t handle)
{
	int num_written = 0;

	if (handle == NULL) {
		return 0;
	}

	int ret = taskENTER_CRITICAL_FROM_ISR();
	switch (handle->type) {
	case PC_COUNT:
		num_written = snprintf(buffer, length, "%s: %llu events",
				       handle->name,
				       (unsigned long long)((struct perf_ctr_count *)handle)->event_count);
		break;

	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;
			num_written = snprintf(buffer, length, "%s: %llu events, %lluus avg, min %lluus max %lluus",
					       handle->name,
					       (unsigned long long)pce->event_count,
					       (pce->event_count == 0) ? 0 : (unsigned long long)pce->time_total / pce->event_count,
					       (unsigned long long)pce->time_least,
					       (unsigned long long)pce->time_most);
			break;
		}

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;

			num_written = snprintf(buffer, length, "%s: %llu events, %lluus avg, min %lluus max %lluus",
					       handle->name,
					       (unsigned long long)pci->event_count,
					       (pci->event_count == 0) ? 0 : (unsigned long long)(pci->time_last - pci->time_first) / pci->event_count,
					       (unsigned long long)pci->time_least,
					       (unsigned long long)pci->time_most);
			break;
		}

	default:
		break;
	}
	taskEXIT_CRITICAL_FROM_ISR(ret);

	buffer[length - 1] = 0; // ensure 0-termination
	return num_written;
}

uint64_t
perf_event_count(perf_counter_t handle)
{
	if (handle == NULL) {
		return 0;
	}

	switch (handle->type) {
	case PC_COUNT:
		return ((struct perf_ctr_count *)handle)->event_count;

	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;
			return pce->event_count;
		}

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;
			return pci->event_count;
		}

	default:
		break;
	}

	return 0;
}

void
perf_print_all(uint8_t type)
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);
    
	while (handle != NULL) {
		if(handle->type == type) {
			perf_print_counter(handle);
			osDelay(10);
		}
		handle = (perf_counter_t)sq_next(&handle->link);
	}
}

void
perf_reset_all(void)
{
	perf_counter_t handle = (perf_counter_t)sq_peek(&perf_counters);

	while (handle != NULL) {
		perf_reset(handle);
		handle = (perf_counter_t)sq_next(&handle->link);
	}
}
