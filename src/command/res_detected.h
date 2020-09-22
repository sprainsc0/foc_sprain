#ifndef __RES_CAL_H__
#define __RES_CAL_H__

#include "ipc.h"
#include <param.h>

#include "topics/parameter_update.h"
#include "topics/foc_target.h"
#include "topics/foc_status.h"
#include "topics/actuator_notify.h"
#include "topics/calibrate_status.h"

class RES_Cal
{
public:
    RES_Cal(orb_advert_t &cal_status_pub);
    ~RES_Cal(void);
    
    bool do_calibration(void);

    void send_status(uint8_t status);
    
private:
    
    // ipc value
    int _params_sub;
    int _foc_status_sub;

    orb_advert_t _foc_ref_pub;
    struct foc_target_s _foc_ref;

    orb_advert_t &_mavlink_cal_pub;
    struct calibrate_status_s _cal_status;

    uint8_t foc_mode;
    
    struct {
        param_t motor_res_handle;
    } _cal_params_handles;

    struct encoder_calibration_s {
		float   motor_res;
	} _res_calibration;
    
    void set_res_parameter(void);
};

#endif