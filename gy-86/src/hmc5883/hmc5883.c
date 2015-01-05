/*
 * hmc5883.c
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#include <math.h>
#include <unistd.h>
#include "logger.h"
#include "hmc5883_regs.h"
#include "hmc5883.h"


//Internal Functions Prototypes
static retcode setRegDefault (Hmc5883 *, Hmc5883Config *);
static retcode updateChip (Hmc5883 *);
static retcode startChip (Hmc5883 *);
static retcode stopChip (Hmc5883 *);
static retcode resetChip (Hmc5883 *);
static retcode resetHmc5883Data (Hmc5883 *);

static retcode setReadPointer(Hmc5883 *, const uint8);

//Read/Write to HW
retcode writeBytes (Hmc5883 *, uint8 *, const uint8);
retcode readBytes (Hmc5883 *, uint8 *, const uint8);

//Get from HW Registers
static Hmc5883_I2cBusSpeed getBusSpeed (Hmc5883 *);
static Hmc5883_OperMode getOperMode (Hmc5883 *);
static Hmc5883_DataOutRate getDataOutRate (Hmc5883 *);
static Hmc5883_OverSmplRatio getOverSmplRatio (Hmc5883 *);
static Hmc5883_MeasureMode getMeasureMode (Hmc5883 *);
static Hmc5883_MeasureGain getMeasureGain (Hmc5883 *);

//Updating Registers
static retcode updateReg_CONFIG_A (Hmc5883 *);
static retcode updateReg_CONFIG_B (Hmc5883 *);
static retcode updateReg_MODE (Hmc5883 *);
//////////////////////////////////////////////

/**
 * Basic Functions
 */

/**
 * This function should create the needed data structure
 * It does not do any Hardware initialization
 * It can be followed by more settings,
 *  which will not be mapped to the HW until the chip is started
 */
hmcHandle hmc5883_Init (void) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)malloc(sizeof(Hmc5883));
	if (!hmc) {
		ALLOC_FAIL("hmc");
		goto END;
	}
	memset(hmc, 0x00, sizeof(Hmc5883));

	hmc->m_state = HMC5883_STATE_INIT;

	//Set Initial Values for configurations (Based on chip default values)
	setRegDefault(hmc, &hmc->m_configs);
	setRegDefault(hmc, &hmc->m_activeConfigs);

	resetHmc5883Data(hmc);
END:
	EXIT();
	return (hmcHandle)hmc;
}


/**
 * This function starts the chip
 */
retcode hmc5883_Start (hmcHandle handle) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	if (hmc->m_state != HMC5883_STATE_INIT) {
		ERROR("HMC-5883 Already Started .... Exiting");
		retVal = -1;
		goto END;
	}

	hmc->m_state = HMC5883_STATE_STARTING;
	retVal = startChip(hmc);
	if (retVal) {
		ERROR("Can not start Chip");
		goto END;
	}

	hmc->m_state = HMC5883_STATE_STARTED;

END:
	EXIT();
	return retVal;
}

/**
 * This function is supposed to put the chip in sleep mode
 * For this chip, it does not do anything
 * It is left for consistency purposes
 */
retcode hmc5883_Stop (hmcHandle handle) {

	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	if (hmc->m_state != HMC5883_STATE_STARTED) {
		ERROR("HMC-5883 Not Started .... Exiting");
		retVal = -1;
		goto END;
	}

	hmc->m_state = HMC5883_STATE_STOPPING;
	retVal = stopChip(hmc);
	if (retVal) {
		ERROR("Can not stop Chip");
		goto END;
	}

	hmc->m_state = HMC5883_STATE_INIT;

END:
	EXIT();
	return retVal;
}

/**
 * This function will cleanup any data structures associated with the chip
 */
retcode hmc5883_Destroy (hmcHandle handle) {

	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	if (hmc->m_state != HMC5883_STATE_INIT) {
		ERROR("HMC-5883 Started ....Need to stop it first ...Exiting");
		retVal = -1;
		goto END;
	}


	//Free the object
	free(hmc);

END:
	EXIT();
	return retVal;
}


/**
 * This function will RESET the chip
 * The chip does not have a Reset Command
 * So this simply puts the register in their default values
 */
retcode hmc5883_Reset (hmcHandle handle) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	//We need also to reset our internal data structures and state
	setRegDefault(hmc, &hmc->m_configs);
	setRegDefault(hmc, &hmc->m_activeConfigs);

	//Init other info to defaults
	resetHmc5883Data(hmc);

	//This should bring the chip to power up condition
	resetChip(hmc);


END:
	EXIT();
	return retVal;
}


/**
 * This function will Update the chip configuration to the HW Register
 */
retcode hmc5883_Update (hmcHandle handle) {

	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	retVal = updateChip(hmc);
	if (retVal) {
		ERROR("Can not update Chip");
		goto END;
	}

END:
	EXIT();
	return retVal;
}




/**
 * These functions perform registration of call back function
 * The prototype of the call back should be
 *
 * void callBackFunc ( void *, long);
 */
retcode hmc5883_RegDataRdyCb (hmcHandle handle,
  		                      void *cb,
						      uint32 param) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}


	hmc->m_intDataRdyCb = cb;
	hmc->m_intDataRdyParam = param;

END:
	EXIT();
	return retVal;
}

retcode hmc5883_RegLockCb (hmcHandle handle,
  		                   void *cb,
						   uint32 param) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}


	hmc->m_intLockCb = cb;
	hmc->m_intLockParam = param;

END:
	EXIT();
	return retVal;
}


/**
 * These functions will Set/Get the Chip Configurations
 */

/**
 * These functions will Set/Get the I2C Bus Speed
 */
retcode hmc5883_SetI2cBusSpeed (hmcHandle handle,
		                        Hmc5883_I2cBusSpeed speed) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	switch (speed) {
	case I2C_BUS_SPEED_STD:
	case I2C_BUS_SPEED_HIGH:
		hmc->m_configs.m_busSpeed = speed;
		break;

	default:
		ERROR("Invalid Value for I2C Bus Speed");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Hmc5883_I2cBusSpeed hmc5883_GetI2cBusSpeed (hmcHandle handle,
		                                    const ConfigDataSrc dataSrc) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	Hmc5883_I2cBusSpeed speed;

	if (!hmc) {
		NULL_POINTER("hmc");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		speed = hmc->m_configs.m_busSpeed;
		break;

	case DATA_SRC_ACTIVE:
		speed = hmc->m_activeConfigs.m_busSpeed;
		break;

	case DATA_SRC_HW:
		speed = getBusSpeed(hmc);
		break;
	}

END:
	EXIT();
	return speed;
}

retcode hmc5883_SetOperMode (hmcHandle handle,
		                     Hmc5883_OperMode mode) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	switch (mode) {
	case OPER_MODE_IDLE:
	case OPER_MODE_SINGLE:
	case OPER_MODE_CONT:
		hmc->m_configs.m_operMode = mode;
		break;

	default:
		ERROR("Invalid Value for Operating Mode");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Hmc5883_OperMode hmc5883_GetOperMode (hmcHandle handle,
		                              const ConfigDataSrc dataSrc) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	Hmc5883_OperMode mode;

	if (!hmc) {
		NULL_POINTER("hmc");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		mode = hmc->m_configs.m_operMode;
		break;

	case DATA_SRC_ACTIVE:
		mode = hmc->m_activeConfigs.m_operMode;
		break;

	case DATA_SRC_HW:
		mode = getOperMode(hmc);
		break;
	}

END:
	EXIT();
	return mode;
}

retcode hmc5883_SetDataOutRate (hmcHandle handle,
		                        Hmc5883_DataOutRate rate) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	switch (rate) {
	case DATA_OUT_RATE_0_75:
	case DATA_OUT_RATE_1_5:
	case DATA_OUT_RATE_3:
	case DATA_OUT_RATE_7_5:
	case DATA_OUT_RATE_15:
	case DATA_OUT_RATE_30:
	case DATA_OUT_RATE_75:
		hmc->m_configs.m_dataRate = rate;
		break;

	default:
		ERROR("Invalid Value for Data Rate");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Hmc5883_DataOutRate hmc5883_GetDataOutRate (hmcHandle handle,
		                                    const ConfigDataSrc dataSrc) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	Hmc5883_DataOutRate rate;

	if (!hmc) {
		NULL_POINTER("hmc");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		rate = hmc->m_configs.m_dataRate;
		break;

	case DATA_SRC_ACTIVE:
		rate = hmc->m_activeConfigs.m_dataRate;
		break;

	case DATA_SRC_HW:
		rate = getDataOutRate(hmc);
		break;
	}

END:
	EXIT();
	return rate;
}

retcode hmc5883_SetOverSmplRatio (hmcHandle handle,
		                          Hmc5883_OverSmplRatio ratio) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	switch (ratio) {
	case OVER_SMPL_RATIO_1:
	case OVER_SMPL_RATIO_2:
	case OVER_SMPL_RATIO_4:
	case OVER_SMPL_RATIO_8:
		hmc->m_configs.m_overSmplRatio = ratio;
		break;

	default:
		ERROR("Invalid Value for Over Sampling Ratio");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Hmc5883_OverSmplRatio hmc5883_GetOverSmplRatio (hmcHandle handle,
		                                        const ConfigDataSrc dataSrc) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	Hmc5883_OverSmplRatio ratio;

	if (!hmc) {
		NULL_POINTER("hmc");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		ratio = hmc->m_configs.m_overSmplRatio;
		break;

	case DATA_SRC_ACTIVE:
		ratio = hmc->m_activeConfigs.m_overSmplRatio;
		break;

	case DATA_SRC_HW:
		ratio = getOverSmplRatio(hmc);
		break;
	}

END:
	EXIT();
	return ratio;
}

retcode hmc5883_SetMeasureMode (hmcHandle handle,
		                        Hmc5883_MeasureMode mode) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	switch (mode) {
	case MEASURE_MODE_NORMAL:
	case MEASURE_MODE_POS_BIAS:
	case MEASURE_MODE_NEG_BIAS:
		hmc->m_configs.m_measureMode = mode;
		break;

	default:
		ERROR("Invalid Value for Measurement Mode");
		retVal = -1;
		goto END;
		break;
	}


END:
	EXIT();
	return retVal;
}

Hmc5883_MeasureMode hmc5883_GetMeasureMode (hmcHandle handle,
		                                    const ConfigDataSrc dataSrc) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	Hmc5883_MeasureMode mode;

	if (!hmc) {
		NULL_POINTER("hmc");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		mode = hmc->m_configs.m_measureMode;
		break;

	case DATA_SRC_ACTIVE:
		mode = hmc->m_activeConfigs.m_measureMode;
		break;

	case DATA_SRC_HW:
		mode = getMeasureMode(hmc);
		break;
	}

END:
	EXIT();
	return mode;
}

retcode hmc5883_SetMeasureGain (hmcHandle handle,
		                        Hmc5883_MeasureGain gain) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	switch (gain) {
	case MEASURE_GAIN_1370:
	case MEASURE_GAIN_1090:
	case MEASURE_GAIN_820:
	case MEASURE_GAIN_660:
	case MEASURE_GAIN_440:
	case MEASURE_GAIN_390:
	case MEASURE_GAIN_330:
	case MEASURE_GAIN_230:
		hmc->m_configs.m_measureGain = gain;
		break;

	default:
		ERROR("Invalid Value for Measurement Gain");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}


Hmc5883_MeasureGain hmc5883_GetMeasureGain (hmcHandle handle,
		                                    const ConfigDataSrc dataSrc) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	Hmc5883_MeasureGain gain;

	if (!hmc) {
		NULL_POINTER("hmc");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		gain = hmc->m_configs.m_measureGain;
		break;

	case DATA_SRC_ACTIVE:
		gain = hmc->m_activeConfigs.m_measureGain;
		break;

	case DATA_SRC_HW:
		gain = getMeasureGain(hmc);
		break;
	}

END:
	EXIT();
	return gain;
}



/**
 * This function is used when running in the Single Measurement mode
 * It is used to trigger a reading,
 */
retcode hmc5883_ReadSensorData (hmcHandle handle) {
	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	//This is done via setting the mode to a Single Read
	hmc->m_configs.m_operMode = OPER_MODE_SINGLE;

	retVal = updateReg_MODE(hmc);
	if (retVal) {
		ERROR("Can not update the MODE Register to perform a single read");
		goto END;
	}

END:
	EXIT();
	return retVal;
}


/**
 * Get the Sensor data
 */
retcode hmc5883_GetSensorData (hmcHandle handle,
		                       uint16 *x, uint16 *y, uint16 *z) {

	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;
	uint8 regVal[8];

	if (!hmc) {
		NULL_POINTER("hmc");
		retVal = -1;
		goto END;
	}

	//Set the Pointer to the data Registers
	retVal = setReadPointer(hmc, HMC5883_REG_DATA_OUT_X_H);
	if (retVal) {
		ERROR("Failed Setting the Pointer to the Data Registers");
		goto END;
	}

	//Now read the data
	retVal = readBytes(hmc, regVal, 6);
	if (retVal) {
		ERROR("Failed to read the data Registers");
		goto END;
	}

	*x = (regVal[0] << 8) | regVal[1];
	*y = (regVal[4] << 8) | regVal[5];
	*z = (regVal[2] << 8) | regVal[3];

END:
	EXIT();
	return retVal;
}


/**
 * This function tests the connectivity to the chip
 */
bool hmc5883_TestConnectivity (hmcHandle handle) {

	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	retcode retVal = 0;
	uint8 regVal[4];
	bool result = TRUE;

	if (!hmc) {
		NULL_POINTER("hmc");
		result = FALSE;
		goto END;
	}

	//Set the Pointer to the Identity Registers
	retVal = setReadPointer(hmc, HMC5883_REG_IDENT_A);
	if (retVal) {
		ERROR("Failed Setting the Pointer to the Identity Registers");
		goto END;
	}

	//Now read the data
	retVal = readBytes(hmc, regVal, 3);
	if (retVal) {
		ERROR("Failed Reading the Identity Registers");
		goto END;
	}

	if ((regVal[0] != HMC5883_IDENTITY_REG_VAL_A) ||
			(regVal[1] != HMC5883_IDENTITY_REG_VAL_B) ||
			(regVal[2] != HMC5883_IDENTITY_REG_VAL_C)) {
		PROGRESS("Failed the Connectivity Test");
		INFO("Expected in Reg-A  %d  Received %d", HMC5883_IDENTITY_REG_VAL_A, regVal[0]);
		INFO("Expected in Reg-B  %d  Received %d", HMC5883_IDENTITY_REG_VAL_B, regVal[1]);
		INFO("Expected in Reg-C  %d  Received %d", HMC5883_IDENTITY_REG_VAL_C, regVal[2]);

		result = FALSE;
	} else {
		PROGRESS("PASSED The Connectivity Test");
	}

END:
	EXIT();
	return result;
}

/**
 * Perform Self-Test
 */
bool hmc5883_PerformSelfTest (hmcHandle handle) {

	ENTER();

	Hmc5883 *hmc = (Hmc5883 *)handle;
	bool result = TRUE;

	if (!hmc) {
		NULL_POINTER("hmc");
		goto END;
	}

    //TODO Need to fill this function

END:
	EXIT();
	return result;
}


/**
 * Internal Functions
 */

static retcode setRegDefault (Hmc5883 *hmc,
		                      Hmc5883Config *config) {
	ENTER();

	//First set all to zeros,
	memset(config, 0, sizeof(Hmc5883Config));

	//Then selective setting of params
	config->m_busSpeed = I2C_BUS_SPEED_STD;
	config->m_dataRate = DATA_OUT_RATE_15;
	config->m_measureGain = MEASURE_GAIN_1090;
	config->m_measureMode = MEASURE_MODE_NORMAL;
	config->m_operMode = OPER_MODE_SINGLE;
	config->m_overSmplRatio = OVER_SMPL_RATIO_1;

	EXIT();
	return 0;
}

static retcode resetHmc5883Data (Hmc5883 *hmc) {

	ENTER();

	retcode retVal = 0;

	//Nothing to do for now

	EXIT();
	return retVal;
}


static retcode updateChip (Hmc5883 *hmc) {
	ENTER();

	retcode retVal = 0;

	//CONFIG_A Register
    retVal = updateReg_CONFIG_A(hmc);
	if (retVal){
		ERROR("Failed to update the CONFIG_A register");
		goto END;
	}

	//CONFIG_B Register
    retVal = updateReg_CONFIG_B(hmc);
	if (retVal){
		ERROR("Failed to update the CONFIG_B register");
		goto END;
	}

	//MODE Register
    retVal = updateReg_MODE(hmc);
	if (retVal){
		ERROR("Failed to update the MODE register");
		goto END;
	}

END:
	EXIT();
	return retVal;
}


static retcode startChip (Hmc5883 *hmc) {
	ENTER();

	retcode retVal = 0;

	retVal = resetChip(hmc);
	if (retVal){
		ERROR("Failed to reset the chip");
		goto END;
	}


END:
	EXIT();
	return retVal;
}


static retcode stopChip (Hmc5883 *hmc) {
	ENTER();

	retcode retVal = 0;

	//Do Nothing

	EXIT();
	return retVal;
}

static retcode resetChip (Hmc5883 *hmc) {
	ENTER();

	retcode retVal = 0;
	uint8 regVal[] = { HMC5883_REG_CONFIG_A,
			           HMC5883_REG_CONFIG_A_DEFAULT,
					   HMC5883_REG_CONFIG_B_DEFAULT,
					   HMC5883_REG_MODE_DEFAULT };

	retVal = writeBytes(hmc, regVal, 4);
	if (retVal){
		ERROR("Failed to write default values to registers");
		goto END;
	}

END:
	EXIT();
	return retVal;
}




static retcode setReadPointer (Hmc5883 * hmc, const uint8 reg) {

	ENTER();

	retcode retVal = 0;
	uint8 regVal[1];

	regVal[0] = reg;

	retVal = writeBytes(hmc, regVal, 1);
	if (retVal){
		ERROR("Failed to Set Read Pointer");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

//Get from HW Registers
static Hmc5883_I2cBusSpeed getBusSpeed (Hmc5883 *hmc) {

	ENTER();

	retcode retVal = 0;
	Hmc5883_I2cBusSpeed speed;
	uint8 regVal[1];

	retVal = setReadPointer(hmc, HMC5883_REG_MODE);
	if (retVal){
		ERROR("Failed to Set Read Pointer");
		speed = I2C_BUS_SPEED_INVALID;
		goto END;
	}

	retVal = readBytes(hmc, regVal, 1);
	if (retVal){
		ERROR("Failed to Read the MODE Register");
		speed = I2C_BUS_SPEED_INVALID;
		goto END;
	}

	switch (regVal[0] & HMC5883_BUS_SPEED_MASK) {
	case HMC5883_BUS_SPEED_STD:
		speed = I2C_BUS_SPEED_STD;
		break;

	case HMC5883_BUS_SPEED_HIGH:
		speed = I2C_BUS_SPEED_HIGH;
		break;

	default:
		ERROR("Invalid Value for Bus Speed");
		speed = I2C_BUS_SPEED_INVALID;
		goto END;
		break;
	}

END:
	EXIT();
	return speed;
}

static Hmc5883_OperMode getOperMode (Hmc5883 *hmc) {

	ENTER();

	retcode retVal = 0;
	Hmc5883_OperMode mode;
	uint8 regVal[1];

	retVal = setReadPointer(hmc, HMC5883_REG_MODE);
	if (retVal){
		ERROR("Failed to Set Read Pointer");
		mode = OPER_MODE_INVALID;
		goto END;
	}

	retVal = readBytes(hmc, regVal, 1);
	if (retVal){
		ERROR("Failed to Read the Mode Register");
		mode = OPER_MODE_INVALID;
		goto END;
	}

	switch (regVal[0] & HMC5883_OPER_MODE_MASK) {
	case HMC5883_OPER_MODE_CONTINUOUS:
		mode = OPER_MODE_CONT;
		break;

	case HMC5883_OPER_MODE_SINGLE:
		mode = OPER_MODE_SINGLE;
		break;

	case HMC5883_OPER_MODE_IDLE:
	case HMC5883_OPER_MODE_IDLE_2:
		mode = OPER_MODE_IDLE;
		break;

	default:
		ERROR("Invalid Value for operating Mode");
		mode = OPER_MODE_INVALID;
		goto END;
		break;
	}

END:
	EXIT();
	return mode;
}


static Hmc5883_DataOutRate getDataOutRate (Hmc5883 *hmc) {

	ENTER();

	retcode retVal = 0;
	Hmc5883_DataOutRate rate;
	uint8 regVal[1];

	retVal = setReadPointer(hmc, HMC5883_REG_CONFIG_A);
	if (retVal) {
		ERROR("Failed to Set Read Pointer");
		rate = DATA_OUT_RATE_INVALID;
		goto END;
	}

	retVal = readBytes(hmc, regVal, 1);
	if (retVal) {
		ERROR("Failed to Read CONFIG_A Register");
		rate = DATA_OUT_RATE_INVALID;
		goto END;
	}

	switch (regVal[0] & HMC5883_DATA_OUT_RATE_MASK) {
	case HMC5883_DATA_OUT_RATE_0_75:
		rate = DATA_OUT_RATE_0_75;
		break;

	case HMC5883_DATA_OUT_RATE_1_5:
		rate = DATA_OUT_RATE_1_5;
		break;

	case HMC5883_DATA_OUT_RATE_3:
		rate = DATA_OUT_RATE_3;
		break;

	case HMC5883_DATA_OUT_RATE_7_5:
		rate = DATA_OUT_RATE_7_5;
		break;

	case HMC5883_DATA_OUT_RATE_15:
		rate = DATA_OUT_RATE_15;
		break;

	case HMC5883_DATA_OUT_RATE_30:
		rate = DATA_OUT_RATE_30;
		break;

	case HMC5883_DATA_OUT_RATE_75:
		rate = DATA_OUT_RATE_75;
		break;


	default:
		ERROR("Invalid Value for operating Mode");
		rate = DATA_OUT_RATE_INVALID;
		goto END;
		break;
	}

END:
	EXIT();
	return rate;
}

static Hmc5883_OverSmplRatio getOverSmplRatio (Hmc5883 *hmc) {

	ENTER();

	retcode retVal = 0;
	Hmc5883_OverSmplRatio ratio;
	uint8 regVal[1];

	retVal = setReadPointer(hmc, HMC5883_REG_CONFIG_A);
	if (retVal) {
		ERROR("Failed to Set Read Pointer");
		ratio = OVER_SMPL_RATIO_INVALID;
		goto END;
	}

	retVal = readBytes(hmc, regVal, 1);
	if (retVal) {
		ERROR("Failed to read the CONFIG_A Register");
		ratio = OVER_SMPL_RATIO_INVALID;
		goto END;
	}

	switch (regVal[0] & HMC5883_OVER_SAMPL_MASK) {
	case HMC5883_OVER_SAMPL_1:
		ratio = OVER_SMPL_RATIO_1;
		break;

	case HMC5883_OVER_SAMPL_2:
		ratio = OVER_SMPL_RATIO_2;
		break;

	case HMC5883_OVER_SAMPL_4:
		ratio = OVER_SMPL_RATIO_4;
		break;

	case HMC5883_OVER_SAMPL_8:
		ratio = OVER_SMPL_RATIO_8;
		break;

	default:
		ERROR("Invalid Value for OVer Sampling Ratio");
		ratio = OVER_SMPL_RATIO_INVALID;
		goto END;
		break;
	}

END:
	EXIT();
	return ratio;
}

static Hmc5883_MeasureMode getMeasureMode (Hmc5883 *hmc) {

	ENTER();

	retcode retVal = 0;
	Hmc5883_MeasureMode mode;
	uint8 regVal[1];

	retVal = setReadPointer(hmc, HMC5883_REG_CONFIG_A);
	if (retVal){
		ERROR("Failed to Set Read Pointer");
		mode = MEASURE_MODE_INVALID;
		goto END;
	}

	retVal = readBytes(hmc, regVal, 1);
	if (retVal){
		ERROR("Failed to Read the CONFIG_A Register");
		mode = MEASURE_MODE_INVALID;
		goto END;
	}

	switch (regVal[0] & HMC5883_MEASURE_MODE_MASK) {
	case HMC5883_MEASURE_MODE_NORMAL:
		mode = MEASURE_MODE_NORMAL;
		break;

	case HMC5883_MEASURE_MODE_POS_BIAS:
		mode = MEASURE_MODE_POS_BIAS;
		break;

	case HMC5883_MEASURE_MODE_NEG_BIAS:
		mode = MEASURE_MODE_NEG_BIAS;
		break;


	default:
		ERROR("Invalid Value for Measurement Mode");
		mode = MEASURE_MODE_INVALID;
		goto END;
		break;
	}

END:
	EXIT();
	return mode;

}

static Hmc5883_MeasureGain getMeasureGain (Hmc5883 *hmc) {
	ENTER();

	retcode retVal = 0;
	Hmc5883_MeasureGain gain;
	uint8 regVal[1];

	retVal = setReadPointer(hmc, HMC5883_REG_CONFIG_B);
	if (retVal){
		ERROR("Failed to Set Read Pointer");
		gain = MEASURE_GAIN_INVALID;
		goto END;
	}

	retVal = readBytes(hmc, regVal, 1);
	if (retVal){
		ERROR("Failed to Read CONFIG_B Register");
		gain = MEASURE_GAIN_INVALID;
		goto END;
	}

	switch (regVal[0] & HMC5883_MEASURE_GAIN_MASK) {
	case HMC5883_MEASURE_GAIN_1370:
		gain = MEASURE_GAIN_1370;
		break;

	case HMC5883_MEASURE_GAIN_1090:
		gain = MEASURE_GAIN_1090;
		break;

	case HMC5883_MEASURE_GAIN_820:
		gain = MEASURE_GAIN_820;
		break;

	case HMC5883_MEASURE_GAIN_660:
		gain = MEASURE_GAIN_660;
		break;

	case HMC5883_MEASURE_GAIN_440:
		gain = MEASURE_GAIN_440;
		break;

	case HMC5883_MEASURE_GAIN_390:
		gain = MEASURE_GAIN_390;
		break;

	case HMC5883_MEASURE_GAIN_330:
		gain = MEASURE_GAIN_330;
		break;

	case HMC5883_MEASURE_GAIN_230:
		gain = MEASURE_GAIN_230;
		break;

	default:
		ERROR("Invalid Value for Measurement Gain");
		gain = MEASURE_GAIN_INVALID;
		goto END;
		break;
	}

END:
	EXIT();
	return gain;

}


static retcode updateReg_CONFIG_A (Hmc5883 *hmc) {

	ENTER();

	Hmc5883Config *config = &hmc->m_configs;
	Hmc5883Config *activeConfig = &hmc->m_activeConfigs;

	retcode retVal = 0;
	uint8 regVal[] = {HMC5883_REG_CONFIG_A ,
			          0 };

	if ((config->m_dataRate != activeConfig->m_dataRate) ||
			(config->m_measureMode != activeConfig->m_measureMode) ||
			(config->m_overSmplRatio != activeConfig->m_overSmplRatio)){

		switch (config->m_measureMode) {
		case MEASURE_MODE_NORMAL:
			regVal[1] |= HMC5883_MEASURE_MODE_NORMAL;
			break;

		case MEASURE_MODE_POS_BIAS:
			regVal[1] |= HMC5883_MEASURE_MODE_POS_BIAS;
			break;

		case MEASURE_MODE_NEG_BIAS:
			regVal[1] |= HMC5883_MEASURE_MODE_NEG_BIAS;
			break;

		default:
			ERROR("Invalid Measure Mode");
			break;
		}

		switch (config->m_dataRate) {
		case DATA_OUT_RATE_0_75:
			regVal[1] |= HMC5883_DATA_OUT_RATE_0_75;
			break;

		case DATA_OUT_RATE_1_5:
			regVal[1] |= HMC5883_DATA_OUT_RATE_1_5;
			break;

		case DATA_OUT_RATE_3:
			regVal[1] |= HMC5883_DATA_OUT_RATE_3;
			break;

		case DATA_OUT_RATE_7_5:
			regVal[1] |= HMC5883_DATA_OUT_RATE_7_5;
			break;

		case DATA_OUT_RATE_15:
			regVal[1] |= HMC5883_DATA_OUT_RATE_15;
			break;

		case DATA_OUT_RATE_30:
			regVal[1] |= HMC5883_DATA_OUT_RATE_30;
			break;

		case DATA_OUT_RATE_75:
			regVal[1] |= HMC5883_DATA_OUT_RATE_75;
			break;

		default:
			ERROR("Invalid Data Rate");
			break;
		}

		switch (config->m_overSmplRatio) {
		case OVER_SMPL_RATIO_1:
			regVal[1] |= HMC5883_OVER_SAMPL_1;
			break;

		case OVER_SMPL_RATIO_2:
			regVal[1] |= HMC5883_OVER_SAMPL_2;
			break;

		case OVER_SMPL_RATIO_4:
			regVal[1] |= HMC5883_OVER_SAMPL_4;
			break;

		case OVER_SMPL_RATIO_8:
			regVal[1] |= HMC5883_OVER_SAMPL_8;
			break;

		default:
			ERROR("Invalid OverSampling Ratio");
			break;
		}

		retVal = writeBytes(hmc, regVal, 2);
    	if (retVal) {
    		ERROR("Failed to Update the CONFIG_A Register");
    		goto END;
    	}

		activeConfig->m_dataRate = config->m_dataRate;
		activeConfig->m_measureMode = config->m_measureMode;
		activeConfig->m_overSmplRatio = config->m_overSmplRatio;
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_CONFIG_B (Hmc5883 *hmc) {

	ENTER();

	Hmc5883Config *config = &hmc->m_configs;
	Hmc5883Config *activeConfig = &hmc->m_activeConfigs;

	retcode retVal = 0;
	uint8 regVal[] = {HMC5883_REG_CONFIG_B,
			          0 };

	if (config->m_measureGain != activeConfig->m_measureGain) {

		switch (config->m_measureGain) {
		case MEASURE_GAIN_1370:
			regVal[1] |= HMC5883_MEASURE_GAIN_1370;
			break;

		case MEASURE_GAIN_1090:
			regVal[1] |= HMC5883_MEASURE_GAIN_1090;
			break;

		case MEASURE_GAIN_820:
			regVal[1] |= HMC5883_MEASURE_GAIN_820;
			break;

		case MEASURE_GAIN_660:
			regVal[1] |= HMC5883_MEASURE_GAIN_660;
			break;

		case MEASURE_GAIN_440:
			regVal[1] |= HMC5883_MEASURE_GAIN_440;
			break;

		case MEASURE_GAIN_390:
			regVal[1] |= HMC5883_MEASURE_GAIN_390;
			break;

		case MEASURE_GAIN_330:
			regVal[1] |= HMC5883_MEASURE_GAIN_330;
			break;

		case MEASURE_GAIN_230:
			regVal[1] |= HMC5883_MEASURE_GAIN_230;
			break;

		default:
			ERROR("Invalid Measure Gain");
			break;
		}

		retVal = writeBytes(hmc, regVal, 2);
    	if (retVal) {
    		ERROR("Failed to Update the CONFIG_B Register");
    		goto END;
    	}

		activeConfig->m_measureGain = config->m_measureGain;
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_MODE (Hmc5883 *hmc) {

	ENTER();

	Hmc5883Config *config = &hmc->m_configs;
	Hmc5883Config *activeConfig = &hmc->m_activeConfigs;

	retcode retVal = 0;
	uint8 regVal[] = {HMC5883_REG_MODE,
			          0 };

	if ((config->m_operMode != activeConfig->m_operMode) ||
			(config->m_busSpeed != activeConfig->m_busSpeed)) {

		switch (config->m_operMode) {
		case OPER_MODE_IDLE:
			regVal[1] |= HMC5883_OPER_MODE_IDLE;
			break;

		case OPER_MODE_SINGLE:
			regVal[1] |= HMC5883_OPER_MODE_SINGLE;
			break;

		case OPER_MODE_CONT:
			regVal[1] |= HMC5883_OPER_MODE_CONTINUOUS;
			break;

		default:
			ERROR("Invalid Operating Mode");
			break;
		}

		switch (config->m_busSpeed) {
		case I2C_BUS_SPEED_STD:
			regVal[1] |= HMC5883_BUS_SPEED_STD;
			break;

		case I2C_BUS_SPEED_HIGH:
			regVal[1] |= HMC5883_BUS_SPEED_HIGH;
			break;

		default:
			ERROR("Invalid Bus Speed");
			break;
		}

		retVal = writeBytes(hmc, regVal, 2);
    	if (retVal) {
    		ERROR("Failed to Update the MODE Register");
    		goto END;
    	}

		activeConfig->m_busSpeed = config->m_busSpeed;

		if (config->m_operMode == HMC5883_OPER_MODE_SINGLE) {
			//Since this mode is a temporary state until a new read is performed
			activeConfig->m_operMode = config->m_operMode = OPER_MODE_IDLE;
		} else {
			activeConfig->m_operMode = config->m_operMode;
		}
	}

END:
	EXIT();
	return retVal;
}


//Read & Write into HW
retcode readBytes (Hmc5883 *hmc, uint8* data, const uint8 count) {
	ENTER();

	//Currently it is just a stub
	//TODO

	PROGRESS("Reading %d Bytes from HW", count);

	EXIT();
	return 0;
}

retcode writeBytes (Hmc5883 *hmc, uint8 *data, const uint8 count) {
	ENTER();

	//Currently it is just a stub
	//TODO

	PROGRESS("Writing %d Bytes to the HW", count);
	for (uint8 i = 0; i < count ; i++) {
		INFO("data[%d] = %d", i, data[i]);
	}

	EXIT();
	return 0;
}

