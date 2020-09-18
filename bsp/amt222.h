#ifndef AMT222_H
#define AMT222_H

#ifdef __cplusplus
extern "C"{
#endif

#include "cmsis_os.h"
#include "spi.h"
#include "datatypes.h"

bool hal_amt222_read(uint32_t *raw);
void hal_amt222_reset(void);

#ifdef __cplusplus
}
#endif

#endif
