#include "platform.h"
#include "foc.h"
#include "mc.h"

void mc_task(void *argument)
{
	while (1)
	{
		// position ctrl
		if ((mc.ctrl_loop & (MC_CTRL_POSITION | MC_CTRL_SPEED)) && 
			(mc.ctrl_loop & MC_CTRL_ENABLE))
			mc_ctrl_process();

		osDelay(1);
	}
}
