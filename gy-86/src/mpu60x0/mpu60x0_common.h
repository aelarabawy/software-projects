/*
 * mpu60x0_common.h
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#ifndef MPU60X0_COMMON_H_
#define MPU60X0_COMMON_H_

#include "common.h"

/**
 * These types will be used as  handles to the mpu60x0 and its components
 */
typedef void* mpuHandle;
typedef uint32 slvHandle;

/**
 * Depending on its connection (AD0 Pin) the I2C bus address can be defined as the high/low
 * This enable us to have 2 MPU Chips on the same I2C bus
 */
typedef enum {
	I2C_ADDR_INVALID = 0,
	I2C_ADDR_LOW,         //AD0 = 0
	I2C_ADDR_HIGH         //AD0 = 1
} Mpu60x0_I2cAddr;

/**
 * Clock source for the MPU Chip
 */
typedef enum {
	CLK_SRC_INVALID = 0,
	CLK_SRC_INT_OSC,
	CLK_SRC_GYRO,           //The lower layers will decide on which Gyro internal clock source to use
	CLK_SRC_EXT_LOW,        //32.768 KHz
	CLK_SRC_EXT_HIGH        //19.2 MHz
} Mpu60x0_ClkSrc;


/**
 * LPF Values
 */
typedef enum {
	LPF_INVALID = 0,
	LPF_NONE,
	LPF_184_188,
	LPF_94_98,
	LPF_44_42,
	LPF_21_20,
	LPF_10_10,
	LPF_5_5,
	LPF_RESERVED
} Mpu60x0_Lpf;

/**
 * Full Scale Range for the Accelerometer
 *
 */
typedef enum {
	ACC_FULL_SCALE_INVALID = 0,
	ACC_FULL_SCALE_2G,
	ACC_FULL_SCALE_4G,
	ACC_FULL_SCALE_8G,
	ACC_FULL_SCALE_16G
} Mpu60x0_AccFullScale;


/**
 * Full Scale Range for the Gyroscope
 */
typedef enum {
	GYRO_FULL_SCALE_INVALID = 0,
	GYRO_FULL_SCALE_250,
	GYRO_FULL_SCALE_500,
	GYRO_FULL_SCALE_1000,
	GYRO_FULL_SCALE_2000
} Mpu60x0_GyroFullScale;


/**
 * Different Modes of Reset
 */
typedef enum {
	RESET_MODE_INVALID = 0,
	RESET_MODE_ALL,
	RESET_MODE_SIGNAL_PATH,
	RESET_MODE_REG,
	RESET_MODE_FIFO,
	RESET_AUX_I2C
} Mpu60x0_ResetMode;

/**
 * Interrupt Signal
 */
typedef enum {
	INT_LVL_INVALID = 0,
    INT_LVL_NO_INT,
	INT_LVL_ACTIVE_HIGH,
	INT_LVL_ACTIVE_LOW
} Mpu60x0_IntLvl;


/**
 * Sensor Ids
 * can be used as bit masks
 */
typedef enum {
	SENSOR_NONE = 0,
	SENSOR_ACC_X = 1,
	SENSOR_ACC_Y = 2,
	SENSOR_ACC_Z = 4,
	SENSOR_ACC_ALL = 7,
	SENSOR_GYRO_X = 8,
	SENSOR_GYRO_Y = 16,
	SENSOR_GYRO_Z = 32,
	SENSOR_GYRO_ALL = 56,
	SENSOR_TEMP = 64,
	SENSOR_ALL = 127
} Mpu60x0_SensorId;

typedef enum {
	LP_WAKE_FREQ_INVALID = 0,
	LP_WAKE_FREQ_1,
	LP_WAKE_FREQ_5,
	LP_WAKE_FREQ_20,
	LP_WAKE_FREQ_40
} Mpu60x0_LowPwrWakeCtrlFreq;


/**
 *  Auxiliary I2C Bus Mode
 */
typedef enum {
	AUX_I2C_INVALID = 0,
	AUX_I2C_DISABLE,
	AUX_I2C_BYPASS,
	AUX_I2C_MSTR
} Mpu60x0_AuxI2cMode;

typedef enum {
	AUX_I2C_CLK_INVALID = 0,
	AUX_I2C_CLK_258,
	AUX_I2C_CLK_267,
	AUX_I2C_CLK_276,
	AUX_I2C_CLK_286,
	AUX_I2C_CLK_296,
	AUX_I2C_CLK_308,
	AUX_I2C_CLK_320,
	AUX_I2C_CLK_333,
	AUX_I2C_CLK_348,
	AUX_I2C_CLK_364,
	AUX_I2C_CLK_381,
	AUX_I2C_CLK_400,
	AUX_I2C_CLK_421,
	AUX_I2C_CLK_444,
	AUX_I2C_CLK_471,
	AUX_I2C_CLK_500
} Mpu60x0_AuxI2cClk;

typedef enum {
	ACCESS_OPER_INVALID = 0,
	ACCESS_OPER_READ,
	ACCESS_OPER_WRITE
} Mpu60x0_AccessOper;

typedef struct {
	Mpu60x0_AccessOper  m_slvOper;
	uint8 m_slvI2cAddr;
	uint8 m_slvStartRegNum;
	uint8 m_slvRegCount;

	bool m_isSwap;
	bool m_isEvengroup;
	bool m_isRegNumDisable;
	bool m_intEnable;  //Only Applicable for Slv-4

	bool m_useReducedSampling;
	uint8 m_padding [3];
} Mpu60x0_SlvConfig;


#endif /* MPU60X0_COMMON_H_ */
