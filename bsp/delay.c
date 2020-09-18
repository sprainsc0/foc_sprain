#include "platform.h"
#include "delay.h"
#include "hrt_timer.h"

void hal_delay_us(uint32_t us)
{
    uint64_t told;
    told = micros()+us;
    while(1)
    {
        if(micros() >= told) break;
    };
}

