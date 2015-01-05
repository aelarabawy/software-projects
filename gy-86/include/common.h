/*
 * common.h
 *
 *  Created on: Nov 21, 2014
 *      Author: aelarabawy
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "data_types.h"

/**
 * Defines the source of info in a Get function
 */
typedef enum {
	DATA_SRC_CONFIG = 0,
	DATA_SRC_ACTIVE,
	DATA_SRC_HW
} ConfigDataSrc;


#endif /*_COMMON_H_ */
