#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"

#include "elec_calibration.h"
#include "mech_calibration.h"
#include "res_detected.h"
#include "ind_detected.h"
#include "fluxlinkage_detected.h"

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

    Enc_CalE *enc_e;
    Enc_CalM *enc_m;
    FLUX_Cal *flux;
    IND_Cal  *ind;
    RES_Cal  *res;

    // command subscribe
    int _commander_sub;
    struct foc_command_s _command;

    orb_advert_t _cal_status_pub;
    
    orb_advert_t _led_notify_pub;
    struct actuator_notify_s _led_notify;

    void enter_cali_mode(void);
    void exit_cali_mode(void);
    
};

#endif
