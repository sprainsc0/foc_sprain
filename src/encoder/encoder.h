#ifndef MC_ENCODER_H
#define MC_ENCODER_H

#include <string.h>

#include <stdint.h>
#include "cmsis_os.h"
#include "datatypes.h"
#include "uPerf.h"
#include "param.h"

#include "topics/parameter_update.h"
#include "topics/encoder.h"

class Encoder
{
public:
    Encoder(void);
    ~Encoder(void);

    void *_param;
    void run(void *parameter);

    bool init(void);
    void enc_process(void);

    void parameter_update(bool force);

    struct encoder_s get_encoder(void) const { return _enc_data; }

protected:
    osThreadId_t _handle;

private:
	struct enc_config {
        int   type;
        float offset_e;
        float offset_m;
		int   invert_e;
		int   invert_m;
		int   pair_num;
    } _enc_cfg;

	struct {
        param_t type_handle;
        param_t offset_e_handle;
        param_t offset_m_handle;
		param_t invert_e_handle;
        param_t invert_m_handle;
		param_t pair_num_handle;
    } _param_handles;

	int _params_sub;

	orb_advert_t _encoder_pub;
    struct encoder_s _enc_data;

    bool _enc_ready;

	perf_counter_t enc_tim_int;
    perf_counter_t enc_tim_ela;

    perf_counter_t enc_task_int;
    perf_counter_t enc_task_ela;

    perf_counter_t enc_err_count;
};

void enc_init(void);
void enc_handle(void);
bool enc_elec_calibrate(float param);
bool enc_mech_calibrate(void);
void enc_linear_detect(void);

#endif
