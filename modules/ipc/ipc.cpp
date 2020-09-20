#include "ipc.h"
#include "ipchelper.h"
#include <cstdlib>
#include <cstring>
#include "cmsis_os.h"
#include "hrt_timer.h"
#include "debug.h"

static IPCType        *ipc_data[TOPICS_COUNT];

/*
 * 32           24             12        4           0
 * | serial no. |      N/A     |   SUB   |  instance |
 *     127                         127         7
 */

orb_advert_t ipc_active(const struct ipc_metadata *meta, const void *data)
{   
    return ipc_active_multi(meta, data, NULL, IPC_PRIO_HIGH);
}

orb_advert_t ipc_active_multi(const struct ipc_metadata *meta, const void *data, int *instance, int priority)
{
    portDISABLE_INTERRUPTS();

    orb_advert_t advert = NULL;
    
    //get the serial number of the current orb
    uint16_t serial = meta->serial;
    
    //search for existing orb
    static int inst = get_orb_instance_according_to_priority(priority);
    
    if(!meta->multi)
        inst = 0;
    
    if(instance != NULL)
        *instance = inst;
    
    //If we find it, copy the data
    if(inst != -1) {
        
        //reset the published flag to false to prevent subscribing and checking
        ipc_data[serial][inst].published = false;

        // if(meta->sem && ipc_data[serial][inst].sem == nullptr) {
        //     ipc_data[serial][inst].sem = new OS_FLAG_GRP;
        //     OSFlagCreate(ipc_data[serial][inst].sem, (char*)meta->o_name, 0x00, &err);
        //     if(err != OS_ERR_NONE) {
        //         delete ipc_data[serial][inst].sem;
        //         ipc_data[serial][inst].sem = nullptr;
        //         Info_Debug("ipc create error\n");
        //     }
        // }
        
        if(ipc_data[serial][inst].buffer == nullptr && meta->buffer) {
            ipc_data[serial][inst].buffer = new ringbuffer::RingBuffer(meta->b_size, meta->o_size);
            ipc_data[serial][inst].buffer->flush();
        }

        if(ipc_data[serial][inst].data == NULL) {
            ipc_data[serial][inst].data = (uint8_t *)pvPortMalloc(meta->o_size);
            memset(ipc_data[serial][inst].data, 0, meta->o_size);
        }
        //copy the data
        memcpy(ipc_data[serial][inst].data, data, meta->o_size);
        
        //copy the serial number
        ipc_data[serial][inst].serial = serial;
    
        //name the advert as the pointer of the orb's internal data
        advert = (void*)(&ipc_data[serial][inst]);
        
        //update the published flag to true
        ipc_data[serial][inst].published = false;

        ipc_data[serial][inst].authority_list = 0x00000000;
    }

    portENABLE_INTERRUPTS();
    
    return advert;
}

void ipc_inactive(orb_advert_t handle)
{
    handle = NULL;
}

int ipc_subscibe(const struct ipc_metadata *meta)
{
    return ipc_subscibe_multi(meta, 0);
}

int ipc_subscibe_multi(const struct ipc_metadata *meta, unsigned instance)
{
    if(meta == NULL || meta->o_name == NULL)
        return 0;
    
    portDISABLE_INTERRUPTS();

    uint16_t serial = meta->serial;
    
    int ret = 0;

    if(!meta->multi)
        instance = 0;

    for(int i = 0; i < 32; ++i) {
        if(!(ipc_data[serial][instance].registered_list & (1<<i))) {
            ipc_data[serial][instance].registered_list |= (1<<i);
            ret = (int)((serial << 24) | (i << 4) | (instance));
            break;
        }
    }
    
    portENABLE_INTERRUPTS();

    return ret;
}

int ipc_unsubscibe(int handle)
{
    if(handle < 0) {
        return 0;
    }
    
    int serial = (handle >> 24);
    int sub_num = (handle >> 4) & ~0xFFFFFF00;
    
    int instance = 0;
    if(is_orb_multi(serial)) {
        instance = IPC_MULTI_MAX_INSTANCES - 1;
    }
    
    ipc_data[serial][instance].registered_list &= ~(1<<sub_num);
    
    return 1;
}

int ipc_push(const struct ipc_metadata *meta, orb_advert_t handle, const void *data)
{  
    portDISABLE_INTERRUPTS();

    uint16_t serial = meta->serial;

    int instance = 0;

    if(meta->multi) {
        for(int i = 0; i < IPC_MULTI_MAX_INSTANCES; ++i) {
            if(handle == &ipc_data[serial][i]) {
                instance = i;
                break;
            }
        }
    }

    if(ipc_data[serial][instance].buffer != NULL && meta->buffer) {
        if(!ipc_data[serial][instance].buffer->full()) {
            ipc_data[serial][instance].buffer->put(data, meta->o_size);
        }
    }

    ipc_data[serial][instance].published = false;

    memcpy(ipc_data[serial][instance].data, data, meta->o_size);

    ipc_data[serial][instance].serial = serial;
    
    ipc_data[serial][instance].published = true;
    
    ipc_data[serial][instance].authority_list = 0x00000000;
    
    // if(meta->sem && ipc_data[serial][instance].sem != nullptr) {
    //     OSFlagPost(ipc_data[serial][instance].sem, 0xFFFFFFFF, OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED, &err);
    //     if(err != OS_ERR_NONE) {
    //         Info_Debug("ipc post error\n");
    //     }
    // }
    portENABLE_INTERRUPTS();
    
    return 1;
}

int ipc_pull(const struct ipc_metadata *meta, int handle, void *buffer)
{
    portDISABLE_INTERRUPTS();

    int ret = 0;
    uint16_t serial = meta->serial;
    int sub_num = (handle >> 4) & ~0xFFFFFF00;

    if(serial == (handle >> 24)) {
        int instance = handle & ~0xFFFFFFF0;
        
        if(instance > 0 && !meta->multi) {
            portENABLE_INTERRUPTS();
            return ret;
        }

        if(ipc_data[serial][instance].buffer != NULL && meta->buffer && !ipc_data[serial][instance].buffer->empty()) {

            ipc_data[serial][instance].buffer->get(ipc_data[serial][instance].data, meta->o_size);
            
            memcpy(buffer, ipc_data[serial][instance].data, meta->o_size);

            ret = 1;
            portENABLE_INTERRUPTS();
            return ret;
        }
        
        if(instance >= 0 && instance < IPC_MULTI_MAX_INSTANCES && 
           ipc_data[serial][instance].data != NULL) 
        {
            bool authorised = false;
                
            if(ipc_data[serial][instance].authority_list & (1<<sub_num)) {
                authorised = true;
            }

            if(!(ipc_data[serial][instance].authority_list & (1<<sub_num))) {
                ipc_data[serial][instance].authority_list |= (1<<sub_num);
                authorised = true;
            }
            
            ret = 1;

            if(authorised) {
                memcpy(buffer, ipc_data[serial][instance].data, meta->o_size);
                //CPU_DCACHE_RANGE_INV((void *)buffer, meta->o_size);
            }
            
            if(ipc_data[serial][instance].authority_list >= ipc_data[serial][instance].registered_list) {
                ipc_data[serial][instance].published = false;
            }
        }
    }
    portENABLE_INTERRUPTS();
    
    return ret;
}

int ipc_wait(int handle, int timeout)
{
    if(handle < 0) {
        return 0;
    }

    // int serial = (handle >> 24);
    // int instance = handle & ~0xFFFFFFF0;
    // int sub_num = (handle >> 4) & ~0xFFFFFF00;

    // if(ipc_data[serial][instance].sem != nullptr) {
    //     OSFlagPend(ipc_data[serial][instance].sem, (OS_FLAGS)(1<<sub_num), (OS_TICK)(TICK_PER_MS * timeout), OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME, &ts, &err);
    //     if(err == OS_ERR_TIMEOUT) {
    //         return B_TIMEOUT;
    //     } else if(err != OS_ERR_NONE) {
    //         Info_Debug("ipc pend error-%d\n", err);
    //         return 0;
    //     }
    // } else {
    //     return 0;
    // }

    return 1;
}

int ipc_check(int handle, bool *updated)
{
    if(handle < 0) {
        *updated = false;
        return 0;
    }
    
    int serial = (handle >> 24);
    int instance = handle & ~0xFFFFFFF0;
    int sub_num = (handle >> 4) & ~0xFFFFFF00;

    if(ipc_data[serial][instance].buffer != nullptr) {
        if(!ipc_data[serial][instance].buffer->empty()) {
            *updated = true;
        } else {
            *updated = false;
        }
        return 1;
    }

    bool authorised = false;

    if(ipc_data[serial][instance].authority_list & (1<<sub_num)) {
        authorised = true;
    }
    
    if(ipc_data[serial][instance].published && !authorised) {
        *updated = true;
    } else {
        *updated = false;
    }
    
    return 1;
}

void ipc_init(void)
{
    for(int i = 0; i < TOPICS_COUNT; ++i) {
        if(is_orb_multi(i)) {
            ipc_data[i] = (IPCType*)pvPortMalloc(IPC_MULTI_MAX_INSTANCES * sizeof(IPCType));
            
            for(int j = 0; j < IPC_MULTI_MAX_INSTANCES; ++j) {
                ipc_data[i][j].data = NULL; 
                ipc_data[i][j].buffer = NULL;
                ipc_data[i][j].priority = (IPC_PRIO)get_priority(j);
                ipc_data[i][j].serial = -1;
                ipc_data[i][j].published = false;
                
                ipc_data[i][j].registered_list = 0x00000000;
                ipc_data[i][j].authority_list = 0x00000000;
            }
        }
        else {
            ipc_data[i] = (IPCType*)pvPortMalloc(sizeof(IPCType));
                
            ipc_data[i]->data = NULL;
            ipc_data[i]->buffer = NULL;
            ipc_data[i]->priority = IPC_PRIO_HIGH;
            ipc_data[i]->serial = -1;
            ipc_data[i]->published = false;
            
            ipc_data[i]->registered_list = 0x00000000;
            ipc_data[i]->authority_list = 0x00000000;
        }
    }
}
