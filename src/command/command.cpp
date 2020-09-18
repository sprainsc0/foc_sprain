#include "command.h"
#include <string>
#include "hrt_timer.h"
#include "foc_function.h"
#include "debug.h"

const osThreadAttr_t cmd_attributes = {
		.name = "cmd",
		.priority = (osPriority_t)osPriorityAboveNormal,
		.stack_size = 1024};

namespace CMD
{
static Command	*gCmd;
}

static void cmd_func(Command *pThis)
{
    pThis->run(pThis->_param);
}

Command::Command(void):
    _param(NULL)
{

}

bool Command::init(void)
{
    _handle = osThreadNew((osThreadFunc_t)cmd_func, this, &cmd_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void Command::run(void *parameter)
{
    while (1)
    {
        
        // 10Hz loop
        osDelay(100);
    }
}

int Commander_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {

            if (CMD::gCmd != nullptr) {
                Info_Debug("already running\n");
                return 0;
            }

            CMD::gCmd = new Command();

            if (CMD::gCmd == NULL) {
                Info_Debug("alloc failed\n");
                return 0;
            }
            CMD::gCmd->init();
        }
    }
    return 1;
}
