#ifndef DRV8301_H
#define DRV8301_H

#ifdef __cplusplus
extern "C"{
#endif

#include "cmsis_os.h"
#include "spi.h"
#include "datatypes.h"

bool hal_drv8301_init(void);

#ifdef __cplusplus
}
#endif

#endif
