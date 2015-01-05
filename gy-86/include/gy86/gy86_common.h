/*
 * gy86_common.h
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#ifndef GY86_COMMON_H_
#define GY86_COMMON_H_

#include "common.h"

/**
 * This type will be used as a handle to the GY-86
 */
typedef void* gyHandle;

/**
 * Chip Type
 */
typedef enum {
	CHIP_TYPE_INVALID = 0,
	CHIP_TYPE_MPU60X0 = 1,
	CHIP_TYPE_MS5611 = 2,
	CHIP_TYPE_HMC5883 = 4,
	CHIP_TYPE_ALL = 7
} Gy86_ChipType;



#endif /* GY86_COMMON_H_ */
