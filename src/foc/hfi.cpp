#include "hfi.h"
#include <string>
#include "hrt_timer.h"
#include "debug.h"

const osThreadAttr_t hfi_attributes = {
		.name = "hfi",
		.priority = (osPriority_t)osPriorityRealtime1,
		.stack_size = 512};

static void hfi_func(HFI *pThis)
{
    pThis->run(pThis->_param);
}

HFI::HFI(void):
    _param(NULL)
{
    
}

bool HFI::init(void)
{    
    _param_handles.motor_r_handle          = param_find("MOTOR_R");
	_param_handles.motor_l_handle          = param_find("MOTOR_L");
    _param_handles.motor_l_diff_handle     = param_find("MOTOR_L_DIFF");
	_param_handles.flux_linkage_handle     = param_find("FLUX_LINKAGE");

    _params_sub     = ipc_subscibe(IPC_ID(parameter_update));

    _handle = osThreadNew((osThreadFunc_t)hfi_func, this, &hfi_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void HFI::parameter_update(bool force)
{
    bool updated = force;
    if (!updated) {
		ipc_check(_params_sub, &updated);
	}
    if (updated) {
		parameter_update_s param_update;
		ipc_pull(IPC_ID(parameter_update), _params_sub, &param_update);

		param_get(_param_handles.motor_r_handle,          &_hfi_cfg.motor_r);
		param_get(_param_handles.motor_l_handle,          &_hfi_cfg.motor_l);
        param_get(_param_handles.motor_l_diff_handle,     &_hfi_cfg.motor_l_diff);
		param_get(_param_handles.flux_linkage_handle,     &_hfi_cfg.flux_linkage);
    }
}

void HFI::run(void *parameter)
{
    parameter_update(true);
    
    while (1)
    {
        parameter_update(false);
        // 100Hz loop
        osDelay(10);
    }
}

