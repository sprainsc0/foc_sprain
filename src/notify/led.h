#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"

#include "topics/actuator_notify.h"

class LedNotify
{
public:
    LedNotify(void);
    
    void *_param;
    void run(void *parameter);

    bool init(void);

protected:
    osThreadId_t _handle;
private:
    
    int led_notify_sub;
    struct actuator_notify_s led_struct;
    
    uint32_t led_status;
    uint8_t  blink_count;
};

#endif
