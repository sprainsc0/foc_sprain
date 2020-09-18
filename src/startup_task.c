#include "main.h"
#include "cmsis_os.h"
#include "platform.h"
#include "foc_function.h"

extern osThreadId_t startupHandle;

int module_func(module_fn_t module_function, const char *arguments)
{
    char *s = strdup(arguments);
    char *args[6];
    uint8_t nargs = 0;
    char *saveptr = NULL;
    
    for (char *tok=strtok_r(s, " ", &saveptr); tok; tok=strtok_r(NULL, " ", &saveptr)) {
        args[nargs++] = tok;
        if (nargs == 5) {
            break;
        }
    }
    args[nargs++] = NULL;
    
    return module_function(nargs, args);
}

void startup_task(void *argument)
{
	platform_init();
    
    module_func(ipc_main,         "start");
    
	if(!module_func(flashfs_main, "start")) {
        module_func(flashfs_main, "erase start");
    }
    if(!module_func(param_main,   "load")) {
        module_func(param_main,   "reset load");
    }
    
    module_func(notify_main,      "start");
    module_func(enc_main,         "start");
    module_func(foc_main,         "start cali");

    module_func(shell_main,       "start");
    module_func(Commander_main,   "start");
    
	osThreadTerminate(startupHandle);
}
