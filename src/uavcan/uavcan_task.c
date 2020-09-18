#include "platform.h"
#include "uavcan.h"
#include "encoder.h"
#include "mc.h"
#include "utils.h"
#include "param.h"

static uint8_t canard_memory_pool[2048];
static CanardInstance canard;
static uint8_t *dyn_buf = NULL;
float command_param = 0.0f;

void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer);
bool shouldAcceptTransfer(const CanardInstance* ins,
                            uint64_t* out_data_type_signature,
                            uint16_t data_type_id,
                            CanardTransferType transfer_type,
                            uint8_t source_node_id);

// can invoked
void processTasksLow(uint64_t timestamp_usec);

void uavcan_process_init()
{
    canardInit(&canard, canard_memory_pool, sizeof(canard_memory_pool), onTransferReceived, shouldAcceptTransfer, NULL);
    
    canardSetLocalNodeID(&canard, mc_cfg.version.node_id);

    dyn_buf = (uint8_t *)pvPortMalloc(256);
}

void processEncoder(void)
{
    uavcan_equipment_esc_Encoder enc;
    uint8_t buffer[UAVCAN_EQUIPMENT_ESC_ENCODER_MAX_SIZE];
    static uint8_t encoder_transfer_id = 0;
    memset(buffer, 0, UAVCAN_EQUIPMENT_ESC_ENCODER_MAX_SIZE);

    enc.encoder = encoder.mech_angle;

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

void commandack(uint8_t cmd, float param1, float param2)
{
    uavcan_equipment_esc_Command ack;
    uint8_t buffer[UAVCAN_EQUIPMENT_ESC_COMMAND_MAX_SIZE];
    static uint8_t commandack_transfer_id = 0;
    memset(buffer, 0, UAVCAN_EQUIPMENT_ESC_COMMAND_MAX_SIZE);

    ack.cmd     = cmd;
    ack.sub_cmd = 255;
    ack.param1 = param1;
    ack.param2 = param2;

    const uint32_t total_size = uavcan_equipment_esc_Command_encode(&ack, buffer);

    const int16_t resp_res = canardRequestOrRespond(&canard,
                                                    GIMBAL_ID,
                                                    UAVCAN_EQUIPMENT_ESC_COMMAND_SIGNATURE,
                                                    UAVCAN_EQUIPMENT_ESC_COMMAND_ID,
                                                    &commandack_transfer_id,
                                                    CANARD_TRANSFER_PRIORITY_MEDIUM,
                                                    CanardResponse,
                                                    &buffer[0],
                                                    (uint16_t)total_size);
    if (resp_res <= 0) {

    }
}

void uavcan_send_task(void *argument)
{
	uint64_t next_1hz_service_at = micros();

	while(1) {
		
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

void uavcan_recv_task(void *argument)
{
	while(1) {
		
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

void processTasksLow(uint64_t timestamp_usec)
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
    nodestatus.mode    = mc.mode;
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

void onTransferReceived(CanardInstance* ins, CanardRxTransfer* transfer)
{
    if ((transfer->transfer_type == CanardTransferTypeBroadcast) && 
        (transfer->data_type_id == UAVCAN_PROTOCOL_NODESTATUS_ID) &&
        (transfer->source_node_id == GIMBAL_ID)) {
        uavcan_protocol_NodeStatus nodestatus;
        uavcan_protocol_NodeStatus_decode(transfer, transfer->payload_len, &nodestatus, &dyn_buf);
        if(nodestatus.mode != UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL) {

        }
    }

    if((transfer->transfer_type == CanardTransferTypeRequest) && 
        (transfer->data_type_id == UAVCAN_PROTOCOL_PARAM_GETSET_ID)) {
        uavcan_protocol_param_GetSetRequest requset;
        uavcan_protocol_param_GetSetResponse param;
        uavcan_protocol_param_GetSetRequest_decode(transfer, transfer->payload_len, &requset, &dyn_buf);
        requset.name.data[requset.name.len] = '\0';

        uint8_t index = find_param_by_name((char *)requset.name.data);
        if(index == 0xff) {
            return;
        }

		char *name = param_get_name(index);
		param.name.data = (uint8_t *)name;
		param.name.len = strlen(name);
		param.index = index;
        if(requset.rw == 20) {
            // read parameter
            float value = param_get_value(index);
            uint8_t type = param_get_type(index);

            if(type == UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE) {
                param.value.real_value = (float)value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
            } else if(type == UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE){
                param.value.boolean_value = (uint8_t)value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE;
            } else {
                param.value.integer_value = (int32_t)value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
            }
        } else if(requset.rw == 23) {
            // write parameter
            if(requset.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE) {
                param_set_value(index, requset.value.real_value);
				param.value.real_value = requset.value.real_value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
            } else if(requset.value.union_tag == UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE){
                param_set_value(index, (float)requset.value.boolean_value);
				param.value.boolean_value = requset.value.boolean_value;
                param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE;
            } else {
                param_set_value(index, (float)requset.value.integer_value);
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
        uavcan_equipment_esc_Command_decode(transfer, transfer->payload_len, &cmd, &dyn_buf);

        switch(cmd.cmd) {
        case MOTOR_CHG_MODE:
            mc.mode = (mc_mode_t)cmd.sub_cmd;
            command_param = cmd.param1;
            break;
        case MOTOR_CTL_MODE:
            mc_ctrl_loop_write(cmd.sub_cmd);
            break;
        default:
            break;
        }
    }
    
    if ((transfer->transfer_type == CanardTransferTypeRequest) && 
        (transfer->data_type_id == UAVCAN_EQUIPMENT_ESC_COMMAND_ID)) {
        uavcan_equipment_esc_Command cmd;
        uavcan_equipment_esc_Command_decode(transfer, transfer->payload_len, &cmd, &dyn_buf);

        switch(cmd.cmd) {
        case MOTOR_CHG_MODE:
            mc.mode = (mc_mode_t)cmd.sub_cmd;
            command_param = cmd.param1;
            break;
        case MOTOR_CTL_MODE:
            mc_ctrl_loop_write(cmd.sub_cmd);
            break;
        default:
            break;
        }
    }
}

bool shouldAcceptTransfer(const CanardInstance* ins,
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
