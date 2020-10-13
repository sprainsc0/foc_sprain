#include "hrt_timer.h"
#include "platform.h"

static uint64_t base_time = 0;
static uint32_t last_count = 0;

uint64_t micros(void)
{
    __disable_irq();
    const uint32_t count = TIM5->CNT;

    if (count < last_count) {
        base_time += TIM5->ARR;
    }

    /* save the count for next time */
    last_count = count;

    const uint64_t abstime = (uint64_t)(base_time + count);
    __enable_irq();

    return abstime;
}

uint64_t millis(void)
{
    return micros()/1000;
}

// unsigned long getRunTimeCounterValue(void)
// {
//     return micros();
// }
