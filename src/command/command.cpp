#include "command.h"
#include <string>
#include "hrt_timer.h"
#include "foc_function.h"
#include "debug.h"

const osThreadAttr_t cmd_attributes = {
		.name = "cmd",
		.priority = (osPriority_t)osPriorityAboveNormal,
		.stack_size = 2048};

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
    _commander_sub = ipc_subscibe(IPC_ID(foc_command));
    _led_notify_pub = ipc_active(IPC_ID(actuator_notify), &_led_notify);
    
    _handle = osThreadNew((osThreadFunc_t)cmd_func, this, &cmd_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void Command::enter_cali_mode(void)
{
    _led_notify.timestamp = micros();
    _led_notify.led_status = LED_PATTERN_BGC_CAL;
    ipc_push(IPC_ID(actuator_notify), _led_notify_pub, &_led_notify);
}

void Command::exit_cali_mode(void)
{
    _led_notify.timestamp = micros();
    _led_notify.led_status = LED_PATTERN_BGC_DISARM;
    ipc_push(IPC_ID(actuator_notify), _led_notify_pub, &_led_notify);
}

void Command::run(void *parameter)
{
    while (1)
    {
        bool cmd_updated = false;
        ipc_check(_commander_sub, &cmd_updated);
        if (cmd_updated) {
            ipc_pull(IPC_ID(foc_command), _commander_sub, &_command);
            switch(_command.command) {
            case CMD_PRE_CALIBRATION:
                if (_command.sub_cmd == 1) {
                    enter_cali_mode();
                    enc_e = new Enc_CalE(_cal_status_pub);
                    enc_e->do_calibration(_command.param1, (uint8_t)_command.param2);
                    delete enc_e;
                    exit_cali_mode();
                }  else if (_command.sub_cmd == 2) {
                    enter_cali_mode();
                    enc_m = new Enc_CalM(_cal_status_pub);
                    enc_m->do_calibration();
                    delete enc_m;
                    exit_cali_mode();
                } else if (_command.sub_cmd == 3) {
                    enter_cali_mode();
                    flux = new FLUX_Cal(_cal_status_pub);
                    flux->do_calibration();
                    delete flux;
                    exit_cali_mode();
                } else if (_command.sub_cmd == 4) {
                    enter_cali_mode();
                    ind = new IND_Cal(_cal_status_pub);
                    ind->do_calibration();
                    delete ind;
                    exit_cali_mode();
                } else if (_command.sub_cmd == 5) {
                    enter_cali_mode();
                    res = new RES_Cal(_cal_status_pub);
                    res->do_calibration(_command.param1);
                    delete res;
                    exit_cali_mode();
                }
                break;
            }
        }
        // 10Hz loop
        osDelay(100);
    }
}

int Commander_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 0;
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

        if (!strcmp(argv[i], "ence")) {
            if (argc < 3) {
                Info_Debug("input argv error\n");
                Info_Debug("cmd ence [torque] [use_current]\n");
                return 0;
            }
            struct foc_command_s command;
            orb_advert_t cmd_pub = ipc_active(IPC_ID(foc_command), &command);
            float param = (float)atof(argv[++i]);
            uint8_t param2 = (uint8_t)atoi(argv[++i]);
            command.timestamp = micros();
            command.command = CMD_PRE_CALIBRATION;
            command.sub_cmd = 1;
            command.param1 = param;
            command.param2 = (float)param2;

            ipc_push(IPC_ID(foc_command), cmd_pub, &command);

            ipc_inactive(cmd_pub);
        }

        if (!strcmp(argv[i], "encm")) {
            struct foc_command_s command;
            orb_advert_t cmd_pub = ipc_active(IPC_ID(foc_command), &command);
            command.timestamp = micros();
            command.command = CMD_PRE_CALIBRATION;
            command.sub_cmd = 2;

            ipc_push(IPC_ID(foc_command), cmd_pub, &command);

            ipc_inactive(cmd_pub);
        }
    }
    return 1;
}
