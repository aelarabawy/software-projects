/*
 * mpu60x0.c
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#include <math.h>
#include <unistd.h>
#include "logger.h"
#include "mpu60x0_regs.h"
#include "mpu60x0.h"


//Internal Functions Prototypes
static retcode setRegDefault (Mpu60x0 *, Mpu60x0Config *);
static retcode updateChip (Mpu60x0 *);
static retcode startChip (Mpu60x0 *);
static retcode stopChip (Mpu60x0 *);
static retcode resetMpuData (Mpu60x0 *);

retcode writeReg (Mpu60x0 *, const uint8, const uint8);
uint8 readReg (Mpu60x0 *, const uint8);
retcode readMultiReg (Mpu60x0 *,  const uint8, uint8 *, const uint8);
retcode multiReadSameReg (Mpu60x0 *,  const uint8, uint8 *, const uint8);

static retcode resetSensorDataRegs(Mpu60x0 *);
static retcode readSensorData(Mpu60x0 *);
static retcode readSensroDataFromFifo (Mpu60x0 *);
static retcode getAccData(Mpu60x0 *, float *, float *, float *);
static retcode getGyroData(Mpu60x0 *, float *, float *, float *);
static retcode getTempData(Mpu60x0 *mpu, float *);

//Get from HW Registers
static Mpu60x0_ClkSrc getClkSrc (Mpu60x0 *);
static uint32 getSamplingRate (Mpu60x0 *);
static Mpu60x0_Lpf getLpf (Mpu60x0 *);
static Mpu60x0_AccFullScale getAccFullScale (Mpu60x0 *);
static Mpu60x0_GyroFullScale getGyroFullScale (Mpu60x0 *);
static bool getFifoUsage(Mpu60x0 *);
static uint8 getMotDetThr (Mpu60x0 *);
static uint8 getActiveSensors (Mpu60x0 *);
static Mpu60x0_IntLvl getIntLvl (Mpu60x0 *);
static bool getIsOpenDrainInt(Mpu60x0 *);
static bool getIsLatchInt(Mpu60x0 *);
static bool getClearOnReadInt(Mpu60x0 *);

static Mpu60x0_AuxI2cMode getAuxI2cMode (Mpu60x0 *);
static Mpu60x0_AuxI2cClk getAuxI2cClk (Mpu60x0 *);
static uint8 getAuxI2cReducedSamplingFactor (Mpu60x0 *);

//Updating Registers
static retcode updateReg_MOT_DETECT_CTRL (Mpu60x0 *);
static retcode updateReg_PWR_MGMT_1 (Mpu60x0 *);
static retcode updateReg_PWR_MGMT_2 (Mpu60x0 *);

//Encode Register values
static uint8 getFsyncExtEncoding (const Mpu60x0_SensorId );
static uint8 getGyroScaleEncoding (const Mpu60x0_GyroFullScale);
static uint8 getAccScaleEncoding (const Mpu60x0_AccFullScale);
static uint8 getAuxI2cClkEncoding (const Mpu60x0_AuxI2cClk);
static uint8 getDlpfEncoding (const Mpu60x0_Lpf);

//Self test
static bool calcSelfTestAccX (Mpu60x0 *, float);
static bool calcSelfTestAccY (Mpu60x0 *, float);
static bool calcSelfTestAccZ (Mpu60x0 *, float);
static bool calcSelfTestGyroX (Mpu60x0 *, float);
static bool calcSelfTestGyroY (Mpu60x0 *, float);
static bool calcSelfTestGyroZ (Mpu60x0 *, float);
//////////////////////////////

/**
 * Basic Functions
 */
mpuHandle mpu60x0_Init (void) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)malloc(sizeof(Mpu60x0));
	if (!mpu) {
		ALLOC_FAIL("mpu");
		goto END;
	}
	memset(mpu, 0x00, sizeof(Mpu60x0));

	mpu->m_state = MPU60X0_STATE_INIT;

	//Set Initial Values for configurations (Based on chip default values)
	setRegDefault(mpu, &mpu->m_configs);
	setRegDefault(mpu, &mpu->m_activeConfigs);
	resetMpuData(mpu);

END:
	EXIT();
	return (mpuHandle)mpu;
}

/**
 * This function starts the chip
 * Before it wakes up the chip (bring it up from sleep mode), it will apply any needed initialization to it
 */
retcode mpu60x0_Start (mpuHandle handle) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}


	if (mpu->m_state != MPU60X0_STATE_INIT) {
		ERROR("MPU Already Started .... Exiting");
		retVal = -1;
		goto END;
	}

	if (!mpu->m_i2cAddr) {
		ERROR("Can not Start Chip .... need to specify the I2C Bus Address First");
		retVal = -1;
		goto END;
	}

	mpu->m_state = MPU60X0_STATE_STARTING;
	retVal = startChip(mpu);
	if (retVal) {
		ERROR("Can not start Chip");
		goto END;
	}

	mpu->m_state = MPU60X0_STATE_STARTED;

END:
	EXIT();
	return retVal;
}

/**
 * This function will put the chip in sleep mode
 * It can be started later if needed
 */
retcode mpu60x0_Stop (mpuHandle handle) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_STARTED) {
		ERROR("MPU Not Started .... Exiting");
		retVal = -1;
		goto END;
	}

	mpu->m_state = MPU60X0_STATE_STOPPING;
	retVal = stopChip(mpu);
	if (retVal) {
		ERROR("Can not stop Chip");
		goto END;
	}

	mpu->m_state = MPU60X0_STATE_INIT;

END:
	EXIT();
	return retVal;
}

/**
 * This function will cleanup any data structures associated with the chip
 */
retcode mpu60x0_Destroy (mpuHandle handle) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		ERROR("MPU Started ....Need to stop it first ...Exiting");
		retVal = -1;
		goto END;
	}


	//Free the object
	free(mpu);

END:
	EXIT();
	return retVal;
}

/**
 * This function will Update the chip configuration to the HW Register
 */
retcode mpu60x0_Update (mpuHandle handle) {

	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	retVal = updateChip(mpu);
	if (retVal) {
		ERROR("Can not update Chip");
		goto END;
	}

	END:
		EXIT();
		return retVal;
}


retcode mpu60x0_SetI2cAddr (mpuHandle handle,
		                 const Mpu60x0_I2cAddr addr) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		ERROR("MPU Started ....Can not Set Address ...Need to stop it first ...Exiting");
		retVal = -1;
		goto END;
	}


	if (mpu->m_i2cAddr) {
		WARN("I2C Address Already Set ....Overriding stored Value");
	}

	switch (addr){
	case I2C_ADDR_LOW:
		mpu->m_i2cAddr = MPU60X0_I2C_ADDRESS_LOW;
		break;

	case I2C_ADDR_HIGH:
		mpu->m_i2cAddr = MPU60X0_I2C_ADDRESS_HIGH;
		break;

	default:
		ERROR("Invalid I2C Address Passed");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}


uint8 mpu60x0_GetI2cAddr (mpuHandle handle) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

END:
	EXIT();
	return mpu->m_i2cAddr;
}

Mpu60x0_I2cAddr mpu60x0_ParseI2cAddr (char *addrStr) {

	ENTER();

	Mpu60x0_I2cAddr addr;

	if (strcmp(addrStr, "low") == 0) {
		addr = I2C_ADDR_LOW;
	} else if (strcmp(addrStr, "high") == 0) {
		addr = I2C_ADDR_HIGH;
	} else {
		ERROR("Invalid I2C Address %s", addrStr);
		addr = I2C_ADDR_INVALID;
		goto END;
	}

END:
	EXIT();
	return addr;
}


/**
 * These functions will set/get the clock source of the chip
 */
retcode mpu60x0_SetClkSrc (mpuHandle handle,
  		                   const Mpu60x0_ClkSrc clk) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_clkSrc = clk;

END:
	EXIT();
	return retVal;
}


Mpu60x0_ClkSrc mpu60x0_GetClkSrc (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	Mpu60x0_ClkSrc clk;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		clk = mpu->m_configs.m_clkSrc;
		break;

	case DATA_SRC_ACTIVE:
		clk = mpu->m_activeConfigs.m_clkSrc;
		break;

	case DATA_SRC_HW:
		clk = getClkSrc(mpu);
		break;
	}

END:
	EXIT();
	return clk;
}

retcode mpu60x0_ConvertClkSrc2String(Mpu60x0_ClkSrc clk, char **pClkStr) {
	ENTER();

	retcode retVal = 0;

	switch (clk) {
	case CLK_SRC_INT_OSC:
		strcpy(*pClkStr, "Internal Oscillator");
		break;

	case CLK_SRC_GYRO:
		strcpy(*pClkStr, "Internal PLL using Gyroscope Oscillator");
		break;

	case CLK_SRC_EXT_LOW:
		strcpy(*pClkStr, "External Clock (32.768 KHz)");
		break;

	case CLK_SRC_EXT_HIGH:
		strcpy(*pClkStr, "External Clock (19.2 MHz)");
		break;

	default:
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Mpu60x0_ClkSrc mpu60x0_ParseClkSrc (char *clkStr) {

	ENTER();

	Mpu60x0_ClkSrc clk;

	if (strcmp(clkStr, "internal") == 0) {
		clk = CLK_SRC_INT_OSC;
	} else if (strcmp(clkStr, "pll") == 0) {
		clk = CLK_SRC_GYRO;
	} else if (strcmp(clkStr, "external-low") == 0) {
		clk = CLK_SRC_EXT_LOW;
	} else if (strcmp(clkStr, "external-high") == 0) {
		clk = CLK_SRC_EXT_HIGH;
	} else {
		ERROR("Invalid Clock source %s", clkStr);
		clk = CLK_SRC_INVALID;
		goto END;
	}

END:
	EXIT();
	return clk;
}


uint32 mpu60x0_SetSamplingRate (mpuHandle handle,
                                uint32 rate,
								Mpu60x0_Lpf lpf) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	uint32 div;
	uint32 actual = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

	mpu->m_configs.m_dlpf = lpf;

	if (rate > MPU60X0_MAX_SMPL_RATE) {
		WARN("Requested Sampling Rate Exceeds Max Sampling Rate supported by the chip....reverting to max available value");
		rate = MPU60X0_MAX_SMPL_RATE;
	}

	if (rate < MPU60X0_MIN_SMPL_RATE) {
		WARN("Requested Sampling Rate Below Min Sampling Rate supported by the chip....reverting to min available value");
		rate = MPU60X0_MIN_SMPL_RATE;
	}

	if ((lpf == LPF_NONE) || (lpf == LPF_RESERVED)) {
		div = 8000 / rate;
		if (div > 256) {
			WARN("Requested Rate is too small ... reverting to smallest possible value");
			div = 256;
		}
		actual = 8000 / div;
	} else {
		div = 1000 / rate;
		if (div > 256) {
			WARN("Requested Rate is too small ... reverting to smallest possible value");
			div = 256;
		}
		actual = 1000 / div;
	}

	mpu->m_configs.m_samplingDiv = (uint8)(div - 1);

END:
	EXIT();
	return actual;
}

uint32 mpu60x0_GetSamplingRate (mpuHandle handle,
		                        const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	uint32 rate = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		if ((mpu->m_configs.m_dlpf == LPF_NONE) ||
				(mpu->m_configs.m_dlpf == LPF_RESERVED)) {
			rate = 8000 / (mpu->m_configs.m_samplingDiv + 1);
		} else {
			rate = 1000 / (mpu->m_configs.m_samplingDiv  + 1);
		}
		break;

	case DATA_SRC_ACTIVE:
		if ((mpu->m_configs.m_dlpf == LPF_NONE) ||
				(mpu->m_configs.m_dlpf == LPF_RESERVED)) {
			rate = 8000 / (mpu->m_activeConfigs.m_samplingDiv + 1);
		} else {
			rate = 1000 / (mpu->m_activeConfigs.m_samplingDiv + 1);
		}
		break;

	case DATA_SRC_HW:
		rate = getSamplingRate(mpu);
		break;
	}

END:
	EXIT();
	return rate;
}

retcode mpu60x0_SetLpf (mpuHandle handle, Mpu60x0_Lpf lpf) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_dlpf = lpf;

END:
	EXIT();
	return retVal;
}

Mpu60x0_Lpf mpu60x0_GetLpf (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	Mpu60x0_Lpf lpf;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		lpf = mpu->m_configs.m_dlpf;
		break;

	case DATA_SRC_ACTIVE:
		lpf = mpu->m_activeConfigs.m_dlpf;
		break;

	case DATA_SRC_HW:
		lpf = getLpf(mpu);
		break;
	}

END:
	EXIT();
	return lpf;
}

retcode mpu60x0_ConvertLpf2String(Mpu60x0_Lpf lpf, char **pLpfStr) {
	ENTER();

	retcode retVal = 0;

	switch (lpf) {
	case LPF_NONE:
		strcpy(*pLpfStr, "LPF None");
		break;

	case LPF_184_188:
		strcpy(*pLpfStr, "LPF 184,188");
		break;

	case LPF_94_98:
		strcpy(*pLpfStr, "LPF 94,98");
		break;

	case LPF_44_42:
		strcpy(*pLpfStr, "LPF 44,42");
		break;

	case LPF_21_20:
		strcpy(*pLpfStr, "LPF 21,20");
		break;

	case LPF_10_10:
		strcpy(*pLpfStr, "LPF 10,10");
		break;

	case LPF_5_5:
		strcpy(*pLpfStr, "LPF 5,5");
		break;

	case LPF_RESERVED:
		strcpy(*pLpfStr, "LPF Reserved");
		break;

	default:
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Mpu60x0_Lpf mpu60x0_ParseLpf (char *lpfStr) {

	ENTER();

	Mpu60x0_Lpf lpf;

	if (strcmp(lpfStr, "none") == 0) {
		lpf = LPF_NONE;
	} else if (strcmp(lpfStr, "184-188") == 0) {
		lpf = LPF_184_188;
	} else if (strcmp(lpfStr, "94-98") == 0) {
		lpf = LPF_94_98;
	} else if (strcmp(lpfStr, "44-42") == 0) {
		lpf = LPF_44_42;
	} else if (strcmp(lpfStr, "21-20") == 0) {
		lpf = LPF_21_20;
	} else if (strcmp(lpfStr, "10-10") == 0) {
		lpf = LPF_10_10;
	} else if (strcmp(lpfStr, "5-5") == 0) {
		lpf = LPF_5_5;
	} else if (strcmp(lpfStr, "reserved") == 0) {
		lpf = LPF_RESERVED;
	} else {
		ERROR("Invalid LPF value %s", lpfStr);
		lpf = LPF_INVALID;
		goto END;
	}

END:
	EXIT();
	return lpf;
}



retcode mpu60x0_SetAccFullScale (mpuHandle handle,
		                         const Mpu60x0_AccFullScale scale) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}


	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_accScale = scale;

END:
	EXIT();
	return retVal;
}


Mpu60x0_AccFullScale mpu60x0_GetAccFullScale (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	Mpu60x0_AccFullScale scale;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		scale = mpu->m_configs.m_accScale;
		break;
	case DATA_SRC_ACTIVE:
		scale = mpu->m_activeConfigs.m_accScale;
		break;
	case DATA_SRC_HW:
		scale = getAccFullScale(mpu);
		break;
	}

END:
	EXIT();
	return scale;
}

retcode mpu60x0_ConvertAccFsr2String(Mpu60x0_AccFullScale fsr, char **pFsrStr) {
	ENTER();

	retcode retVal = 0;

	switch (fsr) {
	case ACC_FULL_SCALE_2G:
		strcpy(*pFsrStr, "2g");
		break;

	case ACC_FULL_SCALE_4G:
		strcpy(*pFsrStr, "4g");
		break;

	case ACC_FULL_SCALE_8G:
		strcpy(*pFsrStr, "8g");
		break;

	case ACC_FULL_SCALE_16G:
		strcpy(*pFsrStr, "16g");
		break;

	default:
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Mpu60x0_AccFullScale mpu60x0_ParseAccFsr (char *fsrStr) {

	ENTER();

	Mpu60x0_AccFullScale fsr;

	if (strcmp(fsrStr, "2g") == 0) {
		fsr = ACC_FULL_SCALE_2G;
	} else if (strcmp(fsrStr, "4g") == 0) {
		fsr = ACC_FULL_SCALE_4G;
	} else if (strcmp(fsrStr, "8g") == 0) {
		fsr = ACC_FULL_SCALE_8G;
	} else if (strcmp(fsrStr, "16g") == 0) {
		fsr = ACC_FULL_SCALE_16G;
	} else {
		ERROR("Invalid Accelerometer FSR value %s", fsrStr);
		fsr = ACC_FULL_SCALE_INVALID;
		goto END;
	}

END:
	EXIT();
	return fsr;
}


retcode mpu60x0_SetGyroFullScale (mpuHandle handle,
		                          const Mpu60x0_GyroFullScale scale) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_gyroScale = scale;

END:
	EXIT();
	return retVal;
}

Mpu60x0_GyroFullScale mpu60x0_GetGyroFullScale (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	Mpu60x0_GyroFullScale scale;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		scale = mpu->m_configs.m_gyroScale;
		break;

	case DATA_SRC_ACTIVE:
		scale = mpu->m_activeConfigs.m_gyroScale;
		break;

	case DATA_SRC_HW:
		scale = getGyroFullScale(mpu);
		break;
	}

END:
	EXIT();
	return scale;
}

retcode mpu60x0_ConvertGyroFsr2String(Mpu60x0_GyroFullScale fsr, char **pFsrStr) {

	ENTER();

	retcode retVal = 0;

	switch (fsr) {
	case GYRO_FULL_SCALE_250:
		strcpy(*pFsrStr, "250");
		break;

	case GYRO_FULL_SCALE_500:
		strcpy(*pFsrStr, "500");
		break;

	case GYRO_FULL_SCALE_1000:
		strcpy(*pFsrStr, "1000");
		break;

	case GYRO_FULL_SCALE_2000:
		strcpy(*pFsrStr, "2000");
		break;

	default:
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Mpu60x0_GyroFullScale mpu60x0_ParseGyroFsr (char *fsrStr) {

	ENTER();

	Mpu60x0_GyroFullScale fsr;

	if (strcmp(fsrStr, "250") == 0) {
		fsr = GYRO_FULL_SCALE_250;
	} else if (strcmp(fsrStr, "500") == 0) {
		fsr = GYRO_FULL_SCALE_500;
	} else if (strcmp(fsrStr, "1000") == 0) {
		fsr = GYRO_FULL_SCALE_1000;
	} else if (strcmp(fsrStr, "2000") == 0) {
		fsr = GYRO_FULL_SCALE_2000;
	} else {
		ERROR("Invalid Gyroscope FSR value %s", fsrStr);
		fsr = GYRO_FULL_SCALE_INVALID;
		goto END;
	}

END:
	EXIT();
	return fsr;
}


retcode mpu60x0_RegDataRdyCb (mpuHandle handle,
  		                      void *cb,
						      uint32 param) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}


	mpu->m_intDataRdyCb = cb;
	mpu->m_intDataRdyParam = param;

END:
	EXIT();
	return retVal;
}

retcode mpu60x0_RegFifoOvrflowCb (mpuHandle handle,
                                  void *cb,
							      uint32 param) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_intFifoOvlFlowCb = cb;
	mpu->m_intFifoOvlFlowParam = param;

END:
	EXIT();
	return retVal;
}

retcode mpu60x0_RegMotDetCb (mpuHandle handle,
                             void *cb,
						     uint32 param) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_intMotDetCb = cb;
	mpu->m_intMotDetParam = param;

END:
	EXIT();
	return retVal;
}

retcode mpu60x0_RegFsynchCb (mpuHandle handle,
                             void *cb,
		                     uint32 param) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_intFsyncCb = cb;
	mpu->m_intFsyncParam = param;

END:
	EXIT();
	return retVal;
}

retcode mpu60x0_RegAuxI2cCb (mpuHandle handle,
                             void *cb,
		                     uint32 param) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_intAuxI2cCb = cb;
	mpu->m_intAuxI2cParam = param;

END:
	EXIT();
	return retVal;
}

retcode mpu60x0_SetIntSignalLvl (mpuHandle handle,
		                         Mpu60x0_IntLvl lvl) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_intLvl = lvl;

END:
	EXIT();
	return retVal;
}

Mpu60x0_IntLvl mpu60x0_GetIntLvl (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	Mpu60x0_IntLvl lvl;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		lvl = mpu->m_configs.m_intLvl;
		break;

	case DATA_SRC_ACTIVE:
		lvl = mpu->m_activeConfigs.m_intLvl;
		break;

	case DATA_SRC_HW:
		lvl = getIntLvl(mpu);
		break;
	}

END:
	EXIT();
	return lvl;
}

retcode mpu60x0_ConvertIntLvl2String(Mpu60x0_IntLvl lvl, char **pLvlStr) {

	ENTER();

	retcode retVal = 0;

	switch (lvl) {
	case INT_LVL_NO_INT:
		strcpy(*pLvlStr, "No Interrupt Enabled");
		break;

	case INT_LVL_ACTIVE_LOW:
		strcpy(*pLvlStr, "Interrupt Level Active Low");
		break;

	case INT_LVL_ACTIVE_HIGH:
		strcpy(*pLvlStr, "Interrupt Level Active High");
		break;

	default:
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Mpu60x0_IntLvl mpu60x0_ParseIntLvl (char *lvlStr) {

	ENTER();

	Mpu60x0_IntLvl lvl;

	if (strcmp(lvlStr, "disabled") == 0) {
		lvl = INT_LVL_NO_INT;
	} else if (strcmp(lvlStr, "active-high") == 0) {
		lvl = INT_LVL_ACTIVE_HIGH;
	} else if (strcmp(lvlStr, "active-low") == 0) {
		lvl = INT_LVL_ACTIVE_LOW;
	} else {
		ERROR("Invalid Interrupt Level value %s", lvlStr);
		lvl = INT_LVL_INVALID;
		goto END;
	}

END:
	EXIT();
	return lvl;
}

retcode mpu60x0_SetIntOpenDrain (mpuHandle handle,
		                         bool isOpen) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_intIsOpenDrain = isOpen;

END:
	EXIT();
	return retVal;
}

bool mpu60x0_GetIntOpenDrain (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	bool isOpen;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		isOpen = mpu->m_configs.m_intIsOpenDrain;
		break;

	case DATA_SRC_ACTIVE:
		isOpen = mpu->m_activeConfigs.m_intIsOpenDrain;
		break;

	case DATA_SRC_HW:
		isOpen = getIsOpenDrainInt(mpu);
		break;
	}

END:
	EXIT();
	return isOpen;
}


retcode mpu60x0_SetIntLatch (mpuHandle handle,
		                     bool isLatch) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_intIsLatchOn = isLatch;

END:
	EXIT();
	return retVal;
}

bool mpu60x0_GetIntLatch (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	bool isLatch;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		isLatch = mpu->m_configs.m_intIsLatchOn;
		break;

	case DATA_SRC_ACTIVE:
		isLatch = mpu->m_activeConfigs.m_intIsLatchOn;
		break;

	case DATA_SRC_HW:
		isLatch = getIsLatchInt(mpu);
		break;
	}

END:
	EXIT();
	return isLatch;
}

retcode mpu60x0_SetIntClearOnRead (mpuHandle handle,
		                           bool clearOnRead) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_intClearOnAnyRead = clearOnRead;

END:
	EXIT();
	return retVal;
}


bool mpu60x0_GetIntClearOnRead (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	bool clearOnRead;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		clearOnRead = mpu->m_configs.m_intClearOnAnyRead;
		break;

	case DATA_SRC_ACTIVE:
		clearOnRead = mpu->m_activeConfigs.m_intClearOnAnyRead;
		break;

	case DATA_SRC_HW:
		clearOnRead = getClearOnReadInt(mpu);
		break;
	}

END:
	EXIT();
	return clearOnRead;
}


/**
 * Using FIFO
 */

retcode mpu60x0_SetFifoUsage (mpuHandle handle,
		                      const bool useFifo) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

    if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_fifoEnable = useFifo;

END:
	EXIT();
	return retVal;
}

bool mpu60x0_GetFifoUsage (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	bool fifo;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		fifo = mpu->m_configs.m_fifoEnable;
		break;

	case DATA_SRC_ACTIVE:
		fifo = mpu->m_activeConfigs.m_fifoEnable;
		break;

	case DATA_SRC_HW:
		fifo = getFifoUsage(mpu);
		break;
	}

END:
	EXIT();
	return fifo;
}


retcode mpu60x0_SetMotDetThr (mpuHandle handle,
							  bool isEnabled,
		                      uint8 thr) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_motDetEnable = isEnabled;
	mpu->m_configs.m_motDetThr = thr;

END:
	EXIT();
	return retVal;
}

uint8 mpu60x0_GetMotDetThr (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	uint8 thr;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		thr = mpu->m_configs.m_motDetThr;
		break;

	case DATA_SRC_ACTIVE:
		thr = mpu->m_activeConfigs.m_motDetThr;
		break;

	case DATA_SRC_HW:
		thr = getMotDetThr(mpu);
		break;
	}

END:
	EXIT();
	return thr;
}


/**
 * Enable/Disable Sensors
 * Use the enum SensorId as bit mask
 */
retcode mupu60x0_SetActiveSensors (mpuHandle handle,
		                           const uint8 sensors) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_activeSensors = sensors;

END:
	EXIT();
	return retVal;
}

uint8 mpu60x0_GetActiveSensors (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	uint8 sensors;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		sensors = mpu->m_configs.m_activeSensors;
		break;

	case DATA_SRC_ACTIVE:
		sensors = mpu->m_activeConfigs.m_activeSensors;
		break;

	case DATA_SRC_HW:
		sensors = getActiveSensors(mpu);
		break;
	}

END:
	EXIT();
	return sensors;
}


retcode mpu60x0_SetAuxI2c (mpuHandle handle,
		                   const Mpu60x0_AuxI2cMode mode) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_auxI2cMode = mode;

END:
	EXIT();
	return retVal;
}

Mpu60x0_AuxI2cMode mpu60x0_GetAuxI2c (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	Mpu60x0_AuxI2cMode mode;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		mode = mpu->m_configs.m_auxI2cMode;
		break;

	case DATA_SRC_ACTIVE:
		mode = mpu->m_activeConfigs.m_auxI2cMode;
		break;

	case DATA_SRC_HW:
		mode = getAuxI2cMode(mpu);
		break;
	}

END:
	EXIT();
	return mode;
}

retcode mpu60x0_ConvertAuxI2cMode2String(Mpu60x0_AuxI2cMode mode, char **pModeStr) {

	ENTER();

	retcode retVal = 0;

	switch (mode) {
	case AUX_I2C_DISABLE:
		strcpy(*pModeStr, "I2C Disabled");
		break;

	case AUX_I2C_BYPASS:
		strcpy(*pModeStr, "I2C Bypassed (the host processor is the bus master)");
		break;

	case AUX_I2C_MSTR:
		strcpy(*pModeStr, "I2C Master (the MPU acts as a I2C Bus Master)");
		break;

	default:
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}

Mpu60x0_AuxI2cMode mpu60x0_ParseAuxI2cMode (char *modeStr) {

	ENTER();

	Mpu60x0_AuxI2cMode mode;

	if (strcmp(modeStr, "disabled") == 0) {
		mode = AUX_I2C_DISABLE;
	} else if (strcmp(modeStr, "bypass") == 0) {
		mode = AUX_I2C_BYPASS;
	} else if (strcmp(modeStr, "master") == 0) {
		mode = AUX_I2C_MSTR;
	} else {
		ERROR("Invalid Auxiliary I2C Bus Mode value %s", modeStr);
		mode = AUX_I2C_INVALID;
		goto END;
	}

END:
	EXIT();
	return mode;
}


retcode mpu60x0_SetAuxI2cClk (mpuHandle handle,
		                      const Mpu60x0_AuxI2cClk clk) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_auxI2cClk = clk;

END:
	EXIT();
	return retVal;
}

Mpu60x0_AuxI2cClk mpu60x0_GetAuxI2cClk (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	Mpu60x0_AuxI2cClk clk;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		clk = mpu->m_configs.m_auxI2cClk;
		break;

	case DATA_SRC_ACTIVE:
		clk = mpu->m_activeConfigs.m_auxI2cClk;
		break;

	case DATA_SRC_HW:
		clk = getAuxI2cClk(mpu);
		break;
	}

END:
	EXIT();
	return clk;
}

retcode mpu60x0_ConvertAuxI2cClk2String(Mpu60x0_AuxI2cClk clk, char **pClkStr) {

	ENTER();

	retcode retVal = 0;

	switch (clk) {
	case AUX_I2C_CLK_258:
		strcpy(*pClkStr, "258 KHz");
		break;

	case AUX_I2C_CLK_267:
		strcpy(*pClkStr, "267 KHz");
		break;

	case AUX_I2C_CLK_276:
		strcpy(*pClkStr, "276 KHz");
		break;

	case AUX_I2C_CLK_286:
		strcpy(*pClkStr, "286 KHz");
		break;

	case AUX_I2C_CLK_296:
		strcpy(*pClkStr, "296 KHz");
		break;

	case AUX_I2C_CLK_308:
		strcpy(*pClkStr, "308 KHz");
		break;

	case AUX_I2C_CLK_320:
		strcpy(*pClkStr, "320 KHz");
		break;

	case AUX_I2C_CLK_333:
		strcpy(*pClkStr, "333 KHz");
		break;

	case AUX_I2C_CLK_348:
		strcpy(*pClkStr, "348 KHz");
		break;

	case AUX_I2C_CLK_364:
		strcpy(*pClkStr, "364 KHz");
		break;

	case AUX_I2C_CLK_381:
		strcpy(*pClkStr, "381 KHz");
		break;

	case AUX_I2C_CLK_400:
		strcpy(*pClkStr, "400 KHz");
		break;

	case AUX_I2C_CLK_421:
		strcpy(*pClkStr, "421 KHz");
		break;

	case AUX_I2C_CLK_444:
		strcpy(*pClkStr, "444 KHz");
		break;

	case AUX_I2C_CLK_471:
		strcpy(*pClkStr, "471 KHz");
		break;

	case AUX_I2C_CLK_500:
		strcpy(*pClkStr, "500 KHz");
		break;

	default:
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}


Mpu60x0_AuxI2cClk mpu60x0_ParseAuxI2cClk (char *clkStr) {

	ENTER();

	Mpu60x0_AuxI2cClk clk;

	if (strcmp(clkStr, "258") == 0) {
		clk = AUX_I2C_CLK_258;
	} else if (strcmp(clkStr, "267") == 0) {
		clk = AUX_I2C_CLK_267;
	} else if (strcmp(clkStr, "276") == 0) {
		clk = AUX_I2C_CLK_276;
	} else if (strcmp(clkStr, "286") == 0) {
		clk = AUX_I2C_CLK_286;
	} else if (strcmp(clkStr, "296") == 0) {
		clk = AUX_I2C_CLK_296;
	} else if (strcmp(clkStr, "308") == 0) {
		clk = AUX_I2C_CLK_308;
	} else if (strcmp(clkStr, "320") == 0) {
		clk = AUX_I2C_CLK_320;
	} else if (strcmp(clkStr, "333") == 0) {
		clk = AUX_I2C_CLK_333;
	} else if (strcmp(clkStr, "348") == 0) {
		clk = AUX_I2C_CLK_348;
	} else if (strcmp(clkStr, "364") == 0) {
		clk = AUX_I2C_CLK_364;
	} else if (strcmp(clkStr, "381") == 0) {
		clk = AUX_I2C_CLK_381;
	} else if (strcmp(clkStr, "400") == 0) {
		clk = AUX_I2C_CLK_400;
	} else if (strcmp(clkStr, "421") == 0) {
		clk = AUX_I2C_CLK_421;
	} else if (strcmp(clkStr, "444") == 0) {
		clk = AUX_I2C_CLK_444;
	} else if (strcmp(clkStr, "471") == 0) {
		clk = AUX_I2C_CLK_471;
	} else if (strcmp(clkStr, "500") == 0) {
		clk = AUX_I2C_CLK_500;
	} else {
		ERROR("Invalid Auxiliary I2C Bus Clock Speed value %s", clkStr);
		clk = AUX_I2C_CLK_INVALID;
		goto END;
	}

END:
	EXIT();
	return clk;
}




retcode mpu60x0_SetAuxI2cReducedSamplingFactor (mpuHandle handle,
		                                        const uint8 factor) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_auxI2cReducedSmplFactor = factor;

END:
	EXIT();
	return retVal;
}


uint8 mpu60x0_GetAuxI2cReducedSamplingFactor (mpuHandle handle, const ConfigDataSrc dataSrc) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	uint8 factor;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

 	switch (dataSrc) {
	case DATA_SRC_CONFIG:
		factor = mpu->m_configs.m_auxI2cReducedSmplFactor;
		break;

	case DATA_SRC_ACTIVE:
		factor = mpu->m_activeConfigs.m_auxI2cReducedSmplFactor;
		break;

	case DATA_SRC_HW:
		factor = getAuxI2cReducedSamplingFactor(mpu);
		break;
	}

END:
	EXIT();
	return factor;
}



retcode mpu60x0_SetFsync (mpuHandle handle,
		                  const Mpu60x0_SensorId id,
					      const Mpu60x0_IntLvl lvl) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	mpu->m_configs.m_fsyncSensorId = id;
	mpu->m_configs.m_fsyncIntLvl = lvl;

END:
	EXIT();
	return retVal;
}


slvHandle mpu60x0_AddSlv (mpuHandle handle,
		                  const uint8 id,
						  Mpu60x0_SlvConfig *config) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;

	if (!mpu) {
		NULL_POINTER("mpu");
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	if (id >= MPU60X0_SLV_COUNT) {
		ERROR("Invalid Slave Id %d", id);
		goto END;
	}

	if (mpu->m_configs.m_auxI2cSlv[id].m_slvIsEnabled) {
		WARN("Slave Already Exists.... will Perform a Modification");
	}

	Mpu60x0SlvInfo *slv = &mpu->m_configs.m_auxI2cSlv[id];
	//Now set the slave

	slv->m_slvIsEvengroup = config->m_isEvengroup;
	slv->m_slvIsRegNumDisable = config->m_isRegNumDisable;
	slv->m_slvIntEnable = config->m_intEnable;  //Only applicable for slv-4
	slv->m_slvIsSwap = config->m_isSwap;
	slv->m_slvI2cAddr = config->m_slvI2cAddr;
	slv->m_slvOper = config->m_slvOper;
	slv->m_slvRegCount = config->m_slvRegCount;
	slv->m_slvStartRegNum = config->m_slvStartRegNum;
	slv->m_slvUseReducedSampling = config->m_useReducedSampling;

END:
	EXIT();
	return (slvHandle)id;
}

retcode mpu60x0_RemSlv (mpuHandle handle,
		             slvHandle slv) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_state != MPU60X0_STATE_INIT) {
		WARN("MPU Started ....Changes will only take effect after a restart");
	}

	uint8 id = (uint8)slv;
	if (id >= MPU60X0_SLV_COUNT) {
		ERROR("Invalid Slave Id %d", id);
		retVal = -1;
		goto END;
	}

	if (!mpu->m_configs.m_auxI2cSlv[id].m_slvIsEnabled) {
		ERROR("Slave Does not Exists.... Exiting");
		retVal = -1;
		goto END;
	}

	//Now Remove the slave
	mpu->m_configs.m_auxI2cSlv[id].m_slvIsEnabled = FALSE;

END:
	EXIT();
	return retVal;
}


/**
 * This function will RESET the chip or part of it
 * The part that will be reset, will be based on the passed Reset Mode
 */
retcode mpu60x0_Reset (mpuHandle handle, const Mpu60x0_ResetMode mode) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;
	uint8 val;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	switch (mode) {
	case RESET_MODE_ALL:
		//This should bring the chip to power up condition
		//This results in putting the chip in sleep mode
		val = readReg(mpu, MPU60X0_REG_PWR_MGMT_1);
		val |= MPU60X0_FIELD_DEVICE_RESET;

		retVal = writeReg(mpu, MPU60X0_REG_PWR_MGMT_1, val);
		if (retVal) {
			ERROR("Failed to write in Register PWR_MGMT_1");
			goto END;
		}


		//We need also to reset our internal data structures

		setRegDefault(mpu, &mpu->m_configs);
		setRegDefault(mpu, &mpu->m_activeConfigs);

		//Also reset the Sensor Data Registers
		resetSensorDataRegs(mpu);

        //Init other info to defaults
		resetMpuData(mpu);
		break;

	case RESET_MODE_SIGNAL_PATH:  //For all the sensors, does not reset the sensor register values
		val = readReg(mpu, MPU60X0_REG_SIGNAL_PATH_RESET);
		val |= MPU60X0_FIELD_TEMP_RESET;
		val |= MPU60X0_FIELD_ACCEL_RESET;
		val |= MPU60X0_FIELD_GYRO_RESET;

		retVal = writeReg(mpu, MPU60X0_REG_SIGNAL_PATH_RESET, val);
		if (retVal) {
			ERROR("Failed to write in Register SIGNAL_PATH_RESET");
			goto END;
		}


		//No effect to the register values, and hence internal data structures
		break;

	case RESET_MODE_REG:  //This also reset the sensor data registers
		val = readReg(mpu, MPU60X0_REG_USER_CTRL);
		val |= MPU60X0_FIELD_SIG_COND_RESET;

		retVal = writeReg(mpu, MPU60X0_REG_USER_CTRL, val);
		if (retVal) {
			ERROR("Failed to write in Register USER_CTRL");
			goto END;
		}


		//Reseting the internal data structure for sensor data register
		retVal = resetSensorDataRegs(mpu);
		if (retVal) {
			ERROR("Failed to Reset Sensor Data Registers");
			goto END;
		}

		break;

	case RESET_MODE_FIFO:
		val = readReg(mpu, MPU60X0_REG_USER_CTRL);
		val |= MPU60X0_FIELD_FIFO_RESET;
		retVal = writeReg(mpu, MPU60X0_REG_USER_CTRL, val);
		if (retVal) {
			ERROR("Failed to write in Register USER_CTRL");
			goto END;
		}

		//No data structures associated
		break;

	case RESET_AUX_I2C:
		val = readReg(mpu, MPU60X0_REG_USER_CTRL);
		val |= MPU60X0_FIELD_I2C_MST_RESET;
		retVal = writeReg(mpu, MPU60X0_REG_USER_CTRL, val);
		if (retVal) {
			ERROR("Failed to write in Register USER_CTRL Data");
			goto END;
		}

		//No data structures associated
		break;

	default:
		ERROR("Invalid Reset Mode");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}


Mpu60x0_ResetMode mpu60x0_ParseResetMode (char* modeStr) {
	ENTER();

	Mpu60x0_ResetMode mode;

	//Get the reset Mode
	if (modeStr != NULL) {
		if (strcmp(modeStr, "all")) {
			mode = RESET_MODE_ALL;
		} else if (strcmp(modeStr, "fifo")) {
			mode = RESET_MODE_FIFO;
		} else if (strcmp(modeStr, "i2c")) {
			mode = RESET_AUX_I2C;
		} else if (strcmp(modeStr, "reg")) {
			mode = RESET_MODE_REG;
		} else if (strcmp(modeStr, "signal-path")) {
			mode = RESET_MODE_SIGNAL_PATH;
		} else {
			ERROR("Invalid Reset Mode %s", modeStr);
			mode = RESET_MODE_INVALID;
		}
	} else {
		ERROR("No Reset Mode Specified...");
		mode = RESET_MODE_INVALID;
	}

	EXIT();
	return mode;
}


/**
 * This function performs a read for all sensor data in a synchronous manner
 * This guarantees that all sensor data are read in one shot
 * Reading can happen from either the sensor registers or
 * The Fifo depending on whether the fifo is Enabled or not
 *
 * The function puts the data in an internal data array,
 * The caller can request any specific sensor data in separate funciton calls
 */
retcode mpu60x0_ReadSensorData (mpuHandle handle) {

	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (mpu->m_activeConfigs.m_fifoEnable) {
		retVal = readSensroDataFromFifo(mpu);
	} else {
		retVal = readSensorData(mpu);
	}
	if (retVal) {
		ERROR("Failed to Read Sensor Data");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

/**
 * Get functions for Accelerometer/Gyroscope/Temperature data
 * The returned data will be scaled based on the scaling factor
 * This does not do any read from the Hardware,
 * instead, it just parses the internal data array
 * You need to call mpu60x0_ReadSensorData() first to fill the array
 * if fresh sensor data are required
 */
retcode mpu60x0_GetAccData (mpuHandle handle, float *x, float *y, float *z) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	retVal = getAccData(mpu, x, y, z);
	if (retVal) {
		ERROR("Failed to Get Accelerometer Data");
		goto END;
	}

END:
	EXIT();
	return retVal;
}


retcode mpu60x0_GetGyroData (mpuHandle handle, float *x, float *y, float *z) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	retVal = getGyroData(mpu, x, y, y);
	if (retVal) {
		ERROR("Failed to Get Gyro Data");
		goto END;
	}

END:
	EXIT();
	return retVal;
}


retcode mpu60x0_GetTempData (mpuHandle handle, float *temp) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

    retVal = getTempData(mpu, temp);
	if (retVal) {
		ERROR("Failed to Get Temperature Data");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

/**
 * Cycle for Low Power Accelerometer only
 * This function takes effect immediately
 */
retcode mpu60x0_SetCycleMode(mpuHandle handle,
		                     bool cycle,
							 Mpu60x0_LowPwrWakeCtrlFreq freq,
							 uint8 accOnDelay) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	mpu->m_configs.m_pwrMgmtAccOnDelay = accOnDelay;

	retVal = updateReg_MOT_DETECT_CTRL(mpu);
	if (retVal) {
		ERROR("Failed to update the MOT_DETECT_CTRL register");
		goto END;
	}

	mpu->m_configs.m_pwrMgmtLowPwrWakeCtrl = freq;

	retVal = updateReg_PWR_MGMT_2(mpu);
	if (retVal) {
		ERROR("Failed to update the PWR_MGMT_2 register");
		goto END;
	}

	if (cycle) {
		mpu->m_operMode = MPU60X0_OPER_MODE_CYCLE;

		retVal = updateReg_PWR_MGMT_1(mpu);
		if (retVal) {
			ERROR("Failed to update the PWR_MGMT_1 register to switch in Cycle Mode");
			goto END;
		}
	}

END:
	EXIT();
	return retVal;

}

/**
 * This function tests the connectivity to the chip
 */
bool mpu60x0_TestConnectivity (mpuHandle handle) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	bool result;
	uint8 val;

	if (!mpu) {
		NULL_POINTER("mpu");
		result = FALSE;
		goto END;
	}

	val = readReg(mpu, MPU60X0_REG_WHO_AM_I);

	if (val == MPU60X0_REG_WHO_AM_I_VAL) {
		PROGRESS("PASSED The Connectivity Test");
		result = TRUE;
	} else {
		PROGRESS("FAILED The Connectivity Test");
		INFO("Expected %d but Found %d", MPU60X0_REG_WHO_AM_I_VAL, val);
		result = FALSE;
	}

END:
	EXIT();
	return result;
}




/**
 * Perform Self-Test
 */


bool mpu60x0_PerformSelfTest (mpuHandle handle,
		                      const uint8 sensors) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	bool result = TRUE;
	Mpu60x0_AccFullScale originalAccScale;
	Mpu60x0_GyroFullScale originalGyroScale;

	float xa, ya, za;
	float xg, yg, zg;
	float xa_st, ya_st, za_st;
	float xg_st, yg_st, zg_st;

	//This command will need to be issued from Sleep mode (not a chip restriction, just to make things simpler)
	if (mpu->m_state != MPU60X0_STATE_INIT) {
		ERROR("Chip needs to be in SLEEP Mode to start this operation .... Exiting");
		result = FALSE;
		goto END;
	}

	//First check if the required sensors are enabled
	if ((mpu->m_configs.m_activeSensors & sensors) != sensors) {
		ERROR("Some requested sensors are not enabled, need to enable these sensors first...Exiting");
		result = FALSE;
		goto END;
	}

	//Memorize the current scale range for Accelerometer and Gyroscope
	originalAccScale = mpu->m_configs.m_accScale;
	originalGyroScale = mpu->m_configs.m_gyroScale;

	//Set the proper full scale range for self test
	mpu->m_configs.m_accScale = ACC_FULL_SCALE_8G;
	mpu->m_configs.m_gyroScale = GYRO_FULL_SCALE_250;

	//Wake up the Chip
	retVal = startChip(mpu);
	if (retVal) {
		ERROR("Failed to Start the chip in no test mode");
		result = FALSE;
		goto END;
	}


	//Wait for some time
	usleep(1000000);

	//Read data with no self test mode
	retVal = readSensorData(mpu);
	if (retVal) {
		ERROR("Failed to Read Sensor data in no test mode");
		result = FALSE;
		goto END;
	}

	getAccData(mpu, &xa, &ya, &za);
	getGyroData(mpu, &xg, &yg, &zg);

	//Now put the Chip back in Sleep mode
	retVal = stopChip(mpu);
	if (retVal) {
		ERROR("Failed to Stop the chip in no test mode");
		result = FALSE;
		goto END;
	}


	//Now set the Self test bits
	mpu->m_configs.m_selfTestSensors = sensors;

	//Awake the chip again
	retVal = startChip(mpu);
	if (retVal) {
		ERROR("Failed to Start the chip in test mode");
		result = FALSE;
		goto END;
	}


	//Wait some time
	usleep(1000000);

	//Read data with self test mode
	retVal = readSensorData(mpu);
	if (retVal) {
		ERROR("Failed to Read Sensor data in test mode");
		result = FALSE;
		goto END;
	}

	getAccData(mpu, &xa_st, &ya_st, &za_st);
	getGyroData(mpu, &xg_st, &yg_st, &zg_st);

	//Put the chip to sleep
	retVal = stopChip(mpu);
	if (retVal) {
		ERROR("Failed to Stop the chip in test mode");
		result = FALSE;
		goto END;
	}


	//Remove self test bits
	mpu->m_configs.m_selfTestSensors = SENSOR_NONE;

	//Revert to original scale ranges
	mpu->m_configs.m_accScale = originalAccScale;
	mpu->m_configs.m_gyroScale = originalGyroScale;

	//Calculate Results
	float delta;

	//Accelerometer X-axis
	if (sensors & SENSOR_ACC_X) {
		delta = xa_st - xa;

		result = calcSelfTestAccX(mpu, delta);
		if (!result) {
			ERROR ("Failed Test for Acc_X");
			goto END;
		}
	}

	if (sensors & SENSOR_ACC_Y) {
		delta = xa_st - xa;

		result = calcSelfTestAccY(mpu, delta);
		if (!result) {
			ERROR ("Failed Test for Acc_Y");
			goto END;
		}
	}

	if (sensors & SENSOR_ACC_Z) {
		delta = xa_st - xa;

		result = calcSelfTestAccZ(mpu, delta);
		if (!result) {
			ERROR ("Failed Test for Acc_Z");
			goto END;
		}
	}

	//Gyroscope
	if (sensors & SENSOR_GYRO_X) {
		delta = xg_st - xg;

		result = calcSelfTestGyroX(mpu, delta);
		if (!result) {
			ERROR ("Failed Test for Gyro_X");
			goto END;
		}
	}

	if (sensors & SENSOR_GYRO_Y) {
		delta = yg_st - yg;

		result = calcSelfTestGyroY(mpu, delta);
		if (!result) {
			ERROR ("Failed Test for Gyro_Y");
			goto END;
		}
	}

	if (sensors & SENSOR_GYRO_Z) {
		delta = zg_st - zg;

		result = calcSelfTestGyroZ(mpu, delta);
		if (!result) {
			ERROR ("Failed Test for Gyro_Z");
			goto END;
		}
	}

END:
	EXIT();
	return result;
}

uint8 mpu60x0_GetSlvRegData (mpuHandle handle,
		                     slvHandle slvId,
							 uint8 *data) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	uint8 count;
	uint8 startOffset;

	if (!mpu) {
		NULL_POINTER("mpu");
		count = 0;
		goto END;
	}

	if (! mpu->m_activeConfigs.m_auxI2cSlv[slvId].m_slvIsEnabled) {
		ERROR("Can not Read SLV[%d] data, Slave is not enabled", slvId);
		count = 0;
		goto END;
	}

	if (mpu->m_activeConfigs.m_auxI2cSlv[slvId].m_slvOper != ACCESS_OPER_READ) {
		ERROR("Can not Read SLV[%d] data, Slave is not set for Read Operation", slvId);
		count = 0;
		goto END;
	}

	switch (slvId) {
	case 0:
	case 1:
	case 2:
	case 3:
		count = mpu->m_activeConfigs.m_auxI2cSlv[slvId].m_slvRegCount;
		startOffset = mpu->m_activeConfigs.m_auxI2cSlv[slvId].m_slvExtSensorRegOffset;

		memcpy(data, &mpu->m_sensorData[startOffset], count * sizeof(uint8));
		break;

	case 4:
		count = 1;
		data[0] = readReg(mpu, MPU60X0_REG_I2C_SLV4_DI);
		break;

	default:
		ERROR("Invalid SlvId: %d", slvId);
		count = 0;
		goto END;
		break;
	}

END:
	EXIT();
	return count;
}

retcode mpu60x0_SetSlvRegData (mpuHandle handle,
		                       slvHandle slvId,
					   		   const uint8 val) {
	ENTER();

	Mpu60x0 *mpu = (Mpu60x0 *)handle;
	retcode retVal = 0;

	if (!mpu) {
		NULL_POINTER("mpu");
		retVal = -1;
		goto END;
	}

	if (val > 255){
		ERROR("Invalid value to set in a 8 bit Register: %d", val);
		retVal = -1;
		goto END;
	}

	switch (slvId) {
	case 0:
		retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV0_DO, val);
    	if (retVal) {
    		ERROR("Failed to Write in I2C_SLV0_DO Register");
    		goto END;
    	}
		break;

	case 1:
		writeReg(mpu, MPU60X0_REG_I2C_SLV1_DO, val);
    	if (retVal) {
    		ERROR("Failed to Write in I2C_SLV1_DO Register");
    		goto END;
    	}
		break;

	case 2:
		writeReg(mpu, MPU60X0_REG_I2C_SLV2_DO, val);
    	if (retVal) {
    		ERROR("Failed to Write in I2C_SLV2_DO Register");
    		goto END;
    	}
		break;

	case 3:
		writeReg(mpu, MPU60X0_REG_I2C_SLV3_DO, val);
    	if (retVal) {
    		ERROR("Failed to Write in I2C_SLV3_DO Register");
    		goto END;
    	}
		break;

	case 4:
		retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV4_DO, val);
    	if (retVal) {
    		ERROR("Failed to Write in I2C_SLV4_DO Register");
    		goto END;
    	}

		break;

	default:
		ERROR("Invalid Slave ID %d", slvId);
		retVal = -1;
		goto END;
	}

END:
	EXIT();
	return retVal;
}



/**
 * Internal Functions
 */
static retcode updateReg_SMPLRT_DIV (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;

	if (config->m_samplingDiv != activeConfig->m_samplingDiv){

		retVal = writeReg(mpu, MPU60X0_REG_SMPLRT_DIV, config->m_samplingDiv);
    	if (retVal) {
    		ERROR("Failed to Write in SMPLRT Register");
    		goto END;
    	}

		activeConfig->m_samplingDiv = config->m_samplingDiv;
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_CONFIG (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if ((config->m_dlpf != activeConfig->m_dlpf) ||
			(config->m_fsyncSensorId != activeConfig->m_fsyncSensorId)) {

        regVal |= getFsyncExtEncoding(config->m_fsyncSensorId);
		regVal |= getDlpfEncoding(config->m_dlpf);

		retVal = writeReg(mpu, MPU60X0_REG_CONFIG,regVal);
    	if (retVal) {
    		ERROR("Failed to Write in CONFIG Register");
    		goto END;
    	}

		activeConfig->m_dlpf = config->m_dlpf;
		activeConfig->m_fsyncSensorId = config->m_fsyncSensorId;
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_GYRO_CONFIG (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if ((config->m_gyroScale != activeConfig->m_gyroScale) ||
			((config->m_selfTestSensors & SENSOR_GYRO_ALL) !=
					(activeConfig->m_selfTestSensors & SENSOR_GYRO_ALL))) {

		regVal |= getGyroScaleEncoding(config->m_gyroScale);

		if (config->m_selfTestSensors & SENSOR_GYRO_X) {
			regVal |= MPU60X0_FIELD_XG_ST;
		}
		if (config->m_selfTestSensors & SENSOR_GYRO_Y) {
			regVal |= MPU60X0_FIELD_YG_ST;
		}
		if (config->m_selfTestSensors & SENSOR_GYRO_Z) {
			regVal |= MPU60X0_FIELD_ZG_ST;
		}

		retVal = writeReg(mpu, MPU60X0_REG_GYRO_CONFIG, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in GYRO_CONFIG Register");
    		goto END;
    	}


		activeConfig->m_gyroScale = config->m_gyroScale;
		activeConfig->m_selfTestSensors &= (!SENSOR_GYRO_ALL);
		activeConfig->m_selfTestSensors |= (config->m_selfTestSensors & SENSOR_GYRO_ALL);
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_ACCEL_CONFIG (Mpu60x0 *mpu) {
	ENTER();
	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if ((config->m_accScale != activeConfig->m_accScale) ||
			((config->m_selfTestSensors & SENSOR_ACC_ALL) !=
					(activeConfig->m_selfTestSensors & SENSOR_ACC_ALL))) {

		regVal |= getAccScaleEncoding(config->m_accScale);

		if (config->m_selfTestSensors & SENSOR_ACC_X) {
			regVal |= MPU60X0_FIELD_XA_ST;
		}
		if (config->m_selfTestSensors & SENSOR_ACC_Y) {
			regVal |= MPU60X0_FIELD_YA_ST;
		}
		if (config->m_selfTestSensors & SENSOR_ACC_Z) {
			regVal |= MPU60X0_FIELD_ZA_ST;
		}

		retVal = writeReg(mpu, MPU60X0_REG_ACCEL_CONFIG, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in ACCEL_CONFIG Register");
    		goto END;
    	}

		activeConfig->m_accScale = config->m_accScale;
		activeConfig->m_selfTestSensors &= (!SENSOR_ACC_ALL);
		activeConfig->m_selfTestSensors |= (config->m_selfTestSensors & SENSOR_ACC_ALL);
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_MOT_THR (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if (config->m_motDetThr != activeConfig->m_motDetThr) {
		regVal = (uint8)(config->m_motDetThr);

		retVal = writeReg(mpu, MPU60X0_REG_MOT_THR, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in MOT_THR Register");
    		goto END;
    	}

		activeConfig->m_motDetThr = config->m_motDetThr;
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_FIFO_EN (Mpu60x0 *mpu) {
	ENTER();
	Mpu60x0Config *config = &mpu->m_configs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if (config->m_fifoEnable) {
		regVal = 0;

		if (config->m_activeSensors | SENSOR_TEMP) {
			regVal |= MPU60X0_FIELD_TEMP_FIFO_EN;
		}
		if (config->m_activeSensors | SENSOR_GYRO_X) {
			regVal |= MPU60X0_FIELD_XG_FIFO_EN;
		}
		if (config->m_activeSensors | SENSOR_GYRO_Y) {
			regVal |= MPU60X0_FIELD_YG_FIFO_EN;
		}
		if (config->m_activeSensors | SENSOR_GYRO_Z) {
			regVal |= MPU60X0_FIELD_ZG_FIFO_EN;
		}
		if (config->m_activeSensors | SENSOR_ACC_ALL) {
			regVal |= MPU60X0_FIELD_ACCEL_FIFO_EN;
		}

		if (config->m_auxI2cMode == AUX_I2C_MSTR) {
			if (config->m_auxI2cSlv[2].m_slvIsEnabled) {
				regVal |= MPU60X0_FIELD_SLV2_FIFO_EN;
			}
			if (config->m_auxI2cSlv[1].m_slvIsEnabled) {
				regVal |= MPU60X0_FIELD_SLV1_FIFO_EN;
			}
			if (config->m_auxI2cSlv[0].m_slvIsEnabled) {
				regVal |= MPU60X0_FIELD_SLV0_FIFO_EN;
			}
		}

		retVal = writeReg(mpu, MPU60X0_REG_FIFO_EN, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in FIFO_EN Register");
    		goto END;
    	}

		//No update to active configuration yet
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_I2C_MST_CTRL (Mpu60x0 *mpu) {
	ENTER();
	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if (config->m_auxI2cMode == AUX_I2C_MSTR) {
		//Always leave Multi-master disabled (not supported yet)
		//MUL_MST_EN = 0

		//Always wait for external sensor data
		//WAIT_FOR_ES = 1
		regVal |= MPU60X0_FIELD_WAIT_FOR_ES;

		if ((config->m_fifoEnable) &&
				(config->m_auxI2cSlv[3].m_slvIsEnabled)) {
			//SLV3_FIFO_EN
			regVal |= MPU60X0_FIELD_SLV3_FIFO_EN;
		}

		//For Now we will leave a restart between read of slaves
		//I2C_MST_P_NSR = 0

		regVal |= getAuxI2cClkEncoding(config->m_auxI2cClk);


		retVal = writeReg(mpu, MPU60X0_REG_I2C_MST_CTRL, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in I2C_MST_CTRL Register");
    		goto END;
    	}

		activeConfig->m_auxI2cClk = config->m_auxI2cClk;
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_I2C_SLVn_xxx (Mpu60x0 *mpu, const uint8 slv) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if (config->m_auxI2cMode == AUX_I2C_MSTR) {
		if ((!config->m_auxI2cSlv[slv].m_slvIsEnabled) &&
				(!activeConfig->m_auxI2cSlv[slv].m_slvIsEnabled)) { //Was and still disabled
			//No Action
		} else if ((!config->m_auxI2cSlv[slv].m_slvIsEnabled) &&
				(activeConfig->m_auxI2cSlv[slv].m_slvIsEnabled)) { //Was Enabled and now is Disabled
			//Need to disable SLV-n
			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV0_CTRL + (3 * slv),0);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLV0_CTRL Register");
	    		goto END;
	    	}

			activeConfig->m_auxI2cSlv[slv].m_slvIsEnabled = FALSE;
		} else {  //Enabled now (no matter what it was before)
			if (config->m_auxI2cSlv[slv].m_slvOper == ACCESS_OPER_READ) {
				regVal |= MPU60X0_FIELD_I2C_SLVn_RW;
				activeConfig->m_auxI2cSlv[slv].m_slvExtSensorRegOffset = mpu->m_sensorRegExtCount;
				mpu->m_sensorRegExtCount += config->m_auxI2cSlv[slv].m_slvRegCount;
			}

			regVal |= config->m_auxI2cSlv[slv].m_slvI2cAddr;
			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV0_ADDR + (3 * slv), regVal);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLVn_ADDR Register");
	    		goto END;
	    	}



			regVal = config->m_auxI2cSlv[slv].m_slvStartRegNum;
			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV0_REG + (3 * slv), regVal);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLVn_REG Register");
	    		goto END;
	    	}


			regVal = MPU60X0_FIELD_I2C_SLVn_EN;

			if (config->m_auxI2cSlv[slv].m_slvIsSwap) {
				regVal |=  MPU60X0_FIELD_I2C_SLVn_BYTE_SW;
			}

			if(config->m_auxI2cSlv[slv].m_slvIsRegNumDisable) {
				regVal |= MPU60X0_FIELD_I2C_SLVn_REG_DIS;
			}

			if (config->m_auxI2cSlv[slv].m_slvIsEvengroup) {
				regVal |=  MPU60X0_FIELD_I2C_SLVn_GRP;
			}

			regVal |= config->m_auxI2cSlv[slv].m_slvRegCount;

			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV0_CTRL + (3 * slv), regVal);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLVn_CTRL Register");
	    		goto END;
	    	}

			activeConfig->m_auxI2cSlv[slv].m_slvIsEnabled = TRUE;
			activeConfig->m_auxI2cSlv[slv].m_slvIsEvengroup = config->m_auxI2cSlv[slv].m_slvIsEvengroup;
			activeConfig->m_auxI2cSlv[slv].m_slvIsRegNumDisable = config->m_auxI2cSlv[slv].m_slvIsRegNumDisable;
			activeConfig->m_auxI2cSlv[slv].m_slvIsSwap = config->m_auxI2cSlv[slv].m_slvIsSwap;
			activeConfig->m_auxI2cSlv[slv].m_slvI2cAddr = config->m_auxI2cSlv[slv].m_slvI2cAddr;
			activeConfig->m_auxI2cSlv[slv].m_slvOper = config->m_auxI2cSlv[slv].m_slvOper;
			activeConfig->m_auxI2cSlv[slv].m_slvRegCount = config->m_auxI2cSlv[slv].m_slvRegCount;
			activeConfig->m_auxI2cSlv[slv].m_slvStartRegNum = config->m_auxI2cSlv[slv].m_slvStartRegNum;
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode updateReg_I2C_SLV4_xxx (Mpu60x0 *mpu) {
	ENTER();
	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

	if (config->m_auxI2cMode == AUX_I2C_MSTR) {

		if (!config->m_auxI2cSlv[4].m_slvIsEnabled) {
			regVal = config->m_auxI2cReducedSmplFactor;
			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV4_CTRL, regVal);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLV4_CTRL Register");
	    		goto END;
	    	}

			activeConfig->m_auxI2cReducedSmplFactor = config->m_auxI2cReducedSmplFactor;
			activeConfig->m_auxI2cSlv[4].m_slvIsEnabled = FALSE;
		} else {  //Enabled
			if (config->m_auxI2cSlv[4].m_slvOper == ACCESS_OPER_READ) {
				regVal = MPU60X0_FIELD_I2C_SLV4_RW;
			} else {
				regVal = 0;
			}

			regVal |= config->m_auxI2cSlv[4].m_slvI2cAddr;
			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV4_ADDR, regVal);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLV4_ADDR Register");
	    		goto END;
	    	}

			regVal = config->m_auxI2cSlv[4].m_slvStartRegNum;
			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV4_REG, regVal);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLV4_REG Register");
	    		goto END;
	    	}


	    	regVal = 0;
			regVal |= MPU60X0_FIELD_I2C_SLV4_EN;

			if (config->m_auxI2cSlv[4].m_slvIntEnable) {
				regVal |= MPU60X0_FIELD_I2C_SLV4_INT_EN;
			}

			if (config->m_auxI2cSlv[4].m_slvIsRegNumDisable) {
				regVal |= MPU60X0_FIELD_I2C_SLV4_REG_DIS;
			}


			regVal |= config->m_auxI2cReducedSmplFactor;

			retVal = writeReg(mpu, MPU60X0_REG_I2C_SLV4_CTRL, regVal);
	    	if (retVal) {
	    		ERROR("Failed to Write in I2C_SLV4_CTRL Register");
	    		goto END;
	    	}


			activeConfig->m_auxI2cSlv[4].m_slvIsEnabled = TRUE;
			activeConfig->m_auxI2cSlv[4].m_slvOper = config->m_auxI2cSlv[4].m_slvOper;
			activeConfig->m_auxI2cSlv[4].m_slvI2cAddr = config->m_auxI2cSlv[4].m_slvI2cAddr;
			activeConfig->m_auxI2cSlv[4].m_slvStartRegNum = config->m_auxI2cSlv[4].m_slvStartRegNum;
			activeConfig->m_auxI2cSlv[4].m_slvIntEnable = config->m_auxI2cSlv[4].m_slvIntEnable;
			activeConfig->m_auxI2cSlv[4].m_slvIsRegNumDisable = config->m_auxI2cSlv[4].m_slvIsRegNumDisable;
			activeConfig->m_auxI2cReducedSmplFactor = config->m_auxI2cReducedSmplFactor;
		}
	}

END:
	EXIT();
	return retVal;
}


static retcode updateReg_INT_PIN_CFG (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

    if (config->m_intLvl == INT_LVL_ACTIVE_LOW) {
    	regVal |= MPU60X0_FIELD_INT_LEVEL;
    }
    if (config->m_intIsOpenDrain) {
    	regVal |= MPU60X0_FIELD_INT_OPEN;
    }
    if (config->m_intIsLatchOn) {
    	regVal |= MPU60X0_FIELD_LATCH_INT_EN;
    }
    if (config->m_intClearOnAnyRead) {
    	regVal |= MPU60X0_FIELD_INT_RD_CLEAR;
    }
    if (config->m_fsyncIntLvl == INT_LVL_ACTIVE_LOW) {
    	regVal |= MPU60X0_FIELD_FSYNC_INT_LEVEL;
    }
    if (config->m_fsyncIntLvl != INT_LVL_NO_INT) {
    	regVal |= MPU60X0_FIELD_FSYNC_INT_EN;
    }
    if (config->m_auxI2cMode == AUX_I2C_BYPASS) {
    	regVal |= MPU60X0_FIELD_I2C_BYPASS_EN;
    }

	retVal = writeReg(mpu, MPU60X0_REG_INT_PIN_CFG, regVal);
	if (retVal) {
		ERROR("Failed to Write in INT_PIN_CFG Register");
		goto END;
	}


	activeConfig->m_auxI2cMode = config->m_auxI2cMode;
	activeConfig->m_fsyncIntLvl = config->m_fsyncIntLvl;
	activeConfig->m_intClearOnAnyRead = config->m_intClearOnAnyRead;
	activeConfig->m_intIsLatchOn = config->m_intIsLatchOn;
	activeConfig->m_intIsOpenDrain = config->m_intIsOpenDrain;
	activeConfig->m_intLvl = config->m_intLvl;

END:
	EXIT();
	return retVal;
}


static retcode updateReg_INT_ENABLE (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

    if ((config->m_intLvl == INT_LVL_NO_INT) &&
    		(activeConfig->m_intLvl != INT_LVL_NO_INT)) {  //Disabling Interrupts
    	retVal = writeReg(mpu, MPU60X0_REG_INT_ENABLE, 0);
    	if (retVal) {
    		ERROR("Failed to Write in INT_ENABLE Register");
    		goto END;
    	}

    	activeConfig->m_intLvl = INT_LVL_NO_INT;
    } else if (config->m_intLvl != INT_LVL_NO_INT) {
    	regVal = 1;

    	if (config->m_auxI2cMode == AUX_I2C_MSTR) {
    		regVal |= MPU60X0_FIELD_I2C_MST_INT_EN;
    	}
    	if (config->m_fifoEnable) {
    		regVal |= MPU60X0_FIELD_FIFO_OFLOW_EN;
    	}
    	if (config->m_motDetEnable) {
    		regVal |= MPU60X0_FIELD_MOT_EN;
    	}

    	retVal = writeReg(mpu, MPU60X0_REG_INT_ENABLE, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in INT_ENABLE Register");
    		goto END;
    	}

    	activeConfig->m_motDetEnable = config->m_motDetEnable;
    }

END:
	EXIT();
	return retVal;
}

static retcode updateReg_I2C_MST_DELAY_CTRL (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
	retcode retVal = 0;
    uint8 regVal = 0;

    if (config->m_auxI2cMode == AUX_I2C_MSTR) {
    	for (uint8 i = 0; i < 5; i++) {
    		if (config->m_auxI2cSlv[i].m_slvIsEnabled && config->m_auxI2cSlv[i].m_slvUseReducedSampling) {
    			regVal |= 1 << i;
    		}
    	}

    	if (config->m_auxI2cdelayEsShadow) {
    		regVal |= MPU60X0_FIELD_DELAY_ES_SHADOW;
    	}

    	retVal = writeReg(mpu, MPU60X0_REG_I2C_MST_DELAY_CTRL, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in I2C_MST_DELAY_CTRL Register");
    		goto END;
    	}

    	activeConfig->m_auxI2cdelayEsShadow = config->m_auxI2cdelayEsShadow;
    	for (uint8 i = 0; i < 5; i++) {
    		if (config->m_auxI2cSlv[i].m_slvIsEnabled) {
    			activeConfig->m_auxI2cSlv[i].m_slvUseReducedSampling = config->m_auxI2cSlv[i].m_slvUseReducedSampling;
    		}
    	}
    }

END:
	EXIT();
	return retVal;
}

static retcode updateReg_MOT_DETECT_CTRL (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
    retcode retVal = 0;
    uint8 regVal = 0;

    if (config->m_pwrMgmtAccOnDelay != activeConfig->m_pwrMgmtAccOnDelay) {

    	regVal = config->m_pwrMgmtAccOnDelay << 4;
    	retVal = writeReg(mpu, MPU60X0_REG_MOT_DET_CTRL, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in MOT_DET_CTRL Register");
    		goto END;
    	}

    	activeConfig->m_pwrMgmtAccOnDelay = config->m_pwrMgmtAccOnDelay;
    }

END:
	EXIT();
	return retVal;
}


static retcode updateReg_USER_CTRL (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
    retcode retVal = 0;
    uint8 regVal = 0;

    if ((config->m_auxI2cMode != activeConfig->m_auxI2cMode) ||
    		(config->m_fifoEnable != activeConfig->m_fifoEnable)) {
    	if (config->m_auxI2cMode == AUX_I2C_MSTR) {
    		regVal |= MPU60X0_FIELD_I2C_MST_EN;
    	}
    	if (config->m_fifoEnable) {
    		regVal |= MPU60X0_FIELD_FIFO_EN;
    	}

    	retVal = writeReg(mpu, MPU60X0_REG_USER_CTRL, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in USER_CTRL Register");
    		goto END;
    	}

    	activeConfig->m_auxI2cMode = config->m_auxI2cMode;
    	activeConfig->m_fifoEnable = config->m_fifoEnable;
    }

END:
	EXIT();
	return retVal;
}

static retcode updateReg_PWR_MGMT_2 (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
    retcode retVal = 0;
    uint8 regVal = 0;
    uint8 lp;

    if ((config->m_activeSensors != activeConfig->m_activeSensors) ||
    		(config->m_pwrMgmtLowPwrWakeCtrl != activeConfig->m_pwrMgmtLowPwrWakeCtrl)) {
    	mpu->m_activeSensorCount = 7;

    	if (!(config->m_activeSensors & SENSOR_GYRO_Z)) {
    		regVal |= MPU60X0_FIELD_STBY_ZG;
    		mpu->m_activeSensorCount --;
    	}
    	if (!(config->m_activeSensors & SENSOR_GYRO_Y)) {
    		regVal |= MPU60X0_FIELD_STBY_YG;
    		mpu->m_activeSensorCount --;
    	}
    	if (!(config->m_activeSensors & SENSOR_GYRO_X)) {
    		regVal |= MPU60X0_FIELD_STBY_XG;
    		mpu->m_activeSensorCount --;
    	}
    	if (!(config->m_activeSensors & SENSOR_ACC_Z)) {
    		regVal |= MPU60X0_FIELD_STBY_ZA;
    		mpu->m_activeSensorCount --;
    	}
    	if (!(config->m_activeSensors & SENSOR_ACC_Y)) {
    		regVal |= MPU60X0_FIELD_STBY_YA;
    		mpu->m_activeSensorCount --;
    	}
    	if (!(config->m_activeSensors & SENSOR_ACC_X)) {
    		regVal |= MPU60X0_FIELD_STBY_XA;
    		mpu->m_activeSensorCount --;
    	}

    	//Test for Temp Sensor as well
    	if (!(config->m_activeSensors & SENSOR_TEMP)) {
    		mpu->m_activeSensorCount --;
    	}


    	switch (config->m_pwrMgmtLowPwrWakeCtrl) {
    	case LP_WAKE_FREQ_1:
    		lp = MPU60X0_LP_WAKE_FREQ_1;
    		break;
    	case LP_WAKE_FREQ_5:
    		lp = MPU60X0_LP_WAKE_FREQ_5;
    		break;

    	case LP_WAKE_FREQ_20:
    		lp = MPU60X0_LP_WAKE_FREQ_20;
    		break;

    	case LP_WAKE_FREQ_40:
    		lp = MPU60X0_LP_WAKE_FREQ_40;
    		break;
    	default:
    		WARN("Invalid value for Low Power Wake Freq ... reverting to chip default value");
    		lp = MPU60X0_LP_WAKE_FREQ_1;
    	}

    	regVal |= lp;

    	retVal = writeReg(mpu, MPU60X0_REG_PWR_MGMT_2, regVal);
    	if (retVal) {
    		ERROR("Failed to Write in PWR_MGMT_2 Register");
    		goto END;
    	}

    	activeConfig->m_pwrMgmtLowPwrWakeCtrl = config->m_pwrMgmtLowPwrWakeCtrl;
    }

END:
	EXIT();
	return retVal;
}


static retcode updateReg_PWR_MGMT_1 (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0Config *config = &mpu->m_configs;
	Mpu60x0Config *activeConfig = &mpu->m_activeConfigs;
    retcode retVal = 0;
    uint8 regVal = 0;
    uint8 clk;

    switch (config->m_clkSrc) {
    case CLK_SRC_INT_OSC:
    	clk = MPU60X0_CLK_SRC_INT_OSC;
    	break;

    case CLK_SRC_GYRO:
    	if (config->m_activeSensors & SENSOR_GYRO_X) {
    		clk = MPU60X0_CLK_SRC_GYRO_X;
    	} else if (config->m_activeSensors & SENSOR_GYRO_Y) {
    		clk = MPU60X0_CLK_SRC_GYRO_Y;
    	} else if (config->m_activeSensors & SENSOR_GYRO_Z) {
    		clk = MPU60X0_CLK_SRC_GYRO_Z;
    	} else {
    		WARN("All Gyroscope Axis are in Standby .... Can not set clk source to it, reverting to chip default valule");
    		clk = MPU60X0_CLK_SRC_INT_OSC;
    	}
    	break;

    case CLK_SRC_EXT_LOW:
    	clk = MPU60X0_CLK_SRC_EXT_L;
    	break;

    case CLK_SRC_EXT_HIGH:
    	clk = MPU60X0_CLK_SRC_EXT_H;
    	break;

    default:
    	ERROR("Invalid Clk Source....reverting the the chip default ...");
    	retVal = -1;
    	goto END;
    }

    regVal |= clk;

 	if (!(config->m_activeSensors & SENSOR_TEMP)) {
    	regVal |= MPU60X0_FIELD_TEMP_DIS;
    }

 	//Wakeup/Cycle or put chip to sleep
 	switch (mpu->m_operMode) {
 	case MPU60X0_OPER_MODE_SLEEP:
 		regVal |= MPU60X0_FIELD_SLEEP;
 		PROGRESS("Putting the Chip in SLEEP mode");
 		break;

 	case MPU60X0_OPER_MODE_CYCLE:
 		regVal |= MPU60X0_FIELD_CYCLE;
 		PROGRESS("Putting the Chip in CYCLE mode");
 		break;

 	case MPU60X0_OPER_MODE_WAKE:
 		PROGRESS("Waking up the Chip");
 		break;
 	}

	retVal = writeReg(mpu, MPU60X0_REG_PWR_MGMT_1, regVal);
	if (retVal) {
		ERROR("Failed to Write in PWR_MGMT_1 Register");
		goto END;
	}
	activeConfig->m_activeSensors = config->m_activeSensors;
	activeConfig->m_clkSrc = config->m_clkSrc;

END:
	EXIT();
	return retVal;
}

/**
 * Internal Functions
 */


static uint8 getFsyncExtEncoding (const Mpu60x0_SensorId id) {

	ENTER();

	uint8 val;


	switch (id) {
	case SENSOR_NONE:
		val = MPU60X0_FSYNCH_EXT_NONE;
		break;

	case SENSOR_ACC_X:
		val = MPU60X0_FSYNCH_EXT_ACC_X;
		break;

	case SENSOR_ACC_Y:
		val = MPU60X0_FSYNCH_EXT_ACC_Y;
		break;

	case SENSOR_ACC_Z:
		val = MPU60X0_FSYNCH_EXT_ACC_Z;
		break;

	case SENSOR_GYRO_X:
		val = MPU60X0_FSYNCH_EXT_GYRO_X;
		break;

	case SENSOR_GYRO_Y:
		val = MPU60X0_FSYNCH_EXT_GYRO_Y;
		break;

	case SENSOR_GYRO_Z:
		val = MPU60X0_FSYNCH_EXT_GYRO_Z;
		break;

	case SENSOR_TEMP:
		val = MPU60X0_FSYNCH_EXT_TEMP;
		break;

	default:
		ERROR("Invalid Sensor Id, set to chip default");
		val = MPU60X0_FSYNCH_EXT_NONE;
	}


	EXIT();
	return val;
}

static uint8 getDlpfEncoding (const Mpu60x0_Lpf lpf) {

	ENTER();

	uint8 val;
	switch (lpf) {
	case LPF_NONE:
		val = MPU60X0_LPF_NONE;
		break;

	case LPF_184_188:
		val = MPU60X0_LPF_184_188;
		break;

	case LPF_94_98:
		val = MPU60X0_LPF_94_98;
		break;

	case LPF_44_42:
		val = MPU60X0_LPF_44_42;
		break;

	case LPF_21_20:
		val = MPU60X0_LPF_21_20;
		break;

	case LPF_10_10:
		val = MPU60X0_LPF_10_10;
		break;

	case LPF_5_5:
		val = MPU60X0_LPF_5_5;
		break;

	case LPF_RESERVED:
		val = MPU60X0_LPF_RESERVED;
		break;

	default:
		ERROR("Invalid LPF value");
		goto END;
	}

END:
	EXIT();
	return val;
}

static uint8 getGyroScaleEncoding (const Mpu60x0_GyroFullScale scale) {

	ENTER();

	uint8 val;

	switch (scale) {
	case GYRO_FULL_SCALE_250:
		val = MPU60X0_GYRO_SCALE_250;
		break;

	case GYRO_FULL_SCALE_500:
		val = MPU60X0_GYRO_SCALE_500;
		break;

	case GYRO_FULL_SCALE_1000:
		val = MPU60X0_GYRO_SCALE_1000;
		break;

	case GYRO_FULL_SCALE_2000:
		val = MPU60X0_GYRO_SCALE_2000;
		break;

	default:
		ERROR("Invalid GYRO Scale, set to chip default");
		val = MPU60X0_GYRO_SCALE_250;
	}

	EXIT();
	return val;
}

static uint8 getAccScaleEncoding (const Mpu60x0_AccFullScale scale) {

	ENTER();

	uint8 val;

	switch (scale) {
	case ACC_FULL_SCALE_2G:
		val = MPU60X0_ACC_SCALE_2G;
		break;

	case ACC_FULL_SCALE_4G:
		val = MPU60X0_ACC_SCALE_4G;
		break;

	case ACC_FULL_SCALE_8G:
		val = MPU60X0_ACC_SCALE_8G;
		break;

	case ACC_FULL_SCALE_16G:
		val = MPU60X0_ACC_SCALE_16G;
		break;

	default:
		ERROR("Invalid Accelerometer Scale, set to chip default");
		val = 0;
	}

	EXIT();
	return val;
}


static uint8 getAuxI2cClkEncoding (const Mpu60x0_AuxI2cClk clk) {
	ENTER();

	uint8 val;

	switch (clk) {
	case AUX_I2C_CLK_258:
		val = MPU60X0_CLK_258;
		break;

	case AUX_I2C_CLK_267:
 		val = MPU60X0_CLK_267;
		break;

	case AUX_I2C_CLK_276:
 		val = MPU60X0_CLK_276;
		break;

	case AUX_I2C_CLK_286:
 		val = MPU60X0_CLK_286;
		break;

	case AUX_I2C_CLK_296:
 		val = MPU60X0_CLK_296;
		break;

	case AUX_I2C_CLK_308:
 		val = MPU60X0_CLK_308;
		break;

	case AUX_I2C_CLK_320:
 		val = MPU60X0_CLK_320;
		break;

	case AUX_I2C_CLK_333:
 		val = MPU60X0_CLK_333;
		break;

	case AUX_I2C_CLK_348:
 		val = MPU60X0_CLK_348;
		break;

	case AUX_I2C_CLK_364:
 		val = MPU60X0_CLK_364;
		break;

	case AUX_I2C_CLK_381:
 		val = MPU60X0_CLK_381;
		break;

	case AUX_I2C_CLK_400:
 		val = MPU60X0_CLK_400;
		break;

	case AUX_I2C_CLK_421:
 		val = MPU60X0_CLK_421;
		break;

	case AUX_I2C_CLK_444:
 		val = MPU60X0_CLK_444;
		break;

	case AUX_I2C_CLK_471:
 		val = MPU60X0_CLK_471;
		break;

	case AUX_I2C_CLK_500:
 		val = MPU60X0_CLK_500;
		break;

	default:
		ERROR("Invalid CLK Speed, set to chip default");
		val = 0;
	}

	EXIT();
	return val;
}


static Mpu60x0_ClkSrc getClkSrc (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0_ClkSrc clk;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_PWR_MGMT_1);
	val &= MPU60X0_FIELD_CLKSEL;
	switch (val) {
	case MPU60X0_CLK_SRC_INT_OSC:
		clk = CLK_SRC_INT_OSC;
		break;

	case MPU60X0_CLK_SRC_GYRO_X:
	case MPU60X0_CLK_SRC_GYRO_Y:
	case MPU60X0_CLK_SRC_GYRO_Z:
		clk = CLK_SRC_GYRO;
		break;

	case MPU60X0_CLK_SRC_EXT_L:
		clk = CLK_SRC_EXT_LOW;
		break;

	case MPU60X0_CLK_SRC_EXT_H:
		clk = CLK_SRC_EXT_HIGH;
		break;

	default:
		ERROR("Invalid Clock Source ... reverting to default");
		clk = CLK_SRC_INT_OSC;
	}

	EXIT();
	return clk;
}

static uint32 getSamplingRate (Mpu60x0 *mpu) {
	ENTER();
	uint32 rate;
	uint8 dlpf;
	uint8 div;

	dlpf = readReg(mpu, MPU60X0_REG_CONFIG);
	dlpf &= MPU60X0_FIELD_DLPF_CFG;

	div = readReg(mpu, MPU60X0_REG_SMPLRT_DIV);

	if ((dlpf == MPU60X0_LPF_NONE) ||
			(dlpf == MPU60X0_LPF_RESERVED)) {
		rate = 8000/(1 + div);
	} else {
		rate = 1000/(1 + div);
	}

	EXIT();
	return rate;
}

static Mpu60x0_Lpf getLpf (Mpu60x0 *mpu) {
	ENTER();
	Mpu60x0_Lpf lpf;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_CONFIG);
	val &= 0x07;

	switch (val) {
	case MPU60X0_LPF_NONE:
		lpf = LPF_NONE;
		break;

	case MPU60X0_LPF_184_188:
		lpf = LPF_184_188;
		break;

	case MPU60X0_LPF_94_98:
		lpf = LPF_94_98;
		break;

	case MPU60X0_LPF_44_42:
		lpf = LPF_44_42;
		break;

	case MPU60X0_LPF_21_20:
		lpf = LPF_21_20;
		break;

	case MPU60X0_LPF_10_10:
		lpf = LPF_10_10;
		break;

	case MPU60X0_LPF_5_5:
		lpf = LPF_5_5;
		break;

	case MPU60X0_LPF_RESERVED:
		lpf = LPF_RESERVED;
		break;
	}

	EXIT();
	return lpf;
}


static Mpu60x0_AccFullScale getAccFullScale (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0_AccFullScale scale;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_ACCEL_CONFIG);
	val &= MPU60X0_FIELD_AFS_SEL;

	switch (val) {
	case MPU60X0_ACC_SCALE_2G:
		scale = ACC_FULL_SCALE_2G;
		break;

	case MPU60X0_ACC_SCALE_4G:
		scale = ACC_FULL_SCALE_4G;
		break;

	case MPU60X0_ACC_SCALE_8G:
		scale = ACC_FULL_SCALE_8G;
		break;

	case MPU60X0_ACC_SCALE_16G:
		scale = ACC_FULL_SCALE_16G;
		break;

	default:
		ERROR("Invalid value for Accelerometer full scale range...reverting to chip default value");
		scale = ACC_FULL_SCALE_2G;
	}

	EXIT();
	return scale;
}

static Mpu60x0_GyroFullScale getGyroFullScale (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0_GyroFullScale scale;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_GYRO_CONFIG);
	val &= MPU60X0_FIELD_FS_SEL;

	switch (val) {
	case MPU60X0_GYRO_SCALE_250:
		scale = GYRO_FULL_SCALE_250;
		break;

	case MPU60X0_GYRO_SCALE_500:
		scale = GYRO_FULL_SCALE_500;
		break;

	case MPU60X0_GYRO_SCALE_1000:
		scale = GYRO_FULL_SCALE_1000;
		break;

	case MPU60X0_GYRO_SCALE_2000:
		scale = GYRO_FULL_SCALE_2000;
		break;

	default:
		ERROR("Invalid value for Gyroscope full scale range...reverting to chip default value");
		scale = GYRO_FULL_SCALE_250;
	}

	EXIT();
	return scale;
}

static bool getFifoUsage(Mpu60x0 *mpu) {
	ENTER();

	bool fifo;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_USER_CTRL);

	val &= MPU60X0_FIELD_FIFO_EN;
	if (val) {
		fifo = TRUE;
	} else {
		fifo = FALSE;
	}

	EXIT();
	return fifo;
}

static Mpu60x0_IntLvl getIntLvl (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0_IntLvl lvl;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_INT_PIN_CFG);

	if  (val &= MPU60X0_FIELD_INT_LEVEL) {
		lvl = INT_LVL_ACTIVE_LOW;
	} else {
		lvl = INT_LVL_ACTIVE_HIGH;
	}

	EXIT();
	return lvl;
}


static bool getIsOpenDrainInt(Mpu60x0 *mpu) {
	ENTER();

	bool isOpen;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_INT_PIN_CFG);

	val &= MPU60X0_FIELD_INT_OPEN;
	if (val) {
		isOpen = TRUE;
	} else {
		isOpen = FALSE;
	}

	EXIT();
	return isOpen;
}

static bool getIsLatchInt(Mpu60x0 *mpu) {
	ENTER();

	bool isLatch;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_INT_PIN_CFG);

	val &= MPU60X0_FIELD_LATCH_INT_EN;
	if (val) {
		isLatch = TRUE;
	} else {
		isLatch = FALSE;
	}

	EXIT();
	return isLatch;
}

static bool getClearOnReadInt(Mpu60x0 *mpu) {
	ENTER();

	bool clearOnRead;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_INT_PIN_CFG);

	val &= MPU60X0_FIELD_INT_RD_CLEAR;
	if (val) {
		clearOnRead = TRUE;
	} else {
		clearOnRead = FALSE;
	}

	EXIT();
	return clearOnRead;
}

static uint8 getMotDetThr (Mpu60x0 *mpu) {
	ENTER();

	uint8 thr;

	thr = readReg(mpu, MPU60X0_REG_MOT_THR);

	EXIT();
	return thr;
}


static uint8 getActiveSensors (Mpu60x0 *mpu) {
	ENTER();

	uint8 sensors = 0;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_PWR_MGMT_2);

	if (!(val & MPU60X0_FIELD_STBY_XA)) {
		sensors |= SENSOR_ACC_X;
	}
	if (!(val & MPU60X0_FIELD_STBY_YA)) {
		sensors |= SENSOR_ACC_Y;
	}
	if (!(val & MPU60X0_FIELD_STBY_ZA)) {
		sensors |= SENSOR_ACC_Z;
	}
	if (!(val & MPU60X0_FIELD_STBY_XG)) {
		sensors |= SENSOR_GYRO_X;
	}
	if (!(val & MPU60X0_FIELD_STBY_YG)) {
		sensors |= SENSOR_GYRO_Y;
	}
	if (!(val & MPU60X0_FIELD_STBY_ZG)) {
		sensors |= SENSOR_GYRO_Z;
	}

	val = readReg(mpu, MPU60X0_REG_PWR_MGMT_2);

	if (!(val & MPU60X0_FIELD_TEMP_DIS)) {
		sensors |= SENSOR_TEMP;
	}

	EXIT();
	return sensors;
}

static Mpu60x0_AuxI2cMode getAuxI2cMode (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0_AuxI2cMode  mode;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_USER_CTRL);
	if (val & MPU60X0_FIELD_I2C_MST_EN) {
		mode = AUX_I2C_MSTR;
	} else {
		val = readReg(mpu, MPU60X0_REG_INT_PIN_CFG);
		if (val & MPU60X0_FIELD_I2C_BYPASS_EN) {
			mode = AUX_I2C_BYPASS;
		} else {
			mode = AUX_I2C_DISABLE;
		}
	}


	EXIT();
	return mode;
}

static Mpu60x0_AuxI2cClk getAuxI2cClk (Mpu60x0 *mpu) {
	ENTER();

	Mpu60x0_AuxI2cClk  clk;
	uint8 val;

	val = readReg(mpu, MPU60X0_REG_I2C_MST_CTRL);
	val &= MPU60X0_FIELD_I2C_MST_CLK;

	switch (val) {
	case MPU60X0_CLK_258:
		clk = AUX_I2C_CLK_258;
		break;

	case MPU60X0_CLK_267:
		clk = AUX_I2C_CLK_267;
		break;

	case MPU60X0_CLK_276:
		clk = AUX_I2C_CLK_276;
		break;

	case MPU60X0_CLK_286:
		clk = AUX_I2C_CLK_286;
		break;

	case MPU60X0_CLK_296:
		clk = AUX_I2C_CLK_296;
		break;

	case MPU60X0_CLK_308:
		clk = AUX_I2C_CLK_308;
		break;

	case MPU60X0_CLK_320:
		clk = AUX_I2C_CLK_320;
		break;

	case MPU60X0_CLK_333:
		clk = AUX_I2C_CLK_333;
		break;

	case MPU60X0_CLK_348:
		clk = AUX_I2C_CLK_348;
		break;

	case MPU60X0_CLK_364:
		clk = AUX_I2C_CLK_364;
		break;

	case MPU60X0_CLK_381:
		clk = AUX_I2C_CLK_381;
		break;

	case MPU60X0_CLK_400:
		clk = AUX_I2C_CLK_400;
		break;

	case MPU60X0_CLK_421:
		clk = AUX_I2C_CLK_421;
		break;

	case MPU60X0_CLK_444:
		clk = AUX_I2C_CLK_444;
		break;

	case MPU60X0_CLK_471:
		clk = AUX_I2C_CLK_471;
		break;

	case MPU60X0_CLK_500:
		clk = AUX_I2C_CLK_500;
		break;

	default:
		ERROR("This should never happen, all values covered");
	}

	EXIT();
	return clk;
}



static uint8 getAuxI2cReducedSamplingFactor (Mpu60x0 *mpu) {

	ENTER();

	uint8 val;

	val = readReg(mpu, MPU60X0_REG_I2C_SLV4_CTRL);
	val &= 0x1F;


	EXIT();
	return val;
}

//Read & Write into HW Registers
retcode writeReg (Mpu60x0 *mpu, const uint8 reg, const uint8 val) {
	ENTER();

	//Currently it is just a stub
	//TODO

	PROGRESS("Writing in Register %d with value = %d", reg, val);

	EXIT();
	return 0;
}

uint8 readReg(Mpu60x0 *mpu, const uint8 reg) {
	uint8 val = 0;

	//Currently it is just a stub
	//TODO

	return val;
}

retcode readMultiReg (Mpu60x0 *mpu,
	  	              const uint8 reg,
				      uint8 *valArray,
				      const uint8 count) {
	//Currently it is just a stub
	//TODO
	return 0;
}


retcode multiReadSameReg (Mpu60x0 *mpu,
		                  const uint8 reg,
				          uint8 *valArray,
				          const uint8 count) {
	//Currently it is just a stub
	//TODO
	return 0;
}

//Reset Internal Functions
static retcode resetMpuData (Mpu60x0 *mpu) {
	ENTER();

	mpu->m_operMode = MPU60X0_OPER_MODE_SLEEP;
	mpu->m_sensorRegExtCount = 0;
	mpu->m_activeSensorCount = 7;

	EXIT();
	return 0;
}

static retcode setRegDefault (Mpu60x0 *mpu, Mpu60x0Config *config) {
	ENTER();

	//First set all to zeros,
	memset(config, 0, sizeof(Mpu60x0Config));

	//Then selective setting of params
	config->m_activeSensors = SENSOR_ALL;
    config->m_clkSrc = CLK_SRC_INT_OSC;
    config->m_samplingDiv = 0;
    config->m_dlpf = LPF_NONE;
    config->m_accScale = ACC_FULL_SCALE_2G;
    config->m_gyroScale = GYRO_FULL_SCALE_250;

    config->m_intLvl = INT_LVL_NO_INT;

	//Auxiliary I2C Bus
	config->m_auxI2cMode = AUX_I2C_DISABLE;
	config->m_auxI2cClk = AUX_I2C_CLK_348;

	//FSync
	 config->m_fsyncSensorId = SENSOR_NONE;
     config->m_fsyncIntLvl = INT_LVL_NO_INT;

	EXIT();
	return 0;
}

static retcode resetSensorDataRegs(Mpu60x0 *mpu) {
	ENTER();

	memset (mpu->m_sensorData, 0, 40 * sizeof(uint8));

	EXIT();
	return 0;
}

static retcode updateChip (Mpu60x0 *mpu) {
	ENTER();

	retcode retVal = 0;

	//SMPLRT_DIV Register
    retVal = updateReg_SMPLRT_DIV(mpu);
	if (retVal){
		ERROR("Failed to update the SMPLRT_DIV register");
		goto END;
	}

	//CONFIG Register
    retVal = updateReg_CONFIG(mpu);
	if (retVal){
		ERROR("Failed to update the CONFIG register");
		goto END;
	}

	//GYRO_CONFIG Register
    retVal = updateReg_GYRO_CONFIG(mpu);
	if (retVal){
		ERROR("Failed to update the GYRO_CONFIG register");
		goto END;
	}

	//ACCEL_CONFIG Register
    retVal = updateReg_ACCEL_CONFIG(mpu);
	if (retVal){
		ERROR("Failed to update the ACCEL_CONFIG register");
		goto END;
	}

	//MOT_THR Register
    retVal = updateReg_MOT_THR(mpu);
	if (retVal){
		ERROR("Failed to update the MOT_THR register");
		goto END;
	}

	//FIFO_EN Register
    retVal = updateReg_FIFO_EN(mpu);
	if (retVal){
		ERROR("Failed to update the FIFO_EN register");
		goto END;
	}

	//I2C_MST_CTRL Register
    updateReg_I2C_MST_CTRL(mpu);
	if (retVal){
		ERROR("Failed to update the I2C_MST_CTRL register");
		goto END;
	}


	//I2C_SLV0_ADDR  (and 1,2,3 as well)
	//I2C_SLV0_REG   (and 1,2,3 as well)
	//I2C_SLV0_CTRL  (and 1,2,3 as well)
    mpu->m_sensorRegExtCount = 0;
    for (uint8 i = 0; i < 4; i++) {
    	retVal = updateReg_I2C_SLVn_xxx(mpu, i);
    	if (retVal){
    		ERROR("Failed to update the I2C_SLV%d_xxx register",i);
    		goto END;
    	}
    }

	//I2C_SLV4_ADDR Register
	//I2C_SLV4_REG Register
	//I2C_SLV4_CTRL Register
    retVal = updateReg_I2C_SLV4_xxx(mpu);
	if (retVal){
		ERROR("Failed to update the I2C_SLV4_xxx register");
		goto END;
	}

    //INT_PIN_CFG Register
    retVal = updateReg_INT_PIN_CFG(mpu);
	if (retVal){
		ERROR("Failed to update the INT_PIN register");
		goto END;
	}


    //INT_ENABLE Register
    retVal = updateReg_INT_ENABLE(mpu);
	if (retVal){
		ERROR("Failed to update the INT_ENABLE register");
		goto END;
	}

    //I2C_MST_DELAY_CTRL Register
    retVal = updateReg_I2C_MST_DELAY_CTRL(mpu);
	if (retVal){
		ERROR("Failed to update the I2C_MST_DELAY_CTRL register");
		goto END;
	}


    //MOT_DETECT_CTRL Register
    retVal = updateReg_MOT_DETECT_CTRL(mpu);
	if (retVal){
		ERROR("Failed to update the MOT_DETECT_CTRL register");
		goto END;
	}


    //USER_CTRL Register
    retVal = updateReg_USER_CTRL(mpu);
	if (retVal){
		ERROR("Failed to update the USER_CTRL register");
		goto END;
	}


    //PWR_MGMT_2 Register
    retVal = updateReg_PWR_MGMT_2(mpu);
	if (retVal){
		ERROR("Failed to update the PWR_MGMT_2 register");
		goto END;
	}

    //PWR_MGMT_1 Register
    retVal = updateReg_PWR_MGMT_1(mpu);
	if (retVal){
		ERROR("Failed to update the PWR_MGMT_1 register");
		goto END;
	}


END:
	EXIT();
	return retVal;
}

static retcode startChip (Mpu60x0 *mpu) {
	ENTER();

	retcode retVal = 0;

	mpu->m_operMode = MPU60X0_OPER_MODE_WAKE;

    //PWR_MGMT_1 Register
    retVal = updateReg_PWR_MGMT_1(mpu);  //This also awakes the chip from SLEEP Mode
	if (retVal){
		ERROR("Failed to update the PWR_MGMT_1 register");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode stopChip (Mpu60x0 *mpu) {
	ENTER();

	retcode retVal = 0;

	mpu->m_operMode = MPU60X0_OPER_MODE_SLEEP;

	retVal = updateReg_PWR_MGMT_1(mpu);
	if (retVal) {
		ERROR("Failed to update the PWR_MGMT_1 register to switch in SLEEP Mode");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

//Reading Sensor Data from Registers Internal Functions
static retcode readSensorData(Mpu60x0 *mpu) {
	ENTER();

	retcode retVal = 0;

	//Reading from the registers
	retVal = readMultiReg(mpu, MPU60X0_REG_ACCEL_XOUT_H, mpu->m_sensorData, 14 + mpu->m_sensorRegExtCount);
	if (retVal) {
		ERROR("Failed to Read Sensor Data");
		goto END;
	}

	if ((mpu->m_activeConfigs.m_auxI2cSlv[4].m_slvIsEnabled) &&
			(mpu->m_activeConfigs.m_auxI2cSlv[4].m_slvOper == ACCESS_OPER_READ)) {
		mpu->m_sensorData[14 + mpu->m_sensorRegExtCount] = readReg(mpu, MPU60X0_REG_I2C_SLV4_DI);
	}

END:
	EXIT();
	return retVal;
}

static retcode readSensroDataFromFifo (Mpu60x0 *mpu) {
	ENTER();

	//First Get the Fifo Length
	retcode retVal = 0;
	uint16 fifoLen;
	uint8 expectedBurstLen;
	uint8 valArray[4];

	expectedBurstLen = mpu->m_activeSensorCount * 2 + mpu->m_sensorRegExtCount;
	//Keep in mind that the m_sensorRegExtCount does not include the SLV-4
	if ((mpu->m_activeConfigs.m_auxI2cSlv[4].m_slvIsEnabled) &&
			(mpu->m_activeConfigs.m_auxI2cSlv[4].m_slvOper == ACCESS_OPER_READ)) {
		expectedBurstLen ++;
	}


	if (expectedBurstLen == 0) {
		ERROR("Nothing To Read, Exiting");
		retVal = -1;
		goto END;
	}

	retVal = readMultiReg(mpu, MPU60X0_REG_FIFO_COUNT_H, valArray, 2 );
	if(retVal) {
		ERROR("Failed to read The FIFO Length...exiting");
		goto END;
	}

	fifoLen = ( valArray[0] << 8 ) | (valArray[1]);
	if (!fifoLen) {
		ERROR("FIFO is Empty ... Exiting");
		retVal = -1;
		goto END;
	}

	if (fifoLen % expectedBurstLen) {
		ERROR("FIFO Length does not seem correct: %d", fifoLen);
		retVal = -1;
		goto END;
	}

	if (fifoLen > expectedBurstLen) {
		WARN("Multiple Bursts, will read only First burst");
	}

	//Read from the FIFO
	retVal = multiReadSameReg(mpu, MPU60X0_REG_FIFO_R_W, mpu->m_sensorData, expectedBurstLen);
	if (retVal) {
		ERROR("Failed to read the FIFO Data ... Exiting");
		goto END;
	}

END:
	EXIT();
	return retVal;
}


//Sensor Data Retrieval Internal Functions
static retcode getAccData(Mpu60x0 *mpu,
		                  float *x, float *y, float *z) {
	ENTER();
	retcode retVal = 0;
	float sens;
	int16 val;
	uint8 index = 0;

	if (!(mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_ALL)) {
		ERROR("All Accelerometer axis are disabled....can not get acc data");
		retVal = -1;
		*x = *y = *z = 0;
		goto END;
	}

	switch (mpu->m_activeConfigs.m_accScale) {
	case ACC_FULL_SCALE_2G:
		sens = 16384.0;
		break;

	case ACC_FULL_SCALE_4G:
		sens = 8192.0;
		break;

	case ACC_FULL_SCALE_8G:
		sens = 4096.0;
		break;

	case ACC_FULL_SCALE_16G:
		sens = 2048.0;
		break;

	default:
		ERROR("Invalid ACC FSR value");
		retVal = -1;
		goto END;
		break;
	}

	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_X) {
		val = mpu->m_sensorData[index++] << 8;
		val |= mpu->m_sensorData[index++];
		*x = val /sens;
	} else {
		*x = 0;
	}

	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_Y) {
		val = mpu->m_sensorData[index++] << 8;
		val |= mpu->m_sensorData[index++];
		*y = val /sens;
	} else {
		*y = 0;
	}

	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_Z) {
		val = mpu->m_sensorData[index++] << 8;
		val |= mpu->m_sensorData[index++];
		*z = val /sens;
	} else {
		*z = 0;
	}

END:
	EXIT();
	return retVal;
}


static retcode getGyroData(Mpu60x0 *mpu,
		                   float *x, float *y, float *z) {
	ENTER();

	retcode retVal = 0;
	float sens;
	int16 val;
	uint8 index = 0;

	if (!(mpu->m_activeConfigs.m_activeSensors & SENSOR_GYRO_ALL)) {
		ERROR("All Gyroscope axis are disabled....can not get Gyro data");
		retVal = -1;
		*x = *y = *z = 0;
		goto END;
	}
//TODO put accurate sensitivity values
	switch (mpu->m_activeConfigs.m_gyroScale) {
	case GYRO_FULL_SCALE_250:
		sens = 131.0;
		break;

	case GYRO_FULL_SCALE_500:
		sens = 65.5;
		break;

	case GYRO_FULL_SCALE_1000:
		sens = 32.8;
		break;

	case GYRO_FULL_SCALE_2000:
		sens = 16.4;
		break;

	default:
		ERROR("Invalid Gyro FSR value");
		retVal = -1;
		goto END;
		break;
	}

	//Set the starting index
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_X) {
		index += 2;
	}
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_Y) {
		index += 2;
	}
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_Z) {
		index += 2;
	}
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_TEMP) {
		index += 2;
	}

	//Now get the data
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_GYRO_X) {
		val = mpu->m_sensorData[index++] << 8;
		val |= mpu->m_sensorData[index++];
		*x = val /sens;
	} else {
		*x = 0;
	}

	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_GYRO_Y) {
		val = mpu->m_sensorData[index++] << 8;
		val |= mpu->m_sensorData[index++];
		*y = val /sens;
	} else {
		*y = 0;
	}

	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_GYRO_Z) {
		val = mpu->m_sensorData[index++] << 8;
		val |= mpu->m_sensorData[index++];
		*z = val /sens;
	} else {
		*z = 0;
	}

END:
	EXIT();
	return retVal;
}

static retcode getTempData(Mpu60x0 *mpu, float *temp) {
	ENTER();

	retcode retVal = 0;
	int16 val;
	uint8 index = 0;

	if (!(mpu->m_activeConfigs.m_activeSensors & SENSOR_TEMP)) {
		ERROR("Temperature sensor is disabled....can not get Temp data");
		retVal = -1;
		temp = 0;
		goto END;
	}

	//Set the starting index
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_X) {
		index += 2;
	}
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_Y) {
		index += 2;
	}
	if (mpu->m_activeConfigs.m_activeSensors & SENSOR_ACC_Z) {
		index += 2;
	}

	val = mpu->m_sensorData[index++] << 8;
	val |= mpu->m_sensorData[index++];
	*temp = ( val / MPU60X0_TEMP_SENS ) + 36.53;

END:
	EXIT();
	return retVal;
}



//Self Test Internal Functions
static float calcAccRefValue(uint8 testVal) {
	ENTER();
	float refVal;

	refVal = 4096 * 0.34 * pow((0.92/0.34),((testVal - 1)/30.0));

	EXIT();
	return refVal;
}

static float calcGyroRefValue (uint8 testVal) {
	ENTER();
	float refVal;
	float sens;

	sens = 32768/250.0;

	refVal = 25 * sens * pow(1.046,(testVal - 1));

	EXIT();
	return refVal;
}


static bool calcSelfTestAccX (Mpu60x0 *mpu, float delta) {
	float refVal;
	bool result;
	uint8 val1, val2, testVal;

	val1 = readReg(mpu, MPU60X0_REG_SELF_TEST_X);
	val1 &= MPU60X0_FIELD_XA_TEST_H;
	val1 >>= 3;

	val2 = readReg(mpu, MPU60X0_REG_SELF_TEST_A);
	val2 &= MPU60X0_FIELD_XA_TEST_L;
	val2 >>= 4;

	testVal = val1 | val2;
	if (testVal) {
		refVal = calcAccRefValue(testVal);
	} else {
		refVal = 0;
		ERROR("RefVal = 0 for ACC_X");
		result = FALSE;
		goto END;
	}

	refVal = (delta - refVal) / refVal;
	if (refVal > MPU60X0_SELF_TEST_TOL) {
		ERROR ("Failed Test for Acc_X");
		result = FALSE;
		goto END;
	}

END:
	EXIT();
	return result;
}

static bool calcSelfTestAccY (Mpu60x0 *mpu, float delta) {
	float refVal;
	bool result;
	uint8 val1, val2, testVal;

	val1 = readReg(mpu, MPU60X0_REG_SELF_TEST_Y);
	val1 &= MPU60X0_FIELD_YA_TEST_H;
	val1 >>= 3;

	val2 = readReg(mpu, MPU60X0_REG_SELF_TEST_A);
	val2 &= MPU60X0_FIELD_YA_TEST_L;
	val2 >>= 2;

	testVal = val1 | val2;
	if (testVal) {
		refVal = calcAccRefValue(testVal);
	} else {
		refVal = 0;
		ERROR("RefVal = 0 for ACC_Y");
		result = FALSE;
		goto END;
	}

	refVal = (delta - refVal) / refVal;
	if (refVal > MPU60X0_SELF_TEST_TOL) {
		ERROR ("Failed Test for Acc_Y");
		result = FALSE;
		goto END;
	}

END:
	EXIT();
	return result;
}

static bool calcSelfTestAccZ (Mpu60x0 *mpu, float delta) {
	float refVal;
	bool result;
	uint8 val1, val2, testVal;

	val1 = readReg(mpu, MPU60X0_REG_SELF_TEST_Z);
	val1 &= MPU60X0_FIELD_ZA_TEST_H;
	val1 >>= 3;

	val2 = readReg(mpu, MPU60X0_REG_SELF_TEST_A);
	val2 &= MPU60X0_FIELD_ZA_TEST_L;

	testVal = val1 | val2;
	if (testVal) {
		refVal = calcAccRefValue(testVal);
	} else {
		refVal = 0;
		ERROR("RefVal = 0 for ACC_Z");
		result = FALSE;
		goto END;
	}

	refVal = (delta - refVal) / refVal;
	if (refVal > MPU60X0_SELF_TEST_TOL) {
		ERROR ("Failed Test for Acc_Z");
		result = FALSE;
		goto END;
	}

END:
	EXIT();
	return result;
}

static bool calcSelfTestGyroX (Mpu60x0 *mpu, float delta) {
	float refVal;
	bool result;
	uint8 testVal;

	testVal = readReg(mpu, MPU60X0_REG_SELF_TEST_X);
	testVal &= MPU60X0_FIELD_XG_TEST;

	if (testVal) {
		refVal = calcGyroRefValue(testVal);
	} else {
		refVal = 0;
		ERROR("RefVal = 0 for Gyro_X");
		result = FALSE;
		goto END;
	}

	refVal = (delta - refVal) / refVal;
	if (refVal > MPU60X0_SELF_TEST_TOL) {
		ERROR ("Failed Test for Gyro_X");
		result = FALSE;
		goto END;
	}

END:
	EXIT();
	return result;
}

static bool calcSelfTestGyroY (Mpu60x0 *mpu, float delta) {
	float refVal;
	bool result;
	uint8 testVal;

	testVal = readReg(mpu, MPU60X0_REG_SELF_TEST_Y);
	testVal &= MPU60X0_FIELD_YG_TEST;

	if (testVal) {
		refVal = -1 * calcGyroRefValue(testVal);
	} else {
		refVal = 0;
		ERROR("RefVal = 0 for Gyro_Y");
		result = FALSE;
		goto END;
	}

	refVal = (delta - refVal) / refVal;
	if (refVal > MPU60X0_SELF_TEST_TOL) {
		ERROR ("Failed Test for Gyro_Y");
		result = FALSE;
		goto END;
	}

END:
	EXIT();
	return result;
}

static bool calcSelfTestGyroZ (Mpu60x0 *mpu, float delta) {
	float refVal;
	bool result;
	uint8 testVal;

	testVal = readReg(mpu, MPU60X0_REG_SELF_TEST_Z);
	testVal &= MPU60X0_FIELD_ZG_TEST;

	if (testVal) {
		refVal = calcGyroRefValue(testVal);
	} else {
		refVal = 0;
		ERROR("RefVal = 0 for Gyro_Z");
		result = FALSE;
		goto END;
	}

	refVal = (delta - refVal) / refVal;
	if (refVal > MPU60X0_SELF_TEST_TOL) {
		ERROR ("Failed Test for Gyro_Z");
		result = FALSE;
		goto END;
	}

END:
	EXIT();
	return result;
}
