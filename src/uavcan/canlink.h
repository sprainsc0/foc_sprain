#ifndef __CANLINK_H__
#define __CANLINK_H__

#include <stdint.h>
#include "cmsis_os.h"
#include <ipc.h>
#include "param.h"
#include "uPerf.h"
#include "uavcan.h"

#include "topics/parameter_update.h"
#include "topics/encoder.h"
#include "topics/foc_status.h"
#include "topics/foc_command.h"

class Canlink
{
public:
    Canlink(void);
    
    void *_param;

    void send(void *parameter);
    void recv(void *parameter);

    bool init(void);

    void TransferReceived(CanardInstance* ins, CanardRxTransfer* transfer);

protected:
    osThreadId_t _send_handle;
    osThreadId_t _recv_handle;
private:
    CanardInstance canard;

    int _encoder_sub;
    struct encoder_s _encoder_data;

    int _foc_states_sub;
    struct foc_status_s _state_data;

    orb_advert_t _cmd_pub;
    struct foc_command_s _cmd_value;

    void processTasksLow(uint64_t timestamp_usec);
    
};

#endif
