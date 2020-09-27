#pragma once


#include "ipc.h"


struct hfi_status_s {
	uint64_t timestamp;
	bool healthy;
};

/* register this as object request broker structure */
IPC_DECLARE(hfi_status);

