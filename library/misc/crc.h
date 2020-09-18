#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

uint16_t crc_16_check(const void *buffer, uint32_t buffer_size);

#endif
