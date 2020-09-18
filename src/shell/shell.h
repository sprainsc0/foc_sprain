#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"

#include "topics/actuator_notify.h"

class Shell
{
public:
    Shell(void);
    
    void *_param;
    void run(void *parameter);

    bool init(void);
    
protected:
    osThreadId_t _handle;

private:
    
    uint8_t cmd[80];
    uint8_t cmd_indes;
   
    char *name;
    char *args[10];
    uint8_t nargs;

    void decode_command(const char *arguments);
    void process_cmd();
};

#endif
