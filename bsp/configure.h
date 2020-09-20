#ifndef CONFIGURE_H
#define CONFIGURE_H

/* change it if needed */
#define FIRMWARE_VERSION				1
#define HARDWARE_VERSION				0xF303D201
#define VECT_TAB_OFFSET					0x0000
//#define CAN_LOCAL_ADDR					CAN_ADDR_ROLL
#define CAN_LOCAL_ADDR					CAN_ADDR_PITCH
//#define CAN_LOCAL_ADDR					CAN_ADDR_YAW

// enc
#define ENC_ELEC_STABLE_VAR_TOR			0.004f
#define ENC_MECH_STABLE_VAR_TOR			0.004f

/* fixed */
// version
#define GET_HW_PLATFORM()				((HARDWARE_VERSION & 0xFFFF0000) >> 16)
#define GET_PRJ_VERSION()				((HARDWARE_VERSION & 0xFF00) >> 8)
#define GET_HW_VERSION()				((HARDWARE_VERSION & 0xFF))

// flash
#define FLASH_CONF_ADDR					0x0807F800
#define FLASH_APP_START_ADDR			(0x08000000 + VECT_TAB_OFFSET)
#define FLASH_MAGIC_ADDR_OFFSET			0x800

// enc
#define ENC_SAMPLE_RATE					10000
#define ENC_ANGLE_RESOLUTION			(16384)

#define LPF_ENC_RAW_SAMPLE_RATE			ENC_SAMPLE_RATE
#define LPF_ENC_RAW_CUTOFF_FREQ			800
#define LPF_ENC_SPD_SAMPLE_RATE			ENC_SAMPLE_RATE
#define LPF_ENC_SPD_CUTOFF_FREQ			10

// pid
#define PID_Id_Kp						0.1f
#define PID_Id_Ki						0.0f
#define PID_Id_Kd						0.0f
#define PID_Id_INTEGRAL					0.0f
#define PID_Id_OUTPUT					0.0f
										
#define PID_Iq_Kp						0.1f
#define PID_Iq_Ki						0.0f
#define PID_Iq_Kd						0.0f
#define PID_Iq_INTEGRAL					0.0f
#define PID_Iq_OUTPUT					0.0f
										
#define PID_SPD_Kp						0.04f
#define PID_SPD_Ki						0.0f
#define PID_SPD_Kd						0.001f
#define PID_SPD_INTEGRAL				1.0f
#define PID_SPD_OUTPUT					1.0f
					
#define PID_POS_Kp						5.0f
#define PID_POS_Ki						0.01f
#define PID_POS_Kd						0.0f
#define PID_POS_INTEGRAL				100.0f
#define PID_POS_OUTPUT					1000.0f

#if defined (__legacy)
#define ADC_SAMPLE_POINT_FORWARD	100
#define ADC_SATURATE_BOUND			50

#define CURRENT_SHUNT_RES			20
#define CURRENT_AMP_GAIN			(7.15f+1)
#define FACTOR_CURRENT				(3300.0f * 1000 / 4095.0f / CURRENT_SHUNT_RES / CURRENT_AMP_GAIN)
#define SHUNT_NUM					(3)

// 云台实际给的范围跟测到的电流范围有关系
#define MC_FOC_IQ_MAX_FACT			(8000)
#define MC_FOC_VQ_MAX_FACT			(SQRT3_BY_2*1000)


// larger than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY can be 
// masked INT by freertos (taskENTER_CRITICAL()), otherwise you can
// only use __disable_irq().
#define INT_PRIO_CAN					2
#define INT_PRIO_FOC					1
#define INT_PRIO_ENC_SPI				3
#define INT_PRIO_ENC_PWM				3
#define INT_PRIO_TIM1					3			// test

#define FOC_TIM_SINGLE_FREQ				(40000)
#define FOC_CTRL_FREQ					(FOC_TIM_SINGLE_FREQ/2)
#define PID_CTRL_DT						(1.0f/(FOC_CTRL_FREQ))

#define ADC_REGULAR_PERIOD				(2000000)	// us

#define ENC_SAMPLE_FREQ					(10000)
#define ENC_DET_MECH_VAR_TOR			(0.02f)		// rough
#define ENC_DET_ELEC_VAR_TOR			(0.1f)		// strick

#define FLASH_FW_CONF_START_ADDR		(0x8004000)
#define FLASH_MC_CONF_START_ADDR		(0x8008000)
#define FLASH_FW_START_ADDR				(0x800C000)
#define FLASH_FW_INFO_START_ADDR		(0x800C800)

#if defined(D1_FOC)
#define ID_K_P							(0.1f)
#define ID_K_I							(0.1f)
#define ID_K_D							(0)
#define ID_INTEGRAL						(1000)
#define ID_OUTPUT						(1000)
#define IQ_K_P							(0.1f)
#define IQ_K_I							(0.1f)
#define IQ_K_D							(0)
#define IQ_INTEGRAL						(1000)
#define IQ_OUTPUT						(1000)
#define SPEED_K_P						(5)
#define SPEED_K_I						(0)
#define SPEED_K_D						(0.001f)
#define SPEED_INTEGRAL					(80)
#define SPEED_OUTPUT					(5000)
#define POSITION_K_P					(10)
#define POSITION_K_I					(0.008f)
#define POSITION_K_D					(0)
#define POSITION_INTEGRAL				(10)
#define POSITION_OUTPUT					(2000)
#elif defined(D2_FOC)
#define ID_K_P (0.1f)
#define ID_K_I (0.1f)
#define ID_K_D (0)
#define ID_INTEGRAL (1000)
#define ID_OUTPUT (1000)
#define IQ_K_P (0.1f)
#define IQ_K_I (0.1f)
#define IQ_K_D (0)
#define IQ_INTEGRAL (1000)
#define IQ_OUTPUT (1000)
#define SPEED_K_P (5)
#define SPEED_K_I (0)
#define SPEED_K_D (0.001f)
#define SPEED_INTEGRAL (1000)
#define SPEED_OUTPUT (5000)
#define POSITION_K_P (1)
#define POSITION_K_I (0.0001f)
#define POSITION_K_D (0)
#define POSITION_INTEGRAL (1000)
#define POSITION_OUTPUT (1000)
#endif
#endif
#endif

