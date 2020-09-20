#ifndef __ENCM_CAL_H__
#define __ENCM_CAL_H__

#include "ipc.h"
#include <param.h>

#include "topics/parameter_update.h"
#include "topics/encoder.h"
#include "topics/foc_target.h"
#include "topics/foc_status.h"
#include "topics/actuator_notify.h"
#include "topics/calibrate_status.h"

class Enc_CalM
{
public:
    Enc_CalM(orb_advert_t &cal_status_pub);
    ~Enc_CalM(void);
    
    bool do_calibration(void);

    void send_status(uint8_t status);
    
private:

    struct EncSample {
        float ang_rad;
        uint16_t sample_count;
    };
    
    // ipc value
    int _params_sub;
    int _enc_sub;
    int _foc_status_sub;

    orb_advert_t _foc_ref_pub;
    struct foc_target_s _foc_ref;

    orb_advert_t &_mavlink_cal_pub;
    struct calibrate_status_s _cal_status;

    struct EncSample _sample_buffer;

    uint8_t foc_mode;
    
    struct {
        param_t offset_m_handle;
    } _cal_params_handles;

    struct encoder_calibration_s {
		float   offset_m;
	} _enc_calibration;
    
    void set_enc_parameter(void);
    float collect_sample(void);
    bool enc_stable_waite(float var);
};

#endif