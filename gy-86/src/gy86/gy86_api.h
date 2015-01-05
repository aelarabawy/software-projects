/*
 * gy86_api.h
 *
 *  Created on: Dec 22, 2014
 *      Author: aelarabawy
 */

#ifndef GY86_API_H_
#define GY86_API_H_

#include "gy86_common.h"
#include "mpu60x0_api.h"
#include "ms5611_api.h"
#include "hmc5883_api.h"

/***
 * This file contains the API for the GY-86 Module
 * This module contains the chips MPU-6050, MS-5611, and the HMC-5883L
 * This API should call the APIs of the corresponding chips
 *
 **/

/**
 * Basic functions
 */

/**
 * This function should initialize all chips in the GY-86 module
 * It can be followed by more settings,
 *  which will not be mapped to the HW until the chips are started
 */
gyHandle gy86_Init (const Gy86_ChipType);


/**
 * This function starts the specified chips in the module
 * This API can be called multiple times with different chip every time
 */
retcode gy86_Start (gyHandle, const Gy86_ChipType);

/**
 * This function stops the specified chips in the module
 * This API can be called multiple times with different chip every time
 */
retcode gy86_Stop (gyHandle, const Gy86_ChipType);


/**
 * This function will cleanup any data structures associated with the chips and the module
 * This function should be called only once
 */
retcode gy86_Destroy (gyHandle);


/**
 * This function will RESET the GY-86 module or part of it
 * In case it is
 */
retcode gy86_Reset (gyHandle, const Gy86_ChipType, const Mpu60x0_ResetMode);

/**
 * This function will Update the chip configuration to the HW Register
 */
retcode gy86_Update (gyHandle, const Gy86_ChipType);


#if 0


/**
 * These functions perform registration of call back functions
 * The prototype of the call back should be
 *
 * void callBackFunc ( void *, long);
 */
retcode mpu60x0_RegDataRdyCb (mpuHandle, void *, uint32);
retcode mpu60x0_RegFifoOvrflowCb (mpuHandle, void *, uint32);
retcode mpu60x0_RegMotDetCb (mpuHandle, void *, uint32);
retcode mpu60x0_RegFsynchCb (mpuHandle, void *, uint32);
retcode mpu60x0_RegAuxI2cCb (mpuHandle, void *, uint32);


/**
 * These functions are Set/Get for the I2CAddr
 * the Set() just specify the high/low address
 * the Get() will return the actual address
 */
retcode mpu60x0_SetI2cAddr (mpuHandle, const Mpu60x0_I2cAddr);
uint8 mpu60x0_GetI2cAddr (mpuHandle);

/**
 * These functions will Set/Get the clock source of the chip
 */
retcode mpu60x0_SetClkSrc (mpuHandle, const Mpu60x0_ClkSrc);
Mpu60x0_ClkSrc mpu60x0_GetClkSrc (mpuHandle , const ConfigDataSrc);


/**
 * These functions will Set/Get the Sampling rate of the chip
 * Note that, the sampling rate may not be achieved exactly through the set function
 * so the return value states the exact value that was achieved
 *
 */
uint32 mpu60x0_SetSamplingRate (mpuHandle,uint32, Mpu60x0_Lpf);
uint32 mpu60x0_GetSamplingRate (mpuHandle, const ConfigDataSrc);
Mpu60x0_Lpf mpu60x0_GetLpf (mpuHandle, const ConfigDataSrc);


/**
 * These functions for Set/Get the Full Scale Range for the Accelerometer
 */
retcode mpu60x0_SetAccFullScale (mpuHandle , const Mpu60x0_AccFullScale);
Mpu60x0_AccFullScale mpu60x0_GetAccFullScale (mpuHandle, const ConfigDataSrc);

/**
 * These functions for Set/Get the Full Scale Range for the Gyroscope
 */
retcode mpu60x0_SetGyroFullScale (mpuHandle, const Mpu60x0_GyroFullScale);
Mpu60x0_GyroFullScale mpu60x0_GetGyroFullScale (mpuHandle, const ConfigDataSrc);

/**
 * Enable/Disable Using FIFO
 */
retcode mpu60x0_SetFifoUsage (mpuHandle, const bool);
bool mpu60x0_GetFifoUsage (mpuHandle, const ConfigDataSrc);

/**
 * Set/Get Motion Detection Threshold
 */
retcode mpu60x0_SetMotDetThr (mpuHandle, bool, uint8);
uint8 mpu60x0_GetMotDetThr (mpuHandle, const ConfigDataSrc);

/**
 * Enable/Disable Sensors
 * Use the enum Mpu60x0_SensorId as bit mask
 */
retcode mupu60x0_SetActiveSensors (mpuHandle, const uint8);
uint8 mpu60x0_GetActiveSensors (mpuHandle, const ConfigDataSrc);

/**
 * This function configures the Interrupt signal
 */
retcode mpu60x0_ConfigIntSignal (mpuHandle, Mpu60x0_IntSignalConfig *);

/**
 * Set/Get Auxiliary I2C Bus mode
 */
retcode mpu60x0_SetAuxI2c (mpuHandle, const Mpu60x0_AuxI2cMode);
Mpu60x0_AuxI2cMode mpu60x0_GetAuxI2c (mpuHandle, const ConfigDataSrc);


/**
 *  Set the Auxiliary I2C Bus Clock Speed
 */
retcode mpu60x0_SetAuxI2cClk (mpuHandle, const Mpu60x0_AuxI2cClk);
Mpu60x0_AuxI2cClk mpu60x0_GetAuxI2cClk (mpuHandle, const ConfigDataSrc);

/**
 *  Set the Auxiliary I2C Reduced Sampling Factor
 */
retcode mpu60x0_SetAuxI2cReducedSamplingFactor (mpuHandle, const uint8 );
uint8 mpu60x0_GetAuxI2cReducedSamplingFactor (mpuHandle, const ConfigDataSrc);


/**
 * FSYNC Signal
 */
retcode mpu60x0_SetFsync (mpuHandle, const Mpu60x0_SensorId, const Mpu60x0_IntLvl);

/**
 * Adding/Removing Slaves
 * Parameters:
 *   Slave Number
 *   Slave Configs
 */
slvHandle mpu60x0_AddSlv (mpuHandle, const uint8, Mpu60x0_SlvConfig *);
retcode mpu60x0_RemSlv (mpuHandle, slvHandle);


/**
 * This function performs a read for all sensor data in a synchronous manner
 * This guarantees that all sensor data are read in one shot
 * Reading can happen from either the sensor registers or
 * The Fifo depending on whether the fifo is Enabled or not
 *
 * The function puts the data in an internal data array,
 * The caller can request any specific sensor data in separate function calls
 */
retcode mpu60x0_ReadSensorData (mpuHandle);


/**
 * Get functions for Accelerometer/Gyroscope/Temperature data
 * The returned data will be scaled based on the scaling factor
 * This does not do any read from the Hardware,
 * instead, it just parses the internal data array
 * You need to call mpu60x0_ReadSensorData() first to fill the array
 * if fresh sensor data are required
 */
retcode mpu60x0_GetAccData (mpuHandle, float *, float *, float *);
retcode mpu60x0_GetGyroData (mpuHandle, float *, float *, float *);
retcode mpu60x0_GetTempData (mpuHandle, float *);

uint8 mpu60x0_GetSlvRegData (mpuHandle, slvHandle, uint8 *);
retcode mpu60x0_SetSlvRegData (mpuHandle, slvHandle, const uint8);

/**
 * Cycle for Low Power Accelerometer only
 */
retcode mpu60x0_SetCycleMode(mpuHandle, bool, Mpu60x0_LowPwrWakeCtrlFreq, uint8);

/**
 * This function tests the connectivity to the chip
 */
bool mpu60x0_TestConnectivity (mpuHandle);


/**
 * Perform Self-Test
 */
bool mpu60x0_PerformSelfTest (mpuHandle handle,
		                      const uint8 sensors);

#endif
#endif /* GY86_API_H_ */
