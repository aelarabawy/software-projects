/*
 * hmc5883.h
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#ifndef HMC5883_H_
#define HMC5883_H_

#include "hmc5883_common.h"

/**
 * Some Useful includes
 */

/**
 * The operating state of the chip
 */
typedef enum {
	HMC5883_STATE_INIT = 0,
	HMC5883_STATE_STARTING,
	HMC5883_STATE_STARTED,
	HMC5883_STATE_STOPPING
} Hmc5883State;



typedef struct {
	//Basic Configurations
	Hmc5883_I2cBusSpeed m_busSpeed;
	Hmc5883_OperMode m_operMode;
	Hmc5883_DataOutRate m_dataRate;
	Hmc5883_OverSmplRatio m_overSmplRatio;
	Hmc5883_MeasureMode m_measureMode;
	Hmc5883_MeasureGain m_measureGain;
	uint8 m_padding[2];
} Hmc5883Config;

typedef struct {

	//Call Back functions and params
	void *m_intDataRdyCb;
	uint32 m_intDataRdyParam;

	void *m_intLockCb;
	uint32 m_intLockParam;

	//Configuration
	Hmc5883Config m_configs;
	Hmc5883Config m_activeConfigs;


	Hmc5883State m_state;

	uint8 m_padding[3];

} Hmc5883;



#endif /* HMC5883_H_ */
