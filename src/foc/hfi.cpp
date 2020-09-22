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
    _handle = osThreadNew((osThreadFunc_t)hfi_func, this, &hfi_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void HFI::run(void *parameter)
{
    while (1)
    {

        // 10Hz loop
        osDelay(1);
    }
}

