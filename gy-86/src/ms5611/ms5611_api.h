/*
 * ms5611_api.h
 *
 *  Created on: Dec 29, 2014
 *      Author: aelarabawy
 */

#ifndef MS5611_MS5611_API_H_
#define MS5611_MS5611_API_H_

#include "ms5611_common.h"

/***
 * This file contains the API for the MS-5611 chip
 * This API should expose the features and capabilities of the chip
 * with avoiding getting in its operation
 * Some features are considered internal things and will be handled inside the lower layers
 *
 **/

/**
 * Note: Although this chip works with both I2C and SPI interfaces
 * This code targets more the I2C interface
 * Later, I will include the SPI handling as well
 */

/**
 * Basic functions
 */

/**
 * This function should create the needed data structure
 * It does not do any Hardware initialization
 */
msHandle ms5611_Init (void);

/**
 * This function starts the chip if not started
 * This includes reading the PROM data and storing its contents internally
 * And verifying its integrity
 */
retcode ms5611_Start (msHandle);

/**
 * This function does not do anything for now
 * It is in the API for consistency reasons
 */
retcode ms5611_Stop (msHandle);

/**
 * This function will cleanup any data structures associated with the chip
 */
retcode ms5611_Destroy (msHandle);


/**
 * This function will RESET the chip (send the RESET Command)
 */
retcode ms5611_Reset (msHandle);

/**
 * This function will Update the HW
 * Note: Since the chip does not have any HW Register, this function does not do anything
 */
retcode ms5611_Update (msHandle);


/**
 * These functions are Set/Get for the I2CAddr
 * the Set() just specify the high/low address
 * the Get() will return the actual address
 */
retcode ms5611_SetI2cAddr (msHandle, const Ms5611_I2cAddr);
uint8 ms5611_GetI2cAddr (msHandle);


/**
 * Retrieving the Chip PROM Data
 */
uint16 ms5611_GetPressureSens (msHandle);
uint16 ms5611_GetPressureOffset (msHandle);
uint16 ms5611_GetTempCoffPressureSens (msHandle);
uint16 ms5611_GetTempCoffPressureOffset (msHandle);
uint16 ms5611_GetRefTemp (msHandle);
uint16 ms5611_GetTempCoffTemp (msHandle);

/**
 * Set/Get the Over-Sampling Ratio
 */
retcode ms5611_SetPressureOsr (msHandle, Ms5611_Osr);
Ms5611_Osr ms5611_GetPressureOsr (msHandle);

retcode ms5611_SetTempOsr (msHandle, Ms5611_Osr);
Ms5611_Osr ms5611_GetTempOsr (msHandle);

retcode ms5611_ConvertOsr2String(Ms5611_Osr, char **);


/**
 * Perform a Pressure/Temperature Measurement
 */
retcode ms5611_ReadSensorData (msHandle, Ms5611_SensorType);

/**
 * Get a reading
 */
retcode ms5611_GetSensorData (msHandle, uint32);

/**
 * Perform Calculation
 */
retcode ms5611_PerformCalculation (msHandle, uint32, uint32, float *, float *);


#endif /* MS5611_MS5611_API_H_ */
