#include "hrt_timer.h"
#include "platform.h"

uint64_t micros(void)
{
    uint64_t	abstime;
    uint32_t	count;

    static volatile uint64_t base_time = 0;
    static volatile uint32_t last_count = 0;

    count = TIM3->CNT;

    if (count < last_count) {
        base_time += 0xffff;
    }

    /* save the count for next time */
    last_count = count;

    abstime = (uint64_t)(base_time + count);

    return abstime;
}

uint64_t millis(void)
{
    return micros()/1000;
}

unsigned long getRunTimeCounterValue(void)
{
    return micros();
}
