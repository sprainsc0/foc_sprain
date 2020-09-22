#ifndef __HFI_H__
#define __HFI_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"
#include "param.h"

class HFI
{
public:
    HFI(void);

    void *_param;
    void run(void *parameter);

    bool init(void);

    void parameter_update(bool force);

protected:
    osThreadId_t _handle;
private:
    struct hfi_config {
        float motor_r;
        float motor_l;
        float motor_l_diff;
        float flux_linkage;
    } _hfi_cfg;

    struct {
        param_t motor_r_handle;
        param_t motor_l_handle;
        param_t motor_l_diff_handle;
        param_t flux_linkage_handle;
    } _param_handles;
};

#endif
