#pragma once


#include "ipc.h"

typedef enum {
	HFI_SAMPLES_8 = 0,
	HFI_SAMPLES_16,
	HFI_SAMPLES_32
} foc_hfi_samples;

struct hfi_status_s {
	uint64_t timestamp;

	void(*fft_bin0_func)(float*, float*, float*);
	void(*fft_bin1_func)(float*, float*, float*);
	void(*fft_bin2_func)(float*, float*, float*);

	int samples;
	int table_fact;
	float buffer[32];
	float buffer_current[32];
	bool ready;
	int ind;
	bool is_samp_n;
	float prev_sample;
	float angle;
	int est_done_cnt;
	float observer_zero_time;
	int flip_cnt;
};

/* register this as object request broker structure */
IPC_DECLARE(hfi_status);

