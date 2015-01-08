/*
 * hmc5883_api.h
 *
 *  Created on: Dec 22, 2014
 *      Author: aelarabawy
 */

#ifndef HMC5883_API_H_
#define HMC5883_API_H_

#include "hmc5883_common.h"

/***
 * This file contains the API for the HMC-5883 chip
 * This API should expose the features and capabilities of the HMC-5883 chip
 * with avoiding getting in its operation
 * Some features are considered internal things and will be handled inside the lower layers
 *
 **/

/**
 * Basic functions
 */

/**
 * This function should create the needed data structure
 * It does not do any Hardware initialization
 * It can be followed by more settings,
 *  which will not be mapped to the HW until the chip is started
 */
hmcHandle hmc5883_Init (void);

/**
 * This function starts the chip
 */
retcode hmc5883_Start (hmcHandle);

/**
 * For this chip, This function does not do anything
 * It is left for consistency purposes
 */
retcode hmc5883_Stop (hmcHandle);

/**
 * This function will cleanup any data structures associated with the chip
 */
retcode hmc5883_Destroy (hmcHandle);


/**
 * This function will RESET the chip
 * The chip does not have a Reset Command
 * So this simply puts the register in their default values
 */
retcode hmc5883_Reset (hmcHandle);

/**
 * This function will Update the chip configuration to the HW Register
 */
retcode hmc5883_Update (hmcHandle);


/**
 * These functions perform registration of call back function
 * The prototype of the call back should be
 *
 * void callBackFunc ( void *, long);
 */
retcode hmc5883_RegDataRdyCb (hmcHandle, void *, uint32);
retcode hmc5883_RegLockCb (hmcHandle, void *, uint32);


/**
 * These functions will Set/Get the Chip Configurations
 */
retcode hmc5883_SetI2cBusSpeed (hmcHandle, Hmc5883_I2cBusSpeed);
Hmc5883_I2cBusSpeed hmc5883_GetI2cBusSpeed (hmcHandle, const ConfigDataSrc);
retcode hmc5883_ConvertI2cSpeed2String(Hmc5883_I2cBusSpeed, char **);
Hmc5883_I2cBusSpeed hmc5883_ParseBusSpeed(char *);

retcode hmc5883_SetOperMode (hmcHandle, Hmc5883_OperMode);
Hmc5883_OperMode hmc5883_GetOperMode (hmcHandle, const ConfigDataSrc);
retcode hmc5883_ConvertOperMode2String(Hmc5883_OperMode, char **);
Hmc5883_OperMode hmc5883_ParseOperMode(char *);


retcode hmc5883_SetDataOutRate (hmcHandle, Hmc5883_DataOutRate);
Hmc5883_DataOutRate hmc5883_GetDataOutRate (hmcHandle, const ConfigDataSrc);
retcode hmc5883_ConvertDataRate2String(Hmc5883_DataOutRate, char **);
Hmc5883_DataOutRate hmc5883_ParseDataRate (char *);


retcode hmc5883_SetOverSmplRatio (hmcHandle, Hmc5883_OverSmplRatio);
Hmc5883_OverSmplRatio hmc5883_GetOverSmplRatio (hmcHandle, const ConfigDataSrc);
retcode hmc5883_ConvertOsr2String(Hmc5883_OverSmplRatio, char **);
Hmc5883_OverSmplRatio hmc5883_ParseOsr (char *);


retcode hmc5883_SetMeasureMode (hmcHandle, Hmc5883_MeasureMode);
Hmc5883_MeasureMode hmc5883_GetMeasureMode (hmcHandle, const ConfigDataSrc);
retcode hmc5883_ConvertMeasureMode2String(Hmc5883_MeasureMode, char **);
Hmc5883_MeasureMode hmc5883_ParseMeasureMode (char *);


retcode hmc5883_SetMeasureGain (hmcHandle, Hmc5883_MeasureGain);
Hmc5883_MeasureGain hmc5883_GetMeasureGain (hmcHandle, const ConfigDataSrc);
retcode hmc5883_ConvertMeasureGain2String(Hmc5883_MeasureGain, char **);
Hmc5883_MeasureGain hmc5883_ParseMeasureGain (char *);


/**
 * This function is used when running in the Single Measurement mode
 * It is used to trigger a reading,
 */
retcode hmc5883_ReadSensorData (hmcHandle);


/**
 * This function reads the sensor data registers and return the values
 */
retcode hmc5883_GetSensorData (hmcHandle, uint16 *, uint16 *, uint16 *);


/**
 * This function tests the connectivity to the chip
 */
bool hmc5883_TestConnectivity (hmcHandle);


/**
 * Perform Self-Test
 */
bool hmc5883_PerformSelfTest (hmcHandle handle);


#endif /* HMC5883_API_H_ */
