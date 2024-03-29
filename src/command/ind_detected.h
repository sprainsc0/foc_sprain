#ifndef __IND_CAL_H__
#define __IND_CAL_H__

#include "ipc.h"
#include <param.h>

#include "topics/parameter_update.h"
#include "topics/foc_target.h"
#include "topics/foc_status.h"
#include "topics/actuator_notify.h"
#include "topics/calibrate_status.h"

class IND_Cal
{
public:
    IND_Cal(orb_advert_t &cal_status_pub);
    ~IND_Cal(void);
    
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
        param_t motor_ind_handle;
        param_t ind_diff_handle;
    } _cal_params_handles;

    struct encoder_calibration_s {
		float   motor_ind;
        float   ind_diff;
	} _ind_calibration;
    
    void set_ind_parameter(void);
};

#endif