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
    __disable_irq();

    orb_advert_t advert = NULL;
    
    //get the serial number of the current orb
    uint16_t serial = meta->serial;
        
    //reset the published flag to false to prevent subscribing and checking
    ipc_data[serial]->published = false;
    
    if(ipc_data[serial]->buffer == nullptr && meta->buffer) {
        ipc_data[serial]->buffer = new ringbuffer::RingBuffer(meta->b_size, meta->o_size);
        ipc_data[serial]->buffer->flush();
    }

    if(ipc_data[serial]->data == NULL) {
        ipc_data[serial]->data = (uint8_t *)pvPortMalloc(meta->o_size);
        memset(ipc_data[serial]->data, 0, meta->o_size);
    }
    //copy the data
    memcpy(ipc_data[serial]->data, data, meta->o_size);
    
    //copy the serial number
    ipc_data[serial]->serial = serial;

    //name the advert as the pointer of the orb's internal data
    advert = (void*)(ipc_data[serial]);
    
    //update the published flag to true
    ipc_data[serial]->published = false;

    ipc_data[serial]->authority_list = 0x00000000;
    

    __enable_irq();
    
    return advert;
}

void ipc_inactive(orb_advert_t handle)
{
    handle = NULL;
}

int ipc_subscibe(const struct ipc_metadata *meta)
{
    if(meta == NULL || meta->o_name == NULL)
        return 0;
    
    __disable_irq();

    uint16_t serial = meta->serial;
    
    int ret = 0;

    for(int i = 0; i < 32; ++i) {
        if(!(ipc_data[serial]->registered_list & (1<<i))) {
            ipc_data[serial]->registered_list |= (1<<i);
            ret = (int)((serial << 24) | (i << 4) | (0));
            break;
        }
    }
    
    __enable_irq();

    return ret;
}

int ipc_unsubscibe(int handle)
{
    if(handle < 0) {
        return 0;
    }
    
    int serial = (handle >> 24);
    int sub_num = (handle >> 4) & ~0xFFFFFF00;
    
    ipc_data[serial]->registered_list &= ~(1<<sub_num);
    
    return 1;
}

int ipc_push(const struct ipc_metadata *meta, orb_advert_t handle, const void *data)
{  
    __disable_irq();

    uint16_t serial = meta->serial;

    if(ipc_data[serial]->buffer != NULL && meta->buffer) {
        if(!ipc_data[serial]->buffer->full()) {
            ipc_data[serial]->buffer->put(data, meta->o_size);
        }
    }

    ipc_data[serial]->published = false;

    memcpy(ipc_data[serial]->data, data, meta->o_size);

    ipc_data[serial]->serial = serial;
    
    ipc_data[serial]->published = true;
    
    ipc_data[serial]->authority_list = 0x00000000;
    
    __enable_irq();
    
    return 1;
}

int ipc_pull(const struct ipc_metadata *meta, int handle, void *buffer)
{
    __disable_irq();

    int ret = 0;
    uint16_t serial = meta->serial;
    int sub_num = (handle >> 4) & ~0xFFFFFF00;

    if(serial == (handle >> 24)) {

        if(ipc_data[serial]->buffer != NULL && meta->buffer && !ipc_data[serial]->buffer->empty()) {

            ipc_data[serial]->buffer->get(ipc_data[serial]->data, meta->o_size);
            
            memcpy(buffer, ipc_data[serial]->data, meta->o_size);

            ret = 1;
            __enable_irq();
            return ret;
        }
        
        if(ipc_data[serial]->data != NULL) 
        {
            bool authorised = false;
                
            if(ipc_data[serial]->authority_list & (1<<sub_num)) {
                authorised = true;
            }

            if(!(ipc_data[serial]->authority_list & (1<<sub_num))) {
                ipc_data[serial]->authority_list |= (1<<sub_num);
                authorised = true;
            }
            
            ret = 1;

            if(authorised) {
                memcpy(buffer, ipc_data[serial]->data, meta->o_size);
            }
            
            if(ipc_data[serial]->authority_list >= ipc_data[serial]->registered_list) {
                ipc_data[serial]->published = false;
            }
        }
    }
    __enable_irq();
    
    return ret;
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

    if(ipc_data[serial]->buffer != nullptr) {
        if(!ipc_data[serial]->buffer->empty()) {
            *updated = true;
        } else {
            *updated = false;
        }
        return 1;
    }

    bool authorised = false;

    if(ipc_data[serial]->authority_list & (1<<sub_num)) {
        authorised = true;
    }
    
    if(ipc_data[serial]->published && !authorised) {
        *updated = true;
    } else {
        *updated = false;
    }
    
    return 1;
}

void ipc_init(void)
{
    for(int i = 0; i < TOPICS_COUNT; ++i) {
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
