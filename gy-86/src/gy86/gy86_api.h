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


/**
 * This function will read the sensor data of the selected chip
 */
retcode gy86_Read(gyHandle, const Gy86_ChipType);

/**
 * A getter to the chip handlers
 */
mpuHandle gy86_getMpuChipHandle (gyHandle);
msHandle  gy86_getMsChipHandle (gyHandle);
hmcHandle gy86_getHmcChipHandle (gyHandle);


#endif /* GY86_API_H_ */
