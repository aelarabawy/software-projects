/*
 * ms5611.c
 *
 *  Created on: Dec 29, 2014
 *      Author: aelarabawy
 */

#include <math.h>
#include <unistd.h>
#include "logger.h"
#include "ms5611_regs.h"
#include "ms5611.h"

//Internal Functions Prototypes
static retcode resetMs5611Data(Ms5611 *);
static retcode startChip (Ms5611 *);
static retcode stopChip (Ms5611 *);
static retcode resetChip (Ms5611 *);
static retcode updateChip (Ms5611 *);

static retcode readBytes (Ms5611 *, uint8 *, uint8);
static retcode writeBytes (Ms5611 *, uint8 *, uint8);
/**
 * Basic functions
 */

/**
 * This function should create the needed data structure
 * It does not do any Hardware initialization
 */
msHandle ms5611_Init (void) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)malloc(sizeof(Ms5611));
	if (!ms5611) {
		ALLOC_FAIL("ms5611");
		goto END;
	}
	memset(ms5611, 0x00, sizeof(Ms5611));

	ms5611->m_state = MS5611_STATE_INIT;

	//Set Initial Values for configurations (Based on chip default values)
	resetMs5611Data(ms5611);

END:
	EXIT();
	return (msHandle)ms5611;
}

/**
 * This function starts the chip if not started
 * This includes reading the PROM data and storing its contents internally
 * And verifying its integrity
 */
retcode ms5611_Start (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}


	if (ms5611->m_state != MS5611_STATE_INIT) {
		ERROR("MS5611 Already Started .... Exiting");
		retVal = -1;
		goto END;
	}

	if (!ms5611->m_i2cAddr) {
		ERROR("Can not Start Chip .... need to specify the I2C Bus Address First");
		retVal = -1;
		goto END;
	}

	ms5611->m_state = MS5611_STATE_STARTING;
	retVal = startChip(ms5611);
	if (retVal) {
		ERROR("Can not start Chip");
		goto END;
	}

	ms5611->m_state = MS5611_STATE_STARTED;

END:
	EXIT();
	return retVal;
}

/**
 * This function does not do anything for now
 * It is in the API for consistency reasons
 */
retcode ms5611_Stop (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	if (ms5611->m_state != MS5611_STATE_STARTED) {
		ERROR("MS5611 Chip Not Started .... Exiting");
		retVal = -1;
		goto END;
	}

	ms5611->m_state = MS5611_STATE_STOPPING;
	retVal = stopChip(ms5611);
	if (retVal) {
		ERROR("Can not stop Chip");
		goto END;
	}

	ms5611->m_state = MS5611_STATE_INIT;

END:
	EXIT();
	return retVal;
}

/**
 * This function will cleanup any data structures associated with the chip
 */
retcode ms5611_Destroy (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	if (ms5611->m_state != MS5611_STATE_INIT) {
		ERROR("MS5611 Started ....Need to stop it first ...Exiting");
		retVal = -1;
		goto END;
	}


	//Free the object
	free(ms5611);

END:
	EXIT();
	return retVal;
}


/**
 * This function will RESET the chip (send the RESET Command)
 */
retcode ms5611_Reset (msHandle handle) {
	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	retVal = resetChip(ms5611);
	if (retVal) {
		ERROR("Can not reset chip");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

/**
 * This function will Update the HW
 * Note: Since the chip does not have any HW Register, this function does not do anything
 */
retcode ms5611_Update (msHandle handle) {
	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	retVal = updateChip(ms5611);
	if (retVal) {
		ERROR("Can not update chip");
		goto END;
	}

END:
	EXIT();
	return retVal;
}


/**
 * These functions are Set/Get for the I2CAddr
 * the Set() just specify the high/low address
 * the Get() will return the actual address
 */
retcode ms5611_SetI2cAddr (msHandle handle,
		                   const Ms5611_I2cAddr addr) {
	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	if (ms5611->m_state != MS5611_STATE_INIT) {
		ERROR("MS5611 Started ....Can not Set Address ...Need to stop it first ...Exiting");
		retVal = -1;
		goto END;
	}


	if (ms5611->m_i2cAddr) {
		WARN("I2C Address Already Set ....Overriding stored Value");
	}

	switch (addr){
	case I2C_ADDR_CSB_LOW:
		ms5611->m_i2cAddr = MS5611_I2C_ADDRESS_CSB_LOW;
		break;

	case I2C_ADDR_CSB_HIGH:
		ms5611->m_i2cAddr = MS5611_I2C_ADDRESS_CSB_HIGH;
		break;

	default:
		ERROR("Invalid I2C Address");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

uint8 ms5611_GetI2cAddr (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_i2cAddr;
}

Ms5611_I2cAddr ms5611_ParseI2cAddr(char *addrStr) {

	ENTER();

	Ms5611_I2cAddr addr;

	if (strcmp(addrStr, "low") == 0) {
		addr = I2C_ADDR_CSB_LOW;
	} else if (strcmp(addrStr, "high") == 0) {
		addr = I2C_ADDR_CSB_HIGH;
	} else {
		ERROR("Invalid I2C Address %s", addrStr);
		addr = I2C_ADDR_CSB_INVALID;
		goto END;
	}

END:
	EXIT();
	return addr;
}


/**
 * Retrieving the Chip PROM Data
 */
uint16 ms5611_GetPressureSens (msHandle handle) {
	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_SENS_T1;
}

uint16 ms5611_GetPressureOffset (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_OFF_T1;
}

uint16 ms5611_GetTempCoffPressureSens (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_TCS;
}

uint16 ms5611_GetTempCoffPressureOffset (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_TCO;
}

uint16 ms5611_GetRefTemp (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_T_REF;
}

uint16 ms5611_GetTempCoffTemp (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_TEMPSENS;
}

/**
 * Set/Get the Over-Sampling Ratio
 */
retcode ms5611_SetPressureOsr (msHandle handle,
		                       Ms5611_Osr osr) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	switch (osr) {
	case OSR_256:
	case OSR_512:
	case OSR_1024:
	case OSR_2048:
	case OSR_4096:
		ms5611->m_pressureOsr = osr;
		break;

	default:
		ERROR("Invalid value for Pressure OSR");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Ms5611_Osr ms5611_GetPressureOsr (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_pressureOsr;
}

retcode ms5611_SetTempOsr (msHandle handle,
		                   Ms5611_Osr osr) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	switch (osr) {
	case OSR_256:
	case OSR_512:
	case OSR_1024:
	case OSR_2048:
	case OSR_4096:
		ms5611->m_tempOsr = osr;
		break;

	default:
		ERROR("Invalid value for Temperature OSR");
		retVal = -1;
		goto END;
		break;
	}


END:
	EXIT();
	return retVal;
}

Ms5611_Osr ms5611_GetTempOsr (msHandle handle) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		goto END;
	}

END:
	EXIT();
	return ms5611->m_tempOsr;
}

retcode ms5611_ConvertOsr2String(Ms5611_Osr osr, char ** pOsrStr) {

	ENTER();
	retcode retVal = 0;

	switch (osr) {
	case OSR_256:
		strcpy(*pOsrStr, "256");
		break;

	case OSR_512:
		strcpy(*pOsrStr, "512");
		break;

	case OSR_1024:
		strcpy(*pOsrStr, "1024");
		break;

	case OSR_2048:
		strcpy(*pOsrStr, "2048");
		break;

	case OSR_4096:
		strcpy(*pOsrStr, "4096");
		break;

	default:
		strcpy(*pOsrStr, "Invalid OSR Value");
		retVal = -1;
		goto END;
		break;

	}


END:
	EXIT();
	return retVal;
}

Ms5611_Osr ms5611_ParseOsr(char *osrStr) {
	ENTER();
	Ms5611_Osr osr;

	if (strcmp(osrStr, "256") == 0) {
		osr = OSR_256;
	} else if (strcmp(osrStr, "512") == 0) {
		osr = OSR_512;
	} else if (strcmp(osrStr, "1024") == 0) {
		osr = OSR_1024;
	} else if (strcmp(osrStr, "2048") == 0) {
		osr = OSR_2048;
	} else if (strcmp(osrStr, "4096") == 0) {
		osr = OSR_4096;
	} else {
		ERROR("Invalid value for OSR %s", osrStr);
		osr = OSR_INVALID;
		goto END;
	}

END:
	EXIT();
	return osr;
}


/**
 * Perform a Pressure/Temperature Measurement
 */
retcode ms5611_ReadSensorData(msHandle handle,
		                      Ms5611_SensorType sensor) {

	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;
	uint8 regVal[1];

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	if (sensor == SENSOR_TYPE_PRESSURE) {
		PROGRESS("Ordering a Read for the Pressure Sensor");

		switch (ms5611->m_pressureOsr) {
		case OSR_256:
			regVal[0] = MS5611_COMMAND_CONVERT_D1_256;
			break;

		case OSR_512:
			regVal[0] = MS5611_COMMAND_CONVERT_D1_512;
			break;

		case OSR_1024:
			regVal[0] = MS5611_COMMAND_CONVERT_D1_1024;
			break;

		case OSR_2048:
			regVal[0] = MS5611_COMMAND_CONVERT_D1_2048;
			break;

		case OSR_4096:
			regVal[0] = MS5611_COMMAND_CONVERT_D1_4096;
			break;

		default:
			ERROR("Stored Value for Pressure OSR is invalid");
			retVal = -1;
			goto END;
			break;
		}

	} else if (sensor == SENSOR_TYPE_TEMP) {
		PROGRESS("Ordering a Read for the Temperature Sensor");

		switch (ms5611->m_tempOsr) {
		case OSR_256:
			regVal[0] = MS5611_COMMAND_CONVERT_D2_256;
			break;

		case OSR_512:
			regVal[0] = MS5611_COMMAND_CONVERT_D2_512;
			break;

		case OSR_1024:
			regVal[0] = MS5611_COMMAND_CONVERT_D2_1024;
			break;

		case OSR_2048:
			regVal[0] = MS5611_COMMAND_CONVERT_D2_2048;
			break;

		case OSR_4096:
			regVal[0] = MS5611_COMMAND_CONVERT_D2_4096;
			break;

		default:
			ERROR("Stored Value for Temperature OSR is invalid");
			retVal = -1;
			goto END;
			break;
		}
	} else {
		ERROR("Invalid Sensor....Exiting");
		retVal = -1;
		goto END;
	}

	retVal = writeBytes(ms5611, regVal, 1);
	if (retVal) {
		ERROR("Can not write the Convert D1/D2 Command to the chip");
		goto END;
	}


END:
	EXIT();
	return retVal;
}

/**
 * Get a reading
 */
retcode ms5611_GetSensorData(msHandle handle, uint32 *data) {
	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;
	uint8 regVal[4];

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	regVal[0] = MS5611_COMMAND_ADC_READ;

	retVal = writeBytes(ms5611, regVal, 1);
	if (retVal) {
		ERROR("Can not write the READ_ADC Command to the chip");
		goto END;
	}

	retVal = readBytes(ms5611, regVal, 3);
	if (retVal) {
		ERROR("Can not Read ADC output from the chip");
		goto END;
	}

	*data = (regVal[0] << 16) | (regVal[1] << 8) | regVal[0];

END:
	EXIT();
	return retVal;
}

/**
 * Perform Calculation
 */
retcode ms5611_PerformCalculation (msHandle handle,
		                           uint32 pressureRaw,
								   uint32 tempRaw,
								   float *p,
								   float *t) {
	ENTER();

	Ms5611 *ms5611 = (Ms5611 *)handle;
	retcode retVal = 0;
	int32 dT;
	int64 tmpVar;
	int64 TEMP = 0, P = 0;
	int64 OFF = 0, SENS = 0;
	int64 T2 = 0, OFF2 = 0, SENS2 = 0;

	if (!ms5611) {
		NULL_POINTER("ms5611");
		retVal = -1;
		goto END;
	}

	//Calculate TEMP
	dT = tempRaw - ((uint32)ms5611->m_T_REF << 8);
	tmpVar = dT * ms5611->m_TEMPSENS;
	tmpVar >>= 23;
	TEMP = 2000 + tmpVar;
	INFO("Before Second Order Compensation Check, TEMP = %d", TEMP);

	//Calculating OFF
	OFF = ms5611->m_OFF_T1 << 16;
	tmpVar = ms5611->m_TCO * dT;
	tmpVar >>= 7;
	OFF += tmpVar;
	INFO("Before Second Order Compensation Check, OFF = %d", OFF);

	//Calculating SENS
	SENS = ms5611->m_SENS_T1 << 15;
	tmpVar = ms5611->m_TCS * dT;
	tmpVar >>= 8;
	SENS += tmpVar;
	INFO("Before Second Order Compensation Check, SENS = %d", SENS);


	//Check if temperature needs second order calibration

	if (TEMP < 2000) {
		T2 = dT * dT;
		T2 >>= 31;

		tmpVar = TEMP - 2000;
		OFF2 = tmpVar * tmpVar * 5;
		OFF2 >>= 1;
		SENS2 = OFF2 >> 1;

		if (TEMP < -1500) {
			tmpVar = TEMP + 1500;
			OFF2 += 7 * tmpVar * tmpVar;

			tmpVar = 11 * tmpVar * tmpVar;
			tmpVar >>= 1;
			SENS2 += tmpVar;
		}

		INFO("Outcome of Second Order Temperature Compensation T2 = %d, OFF2 = %d, SENS2 = %d", T2, OFF2, SENS2);
	}

	TEMP -= T2;
	OFF -= OFF2;
	SENS -= SENS2;

	//Calculating Pressure
	P = pressureRaw * SENS;
	P >>= 21;
	P -= OFF;
	P >>= 15;
	INFO("Pressure = %d", P);

	*p = (float)P / 100.0;
	*t = (float)TEMP / 100.0;


END:
	EXIT();
	return retVal;
}

/**
 * Internal Functions
 */
static retcode startChip (Ms5611 *ms5611) {

	ENTER();

	retcode retVal = 0;
	uint16 coff[8];
	uint8 regVal[2];


	//First we read the PROM
	for (uint8 i = 0; i < 8; i++) {
		regVal[0] = MS5611_COMMAND_READ_PROM + (i * 2);
		retVal = writeBytes(ms5611, regVal, 1);
		if (retVal) {
			ERROR("Can not write READ_PROM [%d] Command to the chip",i);
			goto END;
		}

		retVal = readBytes(ms5611, regVal, 2);
		if (retVal) {
			ERROR("Can not Read READ_PROM [%d] Command to the chip", i);
			goto END;
		}

		coff[i] = (regVal[0] << 8) | regVal[1];
	}

	//Now we need to verify the Integrity of the data (by checking the CRC)
	//TODO check AN520 for the CRC verification

	//Then we assign values
	ms5611->m_SENS_T1 = coff[1];
	ms5611->m_OFF_T1 = coff[2];
	ms5611->m_TCS = coff[3];
	ms5611->m_TCO = coff[4];
	ms5611->m_T_REF = coff[5];
	ms5611->m_TEMPSENS = coff[6];


END:
	EXIT();
	return retVal;
}

static retcode stopChip (Ms5611 *ms5611) {

	ENTER();

	retcode retVal = 0;

	//Do Nothing

	EXIT();
	return retVal;
}

static retcode resetChip (Ms5611 *ms5611) {

	ENTER();

	retcode retVal = 0;
	uint8 regVal[1] = { MS5611_COMMAND_RESET };

	retVal = writeBytes(ms5611, regVal, 1);
	if (retVal) {
		ERROR("Can not write the Reset Command to the chip");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode updateChip (Ms5611 *ms5611) {

	ENTER();

	retcode retVal = 0;

//END:
	EXIT();
	return retVal;
}


static retcode resetMs5611Data(Ms5611 *ms5611) {

	ENTER();

	retcode retVal = 0;

	//Do Nothing

	EXIT();
	return retVal;
}

static retcode readBytes (Ms5611 *ms5611,
		                  uint8 *data,
						  uint8 count) {
	ENTER();

	retcode retVal = 0;

	//Do Nothing  TODO

	EXIT();
	return retVal;
}

static retcode writeBytes (Ms5611 *ms5611,
		                   uint8 *data,
					 	   uint8 count) {
	ENTER();

	retcode retVal = 0;

	//Do Nothing TODO

	EXIT();
	return retVal;
}
