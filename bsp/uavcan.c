#include "uavcan.h"
#include "can.h"
#include "ringbuffer.h"
#include <canard.h>
#include <stm32f3xx_hal.h>
#include "cmsis_os.h"
#include "debug.h"
#include "hrt_timer.h"

#include "topics/foc_target.h"

uint32_t CAN1TxMailbox;

static orb_advert_t foc_ctrl_pub = NULL;
struct foc_target_s foc_ctrl;
static uint8_t foc_control_mode = 0;

static struct ringbuffer can1_ring = {0};
#define CAN1_RINGBUFFER_SIZE     512
static uint8_t can1_buffer[CAN1_RINGBUFFER_SIZE];

CanardSTM32AcceptanceFilterConfiguration filters[3];

int STM32_CAN_Filters(void *config, uint32_t num_filter_configs);
uint32_t caculate_filters(uint32_t *id, uint8_t len);

void foc_can_mode(uint8_t mode)
{
    foc_control_mode = mode;
}

void uavcan_init(void)
{
    CAN_FilterTypeDef  sFilterConfig;

    uint32_t motor_id[] = {
        GET_BROADCAST_ID(CANARD_TRANSFER_PRIORITY_LOWEST, UAVCAN_PROTOCOL_NODESTATUS_ID,     GIMBAL_ID),
        GET_BROADCAST_ID(CANARD_TRANSFER_PRIORITY_MEDIUM, UAVCAN_EQUIPMENT_ESC_COMMAND_ID,   GIMBAL_ID),
    };

    uint32_t param_id[] = {
        GET_REQU_RESP_ID(CANARD_TRANSFER_PRIORITY_MEDIUM, UAVCAN_PROTOCOL_PARAM_GETSET_ID, CanardRequest,  LOCAL_MOTOR_ID, GIMBAL_ID),
        GET_REQU_RESP_ID(CANARD_TRANSFER_PRIORITY_MEDIUM, UAVCAN_EQUIPMENT_ESC_COMMAND_ID, CanardRequest,  LOCAL_MOTOR_ID, GIMBAL_ID),
    };

    foc_ctrl_pub = ipc_active(IPC_ID(foc_target), &foc_ctrl);
    
    /*## Configure the CAN Filter ###########################################*/
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Info_Debug("Can Filter init error \n");
    }

    filters[0].id   = BGC_CTRL_ID;
    filters[0].mask = 0x1FFFFFFF;

    filters[1].id   = motor_id[0];
    filters[1].mask = caculate_filters(motor_id, sizeof(motor_id)/4);

    filters[2].id   = param_id[0];
    filters[2].mask = caculate_filters(param_id, sizeof(param_id)/4);
    STM32_CAN_Filters(filters, 3);

	ringbuffer_init(&can1_ring, can1_buffer, CAN1_RINGBUFFER_SIZE);

    /*## Start the CAN peripheral ###########################################*/
    if (HAL_CAN_Start(&hcan) != HAL_OK)
    {
        Info_Debug("Can start error \n");
    }
    
    // RX Interrupt
    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_FULL) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_OVERRUN) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_FULL) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_OVERRUN) != HAL_OK) {
        Error_Handler();
    }

    // ERROR Interrupt
    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_ERROR_WARNING) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_ERROR_PASSIVE) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_BUSOFF) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_LAST_ERROR_CODE) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_ERROR) != HAL_OK) {
        Error_Handler();
    }
}

uint32_t compare(uint32_t a, uint32_t b)
{
    uint32_t res = 0;
    for (uint32_t i=0; i<32; i++) {
        uint8_t temp = !((a >> i) & 1) ^ ((b >> i) & 1);
        res |= temp << i;
    }
    return res;
}

uint32_t caculate_filters(uint32_t *id, uint8_t len)
{
    uint32_t mask = 0x1FFFFFFF;
    for(uint8_t i=0; i<len; i++) {
        mask &= compare(id[i], id[0]);
    }
    return mask;
}

int STM32_CAN_Filters(void *config, uint32_t num_filter_configs)
{
    CanardSTM32AcceptanceFilterConfiguration* filter_configs = (CanardSTM32AcceptanceFilterConfiguration *)config;
    CAN_FilterTypeDef  sFilterConfig;

    if ((filter_configs == NULL) ||
        (num_filter_configs > 14))
    {
        return -1;
    }

    for (uint8_t i = 0; i < num_filter_configs; i++)
    {
        /*
         * Converting the ID and the Mask into the representation that can be chewed by the hardware.
         * If Mask asks us to accept both STDID and EXTID, we need to use EXT mode on the filter,
         * otherwise it will reject all EXTID frames. This logic is not documented in the RM.
         *
         * The logic of the hardware acceptance filters can be described as follows:
         *
         *  accepted = (received_id & filter_mask) == (filter_id & filter_mask)
         *
         * Where:
         *  - accepted      - if true, the frame will be accepted by the filter.
         *  - received_id   - the CAN ID of the received frame, either 11-bit or 29-bit, with extension bits
         *                    marking extended frames, error frames, etc.
         *  - filter_id     - the value of the filter ID register.
         *  - filter_mask   - the value of the filter mask register.
         *
         * There are special bits that are not members of the CAN ID field:
         *  - EFF - set for extended frames (29-bit), cleared for standard frames (11-bit)
         *  - RTR - like above, indicates Remote Transmission Request frames.
         *
         * The following truth table summarizes the logic (where: FM - filter mask, FID - filter ID, RID - received
         * frame ID, A - true if accepted, X - any state):
         *
         *  FM  FID RID A
         *  0   X   X   1
         *  1   0   0   1
         *  1   1   0   0
         *  1   0   1   0
         *  1   1   1   1
         *
         * One would expect that for the purposes of hardware filtering, the special bits should be treated
         * in the same way as the real ID bits. However, this is not the case with bxCAN. The following truth
         * table has been determined empirically (this behavior was not documented as of 2017):
         *
         *  FM  FID RID A
         *  0   0   0   1
         *  0   0   1   0       <-- frame rejected!
         *  0   1   X   1
         *  1   0   0   1
         *  1   1   0   0
         *  1   0   1   0
         *  1   1   1   1
         */
        uint32_t id   = 0;
        uint32_t mask = 0;

        CanardSTM32AcceptanceFilterConfiguration* cfg = filter_configs + i;

        if ((cfg->id & CANARD_CAN_FRAME_EFF) || !(cfg->mask & CANARD_CAN_FRAME_EFF))
        {
            id   = (cfg->id   & CANARD_CAN_EXT_ID_MASK) << 3U;
            mask = (cfg->mask & CANARD_CAN_EXT_ID_MASK) << 3U;
            id |= CAN_ID_EXT;
        }
        else
        {
            id   = (cfg->id   & CANARD_CAN_STD_ID_MASK) << 21U;
            mask = (cfg->mask & CANARD_CAN_STD_ID_MASK) << 21U;
        }

        sFilterConfig.FilterBank = i;
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh = (id >> 16u) & 0xFFFF;
        sFilterConfig.FilterIdLow = id & 0xFFFF;
        sFilterConfig.FilterMaskIdHigh = (mask >> 16u) & 0xFFFF;
        sFilterConfig.FilterMaskIdLow = mask & 0xFFFF;
        if((i/2) == 0) {
            sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
        } else {
            sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
        }
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.SlaveStartFilterBank = 14;

        if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
        {
            /* Filter configuration Error */
            return 0;
        }
    }
    return 0;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CanardCANFrame rxframe;
    CAN_RxHeaderTypeDef p_rxheader;

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &p_rxheader, rxframe.data) != HAL_OK) {
				return;
    }

    rxframe.id = 0;

    int16_t value[4];
	if(p_rxheader.IDE == CAN_ID_STD) {
		if(p_rxheader.StdId == BGC_CTRL_ID) {
			// control motor
            memcpy(value, &rxframe.data[0], sizeof(value));
            uint8_t index = (uint8_t)(sqrtf(LOCAL_MOTOR_ID) - 1);
  
            foc_ctrl.timestamp = micros();
            foc_ctrl.iq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.id_target = 0.0f;
            foc_ctrl.vq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.vd_target = 0.0f;
            foc_ctrl.ctrl_mode = value[3];

            if(!(foc_control_mode & MC_CTRL_DUTY) && !(foc_control_mode & MC_CTRL_OVERRIDE)) {
                ipc_push(IPC_ID(foc_target), foc_ctrl_pub, &foc_ctrl);
            }
			return;
        }
		rxframe.id = (CANARD_CAN_STD_ID_MASK & p_rxheader.StdId);
    } else {
		if(p_rxheader.ExtId == BGC_CTRL_ID) {
			// control motor
            memcpy(value, &rxframe.data[0], sizeof(value));
            uint8_t index = (uint8_t)(sqrtf(LOCAL_MOTOR_ID) - 1);
            foc_ctrl.timestamp = micros();
            foc_ctrl.iq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.id_target = 0.0f;
            foc_ctrl.vq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.vd_target = 0.0f;
            foc_ctrl.ctrl_mode = value[3];

            if(!(foc_control_mode & MC_CTRL_DUTY) && !(foc_control_mode & MC_CTRL_OVERRIDE)) {
                ipc_push(IPC_ID(foc_target), foc_ctrl_pub, &foc_ctrl);
            }
			return;
		}
        rxframe.id = (CANARD_CAN_EXT_ID_MASK & p_rxheader.ExtId) | CANARD_CAN_FRAME_EFF;
    }

    if(p_rxheader.RTR == CAN_RTR_REMOTE) {
        rxframe.id |= CANARD_CAN_FRAME_RTR;
    }

    rxframe.data_len = p_rxheader.DLC;

    // detect if the message is bgc control
    if(hcan->Instance == CAN && can1_ring.buffer_ptr != NULL) {
        ringbuffer_put(&can1_ring, (const uint8_t *)&rxframe, sizeof(rxframe));
    }
}

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{

}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CanardCANFrame rxframe;
    CAN_RxHeaderTypeDef p_rxheader;

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &p_rxheader, rxframe.data) != HAL_OK) {
        return;
    }

    rxframe.id = 0;

    int16_t value[4];
    if(p_rxheader.IDE == CAN_ID_STD) {
		if(p_rxheader.StdId == BGC_CTRL_ID) {
			// control motor
            memcpy(value, &rxframe.data[0], sizeof(value));
            uint8_t index = (uint8_t)(sqrtf(LOCAL_MOTOR_ID) - 1);
  
            foc_ctrl.timestamp = micros();
            foc_ctrl.iq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.id_target = 0.0f;
            foc_ctrl.vq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.vd_target = 0.0f;
            foc_ctrl.ctrl_mode = value[3];

            if(!(foc_control_mode & MC_CTRL_DUTY) && !(foc_control_mode & MC_CTRL_OVERRIDE)) {
                ipc_push(IPC_ID(foc_target), foc_ctrl_pub, &foc_ctrl);
            }
			return;
        }
		rxframe.id = (CANARD_CAN_STD_ID_MASK & p_rxheader.StdId);
    } else {
		if(p_rxheader.ExtId == BGC_CTRL_ID) {
			// control motor
            memcpy(value, &rxframe.data[0], sizeof(value));
            uint8_t index = (uint8_t)(sqrtf(LOCAL_MOTOR_ID) - 1);

            foc_ctrl.timestamp = micros();
            foc_ctrl.iq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.id_target = 0.0f;
            foc_ctrl.vq_target = (float)(value[index]/32768.0f)*2.0f;
            foc_ctrl.vd_target = 0.0f;
            foc_ctrl.ctrl_mode = value[3];

            if(!(foc_control_mode & MC_CTRL_DUTY) && !(foc_control_mode & MC_CTRL_OVERRIDE)) {
                ipc_push(IPC_ID(foc_target), foc_ctrl_pub, &foc_ctrl);
            }
			return;
		} 
        rxframe.id = (CANARD_CAN_EXT_ID_MASK & p_rxheader.ExtId) | CANARD_CAN_FRAME_EFF;
    }

    if(p_rxheader.RTR == CAN_RTR_REMOTE) {
        rxframe.id |= CANARD_CAN_FRAME_RTR;
    }

    rxframe.data_len = p_rxheader.DLC;

    if(hcan->Instance == CAN && can1_ring.buffer_ptr != NULL) {
        ringbuffer_put(&can1_ring, (const uint8_t *)&rxframe, sizeof(rxframe));
    } 
}

void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)
{

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{

}

int CAN_Receive(void* frame)
{
    CanardCANFrame* out_frame = (CanardCANFrame*)frame;

    if (out_frame == NULL) {
        return -1;
    }
    return ringbuffer_get(&can1_ring, (uint8_t *)out_frame, sizeof(*out_frame));
}

int CAN_Transmit(const void* txframe)
{
    CanardCANFrame* frame = (CanardCANFrame*)txframe;
    CAN_TxHeaderTypeDef p_txheader;
    
    if (frame == NULL) {
        return -1;
    }

    if (frame->id & CANARD_CAN_FRAME_ERR) {
        return -1;
    }

    if (frame->id & CANARD_CAN_FRAME_EFF) {
        p_txheader.ExtId = frame->id;
        p_txheader.IDE   = CAN_ID_EXT;
        p_txheader.RTR   = CAN_RTR_DATA;
    } else {
        p_txheader.StdId = frame->id;
        p_txheader.IDE   = CAN_ID_STD;
        p_txheader.RTR   = CAN_RTR_DATA;
    }

    if (frame->id & CANARD_CAN_FRAME_RTR) {
        p_txheader.RTR   = CAN_RTR_REMOTE;
    }

    p_txheader.DLC = frame->data_len;
    p_txheader.TransmitGlobalTime = DISABLE;
    
    if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) {
        return 0;
    }

    if(HAL_CAN_AddTxMessage(&hcan, &p_txheader, frame->data, &CAN1TxMailbox) != HAL_OK) {
        return 0;
    }
    
    return frame->data_len;
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
    
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
    
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
    
}
