/*
 * ms5611.h
 *
 *  Created on: Dec 29, 2014
 *      Author: aelarabawy
 */

#ifndef MS5611_H_
#define MS5611_H_

#include "ms5611_common.h"


/**
 * The operating state of the chip
 */
typedef enum {
	MS5611_STATE_INIT = 0,
	MS5611_STATE_STARTING,
	MS5611_STATE_STOPPING,
	MS5611_STATE_STARTED
} Ms5611State;


typedef struct {

	Ms5611State m_state;
	uint8 m_i2cAddr;

	Ms5611_Osr m_pressureOsr;
	Ms5611_Osr m_tempOsr;

	uint16 m_SENS_T1;   //Pressure Sensitivity
	uint16 m_OFF_T1;    //Pressure Offset
	uint16 m_TCS;       //Temperature Coefficient of Pressure Sensitivity
	uint16 m_TCO;       //Temperature Coefficient of Pressure Offset
	uint16 m_T_REF;     //Reference Temperature
	uint16 m_TEMPSENS;  //Temperature Coefficient of the Temperature
} Ms5611;




#endif /* MS5611_MS5611_H_ */
