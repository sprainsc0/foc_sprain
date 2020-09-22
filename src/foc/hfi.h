#ifndef __HFI_H__
#define __HFI_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"


class HFI
{
public:
    HFI(void);

    void *_param;
    void run(void *parameter);

    bool init(void);

protected:
    osThreadId_t _handle;
private:
    
};

#endif
