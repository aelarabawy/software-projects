/*
 * hmc5883_common.h
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#ifndef HMC5883_COMMON_H_
#define HMC5883_COMMON_H_

#include "common.h"

/**
 * This type will be used as handle to the hmc5883
 */
typedef void* hmcHandle;

/**
 * I2C Bus Clock Speed
 */
typedef enum {
	I2C_BUS_SPEED_INVALID = 0,
	I2C_BUS_SPEED_STD,   //Default
	I2C_BUS_SPEED_HIGH
} Hmc5883_I2cBusSpeed;

/**
 * Operating Mode
 */
typedef enum {
	OPER_MODE_INVALID = 0,
	OPER_MODE_IDLE,
	OPER_MODE_SINGLE,   //Default
	OPER_MODE_CONT
} Hmc5883_OperMode;

/**
 * Data Output Rate
 */
typedef enum {
	DATA_OUT_RATE_INVALID = 0,
	DATA_OUT_RATE_0_75,
	DATA_OUT_RATE_1_5,
	DATA_OUT_RATE_3,
	DATA_OUT_RATE_7_5,
	DATA_OUT_RATE_15,   //Default
	DATA_OUT_RATE_30,
	DATA_OUT_RATE_75
} Hmc5883_DataOutRate;

typedef enum {
	OVER_SMPL_RATIO_INVALID = 0,
	OVER_SMPL_RATIO_1,  //Default
	OVER_SMPL_RATIO_2,
	OVER_SMPL_RATIO_4,
	OVER_SMPL_RATIO_8
} Hmc5883_OverSmplRatio;

typedef enum {
	MEASURE_MODE_INVALID = 0,
	MEASURE_MODE_NORMAL, //Default
	MEASURE_MODE_POS_BIAS,
	MEASURE_MODE_NEG_BIAS
} Hmc5883_MeasureMode;

typedef enum {
	MEASURE_GAIN_INVALID = 0,
	MEASURE_GAIN_1370,
	MEASURE_GAIN_1090,      //Default
	MEASURE_GAIN_820,
	MEASURE_GAIN_660,
	MEASURE_GAIN_440,
	MEASURE_GAIN_390,
	MEASURE_GAIN_330,
	MEASURE_GAIN_230
}Hmc5883_MeasureGain ;

#endif /* HMC5883_COMMON_H_ */
