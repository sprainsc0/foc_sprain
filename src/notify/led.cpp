#include "led.h"
#include "gpio.h"
#include <string>
#include "hrt_timer.h"
#include "foc_function.h"
#include "debug.h"

const osThreadAttr_t led_attributes = {
		.name = "led",
		.priority = (osPriority_t)osPriorityLow,
		.stack_size = 512};

namespace Notify
{
static LedNotify	*gNotify;
}

static void led_func(LedNotify *pThis)
{
    pThis->run(pThis->_param);
}

LedNotify::LedNotify(void):
    led_status(LED_PATTERN_BGC_ERROR),
    blink_count(0),
    _param(NULL)
{
    
}

bool LedNotify::init(void)
{
    led_notify_sub = ipc_subscibe(IPC_ID(actuator_notify));
    
    _handle = osThreadNew((osThreadFunc_t)led_func, this, &led_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void LedNotify::run(void *parameter)
{
    while (1)
    {
        bool led_stat = (led_status & (1 << blink_count));
        bool updated = false;
        
        ipc_check(led_notify_sub, &updated);
        if(updated) {
            ipc_pull(IPC_ID(actuator_notify), led_notify_sub, &led_struct);
            led_status = led_struct.led_status;
        }
        
        blink_count++;

        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, led_stat ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, led_stat ? GPIO_PIN_SET : GPIO_PIN_RESET);
        
        if (blink_count > 15) {
            blink_count = 0;
        }
        // 10Hz loop
        osDelay(100);
    }
}


int notify_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {

            if (Notify::gNotify != nullptr) {
                Info_Debug("already running\n");
                return 0;
            }

            Notify::gNotify = new LedNotify();
            

            if (Notify::gNotify == NULL) {
                Info_Debug("alloc failed\n");
                return 0;
            }
            Notify::gNotify->init();
        }
    }
    return 1;
}
