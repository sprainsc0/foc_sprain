#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"

#include "topics/actuator_notify.h"
#include "topics/parameter_update.h"
#include "topics/foc_command.h"

class Command
{
public:
    Command(void);

    void *_param;
    void run(void *parameter);

    bool init(void);
    
protected:
    osThreadId_t _handle;

private:
    
    
};

#endif
