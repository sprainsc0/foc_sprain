#include "uPerf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dq_queue.h>
#include "hrt_timer.h"
#include "debug.h"
#include "cmsis_os.h"

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
	float			mean;
	float			M2;
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
	float			mean;
	float			M2;
};

/**
 * List of all known counters.
 */
static sq_queue_t	perf_counters = { NULL, NULL };

perf_counter_t
perf_alloc(enum perf_counter_type type, const char *name)
{
	perf_counter_t ctr = NULL;

	switch (type) {
	case PC_COUNT:
		ctr = (perf_counter_t)pvPortMalloc(sizeof(struct perf_ctr_count));
        memset(ctr, 0, sizeof(struct perf_ctr_count));
		break;

	case PC_ELAPSED:
		ctr = (perf_counter_t)pvPortMalloc(sizeof(struct perf_ctr_elapsed));
        memset(ctr, 0, sizeof(struct perf_ctr_elapsed));
		break;

	case PC_INTERVAL:
		ctr = (perf_counter_t)pvPortMalloc(sizeof(struct perf_ctr_interval));
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

	sq_rem(&handle->link, &perf_counters);
	vPortFree(handle);
}

void
perf_count(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}
    
    __disable_irq();
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
				pci->mean = pci->time_least / 1e6f;
				pci->M2 = 0;
				break;

			default: {
					const uint64_t interval = now - pci->time_last;

					if ((uint32_t)interval < pci->time_least) {
						pci->time_least = (uint32_t)interval;
					}

					if ((uint32_t)interval > pci->time_most) {
						pci->time_most = (uint32_t)interval;
					}

					// maintain mean and variance of interval in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					const float dt = interval / 1e6f;
					const float delta_intvl = dt - pci->mean;
					pci->mean += delta_intvl / pci->event_count;
					pci->M2 += delta_intvl * (dt - pci->mean);
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
    __enable_irq();
}

void
perf_begin(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}
    __disable_irq();
	switch (handle->type) {
	case PC_ELAPSED:
		((struct perf_ctr_elapsed *)handle)->time_start = micros();
		break;

	default:
		break;
	}
    __enable_irq();
}

void
perf_end(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}
    __disable_irq();
	switch (handle->type) {
	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;

			if (pce->time_start != 0) {
				int64_t elapsed = micros() - pce->time_start;

				if (elapsed >= 0) {

					pce->event_count++;
					pce->time_total += elapsed;

					if ((pce->time_least > (uint32_t)elapsed) || (pce->time_least == 0)) {
						pce->time_least = elapsed;
					}

					if (pce->time_most < (uint32_t)elapsed) {
						pce->time_most = elapsed;
					}

					// maintain mean and variance of the elapsed time in seconds
					// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
					float dt = elapsed / 1e6f;
					float delta_intvl = dt - pce->mean;
					pce->mean += delta_intvl / pce->event_count;
					pce->M2 += delta_intvl * (dt - pce->mean);

					pce->time_start = 0;
				}
			}
		}
		break;

	default:
		break;
	}
    __enable_irq();
}

void
perf_set_elapsed(perf_counter_t handle, int64_t elapsed)
{
	if (handle == NULL) {
		return;
	}

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

				// maintain mean and variance of the elapsed time in seconds
				// Knuth/Welford recursive mean and variance of update intervals (via Wikipedia)
				float dt = elapsed / 1e6f;
				float delta_intvl = dt - pce->mean;
				pce->mean += delta_intvl / pce->event_count;
				pce->M2 += delta_intvl * (dt - pce->mean);

				pce->time_start = 0;
			}
		}
		break;

	default:
		break;
	}
}

void
perf_set_count(perf_counter_t handle, uint64_t count)
{
	if (handle == NULL) {
		return;
	}

	switch (handle->type) {
	case PC_COUNT: {
			((struct perf_ctr_count *)handle)->event_count = count;
		}
		break;

	default:
		break;
	}

}

void
perf_cancel(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}

	switch (handle->type) {
	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;

			pce->time_start = 0;
		}
		break;

	default:
		break;
	}
}



void
perf_reset(perf_counter_t handle)
{
	if (handle == NULL) {
		return;
	}

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
}

void
perf_print_counter(perf_counter_t handle)
{
    char print[160];
    
	if (handle == NULL) {
		return;
	}

	switch (handle->type) {
	case PC_COUNT:
        sprintf(print, "%10llu events|",
			(unsigned long long)((struct perf_ctr_count *)handle)->event_count);
		Info_Debug("%16s:\t %s\n", handle->name, print);
		break;

	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;
			float rms = sqrtf(pce->M2 / (pce->event_count - 1));
            
			sprintf(print, "%10llu events| %10lluus elapsed| %10lluus avg| min %10lluus| max %10lluus| %5.3fus rms",
				(unsigned long long)pce->event_count,
				(unsigned long long)pce->time_total,
				(pce->event_count == 0) ? 0 : (unsigned long long)pce->time_total / pce->event_count,
				(unsigned long long)pce->time_least,
				(unsigned long long)pce->time_most,
				(double)(1e6f * rms));
            Info_Debug("%16s:\t %s\n", handle->name, print);
			break;
		}

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;
			float rms = sqrtf(pci->M2 / (pci->event_count - 1));

			sprintf(print, "%10llu events| %10lluus avg| min %10lluus| max %10lluus| %5.3fus rms",
				(unsigned long long)pci->event_count,
				(pci->event_count == 0) ? 0 : (unsigned long long)(pci->time_last - pci->time_first) / pci->event_count,
				(unsigned long long)pci->time_least,
				(unsigned long long)pci->time_most,
				(double)(1e6f * rms));
            Info_Debug("%16s:\t %s\n", handle->name, print);
			break;
		}

	default:
		break;
	}
}


int
perf_print_counter_buffer(char *buffer, int length, perf_counter_t handle)
{
	int num_written = 0;

	if (handle == NULL) {
		return 0;
	}

	switch (handle->type) {
	case PC_COUNT:
		num_written = snprintf(buffer, length, "%s: %llu events",
				       handle->name,
				       (unsigned long long)((struct perf_ctr_count *)handle)->event_count);
		break;

	case PC_ELAPSED: {
			struct perf_ctr_elapsed *pce = (struct perf_ctr_elapsed *)handle;
			float rms = sqrtf(pce->M2 / (pce->event_count - 1));
			num_written = snprintf(buffer, length, "%s: %llu events, %lluus elapsed, %lluus avg, min %lluus max %lluus %5.3fus rms",
					       handle->name,
					       (unsigned long long)pce->event_count,
					       (unsigned long long)pce->time_total,
					       (pce->event_count == 0) ? 0 : (unsigned long long)pce->time_total / pce->event_count,
					       (unsigned long long)pce->time_least,
					       (unsigned long long)pce->time_most,
					       (double)(1e6f * rms));
			break;
		}

	case PC_INTERVAL: {
			struct perf_ctr_interval *pci = (struct perf_ctr_interval *)handle;
			float rms = sqrtf(pci->M2 / (pci->event_count - 1));

			num_written = snprintf(buffer, length, "%s: %llu events, %lluus avg, min %lluus max %lluus %5.3fus rms",
					       handle->name,
					       (unsigned long long)pci->event_count,
					       (pci->event_count == 0) ? 0 : (unsigned long long)(pci->time_last - pci->time_first) / pci->event_count,
					       (unsigned long long)pci->time_least,
					       (unsigned long long)pci->time_most,
					       (double)(1e6f * rms));
			break;
		}

	default:
		break;
	}

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
