#include "canlink.h"
#include <string>
#include "hrt_timer.h"
#include "foc_function.h"
#include "debug.h"

const osThreadAttr_t cans_attributes = {
    .name = "cans",
    .priority = (osPriority_t)osPriorityRealtime1,
    .stack_size = 512};

const osThreadAttr_t canr_attributes = {
    .name = "canr",
    .priority = (osPriority_t)osPriorityRealtime,
    .stack_size = 512};

namespace CANBUS
{
static Canlink	*gCan;
static uint8_t canard_memory_pool[2048];
static uint8_t *dyn_buf = NULL;
}

static void cans_func(Canlink *pThis)
{
    pThis->send(pThis->_param);
}

static void canr_func(Canlink *pThis)
{
    pThis->recv(pThis->_param);
}

static void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer)
{
    if(CANBUS::gCan != nullptr) {
        CANBUS::gCan->TransferReceived(ins, transfer);
    }
}

static bool shouldAcceptTransfer(const CanardInstance* ins,
                            uint64_t* out_data_type_signature,
                            uint16_t data_type_id,
                            CanardTransferType transfer_type,
                            uint8_t source_node_id)
{
    switch(data_type_id) {
    case UAVCAN_PROTOCOL_NODESTATUS_ID:
        *out_data_type_signature = UAVCAN_EQUIPMENT_ESC_STATUS_SIGNATURE;
        break;
    case UAVCAN_EQUIPMENT_ESC_COMMAND_ID:
        *out_data_type_signature = UAVCAN_EQUIPMENT_ESC_COMMAND_SIGNATURE;
        break;
    case UAVCAN_PROTOCOL_PARAM_GETSET_ID:
        *out_data_type_signature = UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE;
        break;
    default:
        return false;
    }
    return true;
}

Canlink::Canlink(void):
    _param(NULL)
{
    
}

bool Canlink::init(void)
{
    _encoder_sub    = ipc_subscibe(IPC_ID(encoder));
    _foc_states_sub = ipc_subscibe(IPC_ID(foc_status));
    _cmd_pub        = ipc_active(IPC_ID(foc_command), &_cmd_value);

    uavcan_init();

    canardInit(&canard, CANBUS::canard_memory_pool, sizeof(CANBUS::canard_memory_pool), onTransferReceived, shouldAcceptTransfer, NULL);

    canardSetLocalNodeID(&canard, LOCAL_MOTOR_ID);

    CANBUS::dyn_buf = (uint8_t *)pvPortMalloc(256);
    
    _send_handle = osThreadNew((osThreadFunc_t)cans_func, this, &cans_attributes);
    _recv_handle = osThreadNew((osThreadFunc_t)canr_func, this, &canr_attributes);

    if ((_send_handle == nullptr) || (_recv_handle == nullptr)) {
        return false;
    }

	return true;
}

void Canlink::processTasksLow(uint64_t timestamp_usec)
{
    // 定时清除不再发送数据的缓存
    canardCleanupStaleTransfers(&canard, timestamp_usec);

    // 检测CAN动态缓存池大小
    const CanardPoolAllocatorStatistics stats = canardGetPoolAllocatorStatistics(&canard);
    const uint16_t peak_percent = (uint16_t)(100U * stats.peak_usage_blocks / stats.capacity_blocks);

    if (peak_percent > 70){
        
    }

    // spin
    uint8_t buffer[UAVCAN_EQUIPMENT_ESC_STATUS_MAX_SIZE];
    static uint8_t status_transfer_id = 0;
    memset(buffer, 0, UAVCAN_EQUIPMENT_ESC_STATUS_MAX_SIZE);

    uavcan_equipment_esc_Status nodestatus;
    nodestatus.current = 0.0f;
    nodestatus.voltage = 12.0f;
    nodestatus.mode    = 0;
    nodestatus.temperature = 40.0f;

    const uint32_t total_size = uavcan_equipment_esc_Status_encode(&nodestatus, buffer);

    const int16_t resp_res = canardBroadcast(&canard,
                                            UAVCAN_EQUIPMENT_ESC_STATUS_SIGNATURE,
                                            UAVCAN_EQUIPMENT_ESC_STATUS_ID,
                                            &status_transfer_id,
                                            CANARD_TRANSFER_PRIORITY_LOWEST,
                                            &buffer[0],
                                            (uint16_t)total_size);
    if (resp_res <= 0) {
        
    }
}

void Canlink::TransferReceived(CanardInstance* ins, CanardRxTransfer* transfer)
{
    if ((transfer->transfer_type == CanardTransferTypeBroadcast) && 
        (transfer->data_type_id == UAVCAN_PROTOCOL_NODESTATUS_ID) &&
        (transfer->source_node_id == GIMBAL_ID)) {
        uavcan_protocol_NodeStatus nodestatus;
        uavcan_protocol_NodeStatus_decode(transfer, transfer->payload_len, &nodestatus, &CANBUS::dyn_buf);
        if(nodestatus.mode != UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL) {

        }
    }

    if((transfer->transfer_type == CanardTransferTypeRequest) && 
        (transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID)) {
        uavcan_protocol_param_GetSetRequest requset;
        uavcan_protocol_param_GetSetResponse param;
        uavcan_protocol_param_GetSetRequest_decode(transfer, transfer->payload_len, &requset, &CANBUS::dyn_buf);
        requset.name.data[requset.name.len] = '\0';

        param_t param_h = param_find_no_notification((const char *)requset.name.data);
        if (param_h == PARAM_INVALID) {
            return;
        }

		param.name.data = requset.name.data;
		param.name.len = requset.name.len;
		param.index = param_get_index(param_h);
        if(requset.rw == 20) {
            // read parameter
            param_type_t type = param_type(param_h);

            if(type == PARAM_TYPE_FLOAT) {
                float param_value;
                param_get(param_h, &param_value);
                param.value.real_value = param_value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
            } else {
                int32_t param_value;
                param_get(param_h, &param_value);
                param.value.integer_value = param_value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
            }
        } else if(requset.rw == 23) {

            // write parameter
            if(requset.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE) {
                float curr_val = requset.value.real_value;
                param_set(param_h, &curr_val);
				param.value.real_value = requset.value.real_value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
            } else {
                int32_t curr_val = requset.value.integer_value;
                param_set(param_h, &curr_val);
				param.value.integer_value = requset.value.integer_value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
            }
        }

        uint8_t buffer[UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_MAX_SIZE];
        static uint8_t param_transfer_id = 0;
        memset(buffer, 0, UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_MAX_SIZE);
        const uint32_t total_size = uavcan_protocol_param_GetSetResponse_encode(&param, buffer);
        
        const int16_t resp_res = canardRequestOrRespond(&canard,
                                                        transfer->source_node_id,
                                                        UAVCAN_PROTOCOL_PARAM_GETSET_SIGNATURE,
                                                        UAVCAN_PROTOCOL_PARAM_GETSET_ID,
                                                        &param_transfer_id,
                                                        CANARD_TRANSFER_PRIORITY_MEDIUM,
                                                        CanardResponse,
                                                        &buffer[0],
                                                        (uint16_t)total_size);
        if (resp_res <= 0) {
        
        }
    }

    if ((transfer->transfer_type == CanardTransferTypeBroadcast) && 
        (transfer->data_type_id == UAVCAN_EQUIPMENT_ESC_COMMAND_ID) &&
        (transfer->source_node_id == GIMBAL_ID)) {
        uavcan_equipment_esc_Command cmd;
        uavcan_equipment_esc_Command_decode(transfer, transfer->payload_len, &cmd, &CANBUS::dyn_buf);

        _cmd_value.timestamp = micros();
        _cmd_value.command = cmd.cmd;
        _cmd_value.sub_cmd = cmd.sub_cmd;
        _cmd_value.param1  = cmd.param1;
        _cmd_value.param2  = cmd.param2;
        ipc_push(IPC_ID(foc_command), _cmd_pub, &_cmd_value);
    }
    
    if ((transfer->transfer_type == CanardTransferTypeRequest) && 
        (transfer->data_type_id == UAVCAN_EQUIPMENT_ESC_COMMAND_ID)) {
        uavcan_equipment_esc_Command cmd;
        uavcan_equipment_esc_Command_decode(transfer, transfer->payload_len, &cmd, &CANBUS::dyn_buf);

        _cmd_value.timestamp = micros();
        _cmd_value.command = cmd.cmd;
        _cmd_value.sub_cmd = cmd.sub_cmd;
        _cmd_value.param1  = cmd.param1;
        _cmd_value.param2  = cmd.param2;
        ipc_push(IPC_ID(foc_command), _cmd_pub, &_cmd_value);
    }
}

void Canlink::processEncoder(void)
{
    uavcan_equipment_esc_Encoder enc;
    uint8_t buffer[UAVCAN_EQUIPMENT_ESC_ENCODER_MAX_SIZE];
    static uint8_t encoder_transfer_id = 0;
    memset(buffer, 0, UAVCAN_EQUIPMENT_ESC_ENCODER_MAX_SIZE);

    ipc_pull(IPC_ID(encoder), _encoder_sub, &_encoder_data);

    enc.encoder = _encoder_data.angle_m;

    const uint32_t total_size = uavcan_equipment_esc_Encoder_encode(&enc, buffer);

    const int16_t resp_res = canardBroadcast(&canard,
                                            UAVCAN_EQUIPMENT_ESC_ENCODER_SIGNATURE,
                                            UAVCAN_EQUIPMENT_ESC_ENCODER_ID,
                                            &encoder_transfer_id,
                                            CANARD_TRANSFER_PRIORITY_HIGHEST,
                                            &buffer[0],
                                            (uint16_t)total_size);
    if (resp_res <= 0) {
        
    }
}

void Canlink::send(void *parameter)
{
    uint64_t next_1hz_service_at = micros();

    while (1)
    {
        const uint64_t ts = micros();

        processEncoder();
        
        if(ts >= next_1hz_service_at) {
				next_1hz_service_at += 1000000;
				processTasksLow(ts);
		}

		for (const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&canard)) != NULL;) {
            const int16_t tx_res = CAN_Transmit(txf);
            if(tx_res > 0) {
                canardPopTxQueue(&canard);
            } else {
                osDelay(1);
            }
		}
        osDelay(2);
    }
}

void Canlink::recv(void *parameter)
{
    while (1)
    {
        const uint64_t ts = micros();

        CanardCANFrame rx_frame;
        for (int16_t rx_res=0; (rx_res = CAN_Receive(&rx_frame)) > 0;) {
            if(rx_res > 0) {
                canardHandleRxFrame(&canard, &rx_frame, ts);
            }
        }

        osDelay(10);
    }
}


int canlink_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {

            if (CANBUS::gCan != nullptr) {
                Info_Debug("already running\n");
                return 0;
            }

            CANBUS::gCan = new Canlink();
            

            if (CANBUS::gCan == NULL) {
                Info_Debug("alloc failed\n");
                return 0;
            }
            CANBUS::gCan->init();
        }
    }
    return 1;
}
