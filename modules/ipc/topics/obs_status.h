#pragma once


#include "ipc.h"

typedef enum
{
	FOC_OBSERVER_ORTEGA_ORIGINAL = 0,
	FOC_OBSERVER_ORTEGA_ITERATIVE = 1
} foc_observer_type_t;



struct obs_status_s {
	uint64_t timestamp;
	uint8_t obs_mode;
	float m_gamma_now;

	float m_observer_x1;
	float m_observer_x2;

	float m_x1_prev;
	float m_x2_prev;
};

/* register this as object request broker structure */
IPC_DECLARE(obs_status);

