#include "shell.h"
#include <string>
#include "task.h"
#include "hrt_timer.h"
#include "foc_function.h"
#include "debug.h"

const osThreadAttr_t shell_attributes = {
		.name = "shell",
		.priority = (osPriority_t)osPriorityNormal7,
		.stack_size = 2048};

namespace DEBUG
{
static Shell	*gShell;
}

int perf_main(int argc, char *argv[]);
int motor_main(int argc, char *argv[]);

static void shell_func(Shell *pThis)
{
    pThis->run(pThis->_param);
}

Shell::Shell(void):
    cmd_indes(0),
    _param(NULL)
{
    memset(cmd, 0, sizeof(cmd));
}

bool Shell::init(void)
{
    _handle = osThreadNew((osThreadFunc_t)shell_func, this, &shell_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void Shell::decode_command(const char *arguments)
{
    char *s = strdup(arguments);
    char *saveptr = NULL;
    bool first = true;
    nargs = 0;
    
    for (char *tok=strtok_r(s, " ", &saveptr); tok; tok=strtok_r(NULL, " ", &saveptr)) {
        if(first) {
            name = tok;
            first = false;
        } else {
            args[nargs++] = tok;
            if (nargs == 9) {
                break;
            }
        }
    }
}

void Shell::process_cmd()
{
    if(!strcmp(name, "perf")) {
         perf_main(nargs, args);
    } else if(!strcmp(name, "param")) {
         param_main(nargs, args);
    } else if(!strcmp(name, "flash")) {
         flashfs_main(nargs, args);
    } else if(!strcmp(name, "motor")) {
         motor_main(nargs, args);
    } else if(!strcmp(name, "cmd")) {
         Commander_main(nargs, args);
    } else if(!strcmp(name, "reboot")) {
         HAL_NVIC_SystemReset();
    }
}

void Shell::run(void *parameter)
{
    while (1)
    {
        if(SEGGER_RTT_HasKey()) {
            cmd[cmd_indes] = SEGGER_RTT_GetKey();
            SEGGER_RTT_Write(0, (const char*)&cmd[cmd_indes], 1);

            if(cmd[cmd_indes] == '\n') {
                cmd[cmd_indes] = '\0';
                decode_command((const char*)cmd);
                process_cmd();
                cmd_indes = 0;
                memset(cmd, 0, sizeof(cmd));
            } else {
                cmd_indes++;
            }
        }
        // 20Hz loop
        osDelay(50);
    }
}

int shell_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {

            if (DEBUG::gShell != nullptr) {
                Info_Debug("already running\n");
                return 0;
            }

            DEBUG::gShell = new Shell();

            if (DEBUG::gShell == NULL) {
                Info_Debug("alloc failed\n");
                return 0;
            }
            DEBUG::gShell->init();
        }
    }
    return 1;
}

int perf_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "reset")) {
            perf_reset_all();
            Info_Debug("perf reset done\n");
        }

        if (!strcmp(argv[i], "int")) {
            Info_Debug(" INTERVAL:\n");
            perf_print_all(2);
        }
        if (!strcmp(argv[i], "ela")) {
            Info_Debug(" ELAPSED:\n");
            perf_print_all(1);
        }
        if (!strcmp(argv[i], "cnt")) {
            Info_Debug(" COUNT:\n");
            perf_print_all(0);
        }

        if (!strcmp(argv[i], "task")) {
            char pcWriteBuffer[500];
          
            Info_Debug("Name\t\tStatus\tPriority\tStack\tSeq_num\n");
            vTaskList((char *)pcWriteBuffer);
            Info_Debug("%s\n", pcWriteBuffer);
        }

        if (!strcmp(argv[i], "usage")) {
            char pcWriteBuffer[500];
          
            Info_Debug("Name\t\tRunTimes\t\tUseRates\n");
            vTaskGetRunTimeStats((char *)pcWriteBuffer);
            Info_Debug("%s\n", pcWriteBuffer);
            
            Info_Debug("Heap:%d,free size:%d\n",configTOTAL_HEAP_SIZE, xPortGetFreeHeapSize());
        }
    }
    return 1;
}

orb_advert_t _foc_ref_pub = nullptr;
int motor_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "runv")) {
            if (argc < 2) {
                Info_Debug("input argv error\n");
                return 0;
            }
            float param = (float)atof(argv[++i]);
            struct foc_target_s foc_ref;
            if(_foc_ref_pub == nullptr) {
                _foc_ref_pub = ipc_active(IPC_ID(foc_target), &foc_ref);
            }
            foc_ref.ctrl_mode = MC_CTRL_ENABLE;
            foc_ref.id_target = 0;
            foc_ref.iq_target = 0;
            foc_ref.vd_target = 0;
            foc_ref.vq_target = param;
            ipc_push(IPC_ID(foc_target), _foc_ref_pub, &foc_ref);
        }

        if (!strcmp(argv[i], "runi")) {
            if (argc < 2) {
                Info_Debug("input argv error\n");
                return 0;
            }
            float param = (float)atof(argv[++i]);
            struct foc_target_s foc_ref;
            if(_foc_ref_pub == nullptr) {
                _foc_ref_pub = ipc_active(IPC_ID(foc_target), &foc_ref);
            }
            foc_ref.ctrl_mode = (MC_CTRL_CURRENT | MC_CTRL_ENABLE);
            foc_ref.id_target = 0;
            foc_ref.iq_target = param;
            foc_ref.vd_target = 0;
            foc_ref.vq_target = 0;
            ipc_push(IPC_ID(foc_target), _foc_ref_pub, &foc_ref);
        }

        if (!strcmp(argv[i], "idle")) {
            struct foc_target_s foc_ref;
            if(_foc_ref_pub == nullptr) {
                _foc_ref_pub = ipc_active(IPC_ID(foc_target), &foc_ref);
            }
            foc_ref.ctrl_mode = 0;
            foc_ref.id_target = 0;
            foc_ref.iq_target = 0;
            foc_ref.vd_target = 0;
            foc_ref.vq_target = 0;
            ipc_push(IPC_ID(foc_target), _foc_ref_pub, &foc_ref);
        }
    }
    return 1;
}

