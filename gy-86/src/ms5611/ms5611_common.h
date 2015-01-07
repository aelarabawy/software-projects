/*
 * ms5611_common.h
 *
 *  Created on: Dec 29, 2014
 *      Author: aelarabawy
 */

#ifndef MS5611_COMMON_H_
#define MS5611_COMMON_H_

/**
 * This type will be used as a handle to the ms5611
 */
typedef void* msHandle;

/**
 * Depending on its connection (AD0 Pin) the I2C bus address can be defined as the high/low
 * This enable us to have 2 MPU Chips on the same I2C bus
 */
typedef enum {
	I2C_ADDR_CSB_INVALID = 0,
	I2C_ADDR_CSB_LOW,     //CSB = 0
	I2C_ADDR_CSB_HIGH     //CSB = 1
} Ms5611_I2cAddr;

typedef enum {
	OSR_INVALID = 0,
	OSR_256,
	OSR_512,
	OSR_1024,
	OSR_2048,
	OSR_4096
} Ms5611_Osr;

typedef enum {
	SENSOR_TYPE_PRESSURE = 1,
	SENSOR_TYPE_TEMP = 2,
} Ms5611_SensorType;

#endif /* MS5611_COMMON_H_ */
