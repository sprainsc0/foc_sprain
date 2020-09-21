#ifndef __IPC_HELPER_H
#define __IPC_HELPER_H

#include "ipc.h"
#include "ringbuffer_cpp.h"

struct IPCType{
    uint8_t                             *data;
    ringbuffer::RingBuffer              *buffer;
    int                                 serial;
    bool                                published;
    uint32_t                            registered_list;
    uint32_t                            authority_list;
};

extern IPCType          *ipc_data[TOPICS_COUNT];

#endif
