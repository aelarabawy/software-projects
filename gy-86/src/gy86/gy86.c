/*
 * gy86.c
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#include <math.h>
#include <unistd.h>
#include "logger.h"
#include "gy86_regs.h"
#include "gy86.h"

//Internal Functions Prototypes
static retcode resetGyData(Gy86 *);

static void mpuDataRdyIntHandler (void *, uint32);
static void mpuFifoIntHandler (void *, uint32);
static void mpuMotDetIntHandler (void *, uint32);
static void mpuFsyncIntHandler (void *, uint32);
static void mpuAuxI2cIntHandler (void *, uint32);

static void hmcDataRdyIntHandler (void *, uint32);
static void hmcRegLockIntHandler (void *, uint32);
//////////////////////////////

/**
 * Basic Functions
 */

gyHandle gy86_Init (Gy86_ChipType chip) {

	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)malloc(sizeof(Gy86));
	if (!gy) {
		ALLOC_FAIL("gy");
		goto END;
	}
	memset(gy, 0x00, sizeof(Gy86));

	resetGyData(gy);

	if (chip & CHIP_TYPE_MPU60X0) {
		gy->m_mpu = mpu60x0_Init();
		if (!gy->m_mpu) {
			ERROR("Failed to Initialize the MPU Chip");
			goto FAIL_MPU;
		}
	}

	//Register the Interrupt Functions
	retVal = mpu60x0_RegDataRdyCb (gy->m_mpu, mpuDataRdyIntHandler, (uint32)gy);
	if (retVal) {
		ERROR("Failed in Registering the Interrupt Handler for the MPU:DataRdy");
	}

	retVal = mpu60x0_RegFifoOvrflowCb (gy->m_mpu, mpuFifoIntHandler, (uint32)gy);
	if (retVal) {
		ERROR("Failed in Registering the Interrupt Handler for the MPU:FIFO");
	}

	retVal = mpu60x0_RegMotDetCb (gy->m_mpu, mpuMotDetIntHandler, (uint32)gy);
	if (retVal) {
		ERROR("Failed in Registering the Interrupt Handler for the MPU:MotDet");
	}

	retVal = mpu60x0_RegFsynchCb (gy->m_mpu, mpuFsyncIntHandler, (uint32)gy);
	if (retVal) {
		ERROR("Failed in Registering the Interrupt Handler for the MPU:Fsync");
	}

	retVal = mpu60x0_RegAuxI2cCb (gy->m_mpu, mpuAuxI2cIntHandler, (uint32)gy);
	if (retVal) {
		ERROR("Failed in Registering the Interrupt Handler for the MPU:AuxI2c");
	}


	if (chip & CHIP_TYPE_HMC5883) {
		gy->m_hmc = hmc5883_Init();
		if (!gy->m_hmc) {
			ERROR("Failed to Initialize the HMC Chip");
			goto FAIL_HMC;
		}
	}

	//Register the Interrupt Functions
	retVal = hmc5883_RegDataRdyCb (gy->m_hmc, hmcDataRdyIntHandler, (uint32)gy);
	if (retVal) {
		ERROR("Failed in Registering the Interrupt Handler for the HMC:DataRdy");
	}

	retVal = hmc5883_RegLockCb (gy->m_hmc, hmcRegLockIntHandler, (uint32)gy);
	if (retVal) {
		ERROR("Failed in Registering the Interrupt Handler for the HMC:Reg Lock");
	}


	if (chip & CHIP_TYPE_MS5611) {
		gy->m_ms = ms5611_Init();
		if (!gy->m_ms) {
			ERROR("Failed to Initialize the MS Chip");
			goto FAIL_MS;
		}
	}

	//Init Completed Successfully
	goto END;

FAIL_MS:
	hmc5883_Destroy(gy->m_hmc);
FAIL_HMC:
	mpu60x0_Destroy(gy->m_mpu);
FAIL_MPU:
	free(gy);
	gy = NULL;
END:
	EXIT();
	return (gyHandle)gy;
}

retcode gy86_Start (gyHandle handle, Gy86_ChipType chip) {
	ENTER();

	Gy86 *gy = (Gy86 *)handle;
	retcode retVal = 0;

	if (!gy) {
		NULL_POINTER("gy");
		retVal = -1;
		goto END;
	}

	if (chip & CHIP_TYPE_MPU60X0) {
		if (gy->m_mpu) {
			retVal = mpu60x0_Start(gy->m_mpu);
			if (retVal) {
				ERROR("Can not start the MPU Chip");
				goto END;
			}
		} else {
			ERROR("Can not start the MPU Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_HMC5883) {
		if (gy->m_hmc) {
			retVal = hmc5883_Start(gy->m_hmc);
			if (retVal) {
				ERROR("Can not start the HMC Chip");
				goto END;
			}
		} else {
			ERROR("Can not start the HMC Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_MS5611) {
		if (gy->m_ms) {
			retVal = ms5611_Start(gy->m_ms);
			if (retVal) {
				ERROR("Can not start the MS Chip");
				goto END;
			}
		} else {
			ERROR("Can not start the MS Chip, chip not initialized");
			goto END;
		}
	}


END:
	EXIT();
	return retVal;

}


retcode gy86_Stop (gyHandle handle, Gy86_ChipType chip) {
	ENTER();

	Gy86 *gy = (Gy86 *)handle;
	retcode retVal = 0;

	if (!gy) {
		NULL_POINTER("gy");
		retVal = -1;
		goto END;
	}

	if (chip & CHIP_TYPE_MPU60X0) {
		if (gy->m_mpu) {
			retVal = mpu60x0_Stop(gy->m_mpu);
			if (retVal) {
				ERROR("Can not stop the MPU Chip");
				goto END;
			}
		} else {
			ERROR("Can not stop the MPU Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_HMC5883) {
		if (gy->m_hmc) {
			retVal = hmc5883_Stop(gy->m_hmc);
			if (retVal) {
				ERROR("Can not stop the HMC Chip");
				goto END;
			}
		} else {
			ERROR("Can not stop the HMC Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_MS5611) {
		if (gy->m_ms) {
			retVal = ms5611_Stop(gy->m_ms);
			if (retVal) {
				ERROR("Can not stop the MS Chip");
				goto END;
			}
		} else {
			ERROR("Can not stop the MS Chip, chip not initialized");
			goto END;
		}
	}

END:
	EXIT();
	return retVal;

}


/**
 * This function will cleanup any data structures associated with the chips and the module
 * This function should be called only once
 */
retcode gy86_Destroy (gyHandle handle) {
	ENTER();

	Gy86 *gy = (Gy86 *)handle;
	retcode retVal = 0;

	if (!gy) {
		NULL_POINTER("gy");
		retVal = -1;
		goto END;
	}


	if (gy->m_mpu) {
		retVal = mpu60x0_Destroy(gy->m_mpu);
		if (retVal) {
			ERROR("Can not Destroy the MPU Chip");
		}
	}

	if (gy->m_hmc) {
		retVal = hmc5883_Destroy(gy->m_hmc);
		if (retVal) {
			ERROR("Can not Destroy the HMC Chip");
		}
	}

	if (gy->m_ms) {
		retVal = ms5611_Destroy(gy->m_ms);
		if (retVal) {
			ERROR("Can not Destroy the MS Chip");
		}
	}

	//Now free the gy data structure
	free(gy);

END:
	EXIT();
	return retVal;
}


/**
 * This function will Update the chip configuration to the HW Register
 */
retcode gy86_Update (gyHandle handle, const Gy86_ChipType chip) {

	ENTER();

	Gy86 *gy = (Gy86 *)handle;
	retcode retVal = 0;

	if (!gy) {
		NULL_POINTER("gy");
		retVal = -1;
		goto END;
	}

	if (chip & CHIP_TYPE_MPU60X0) {
		if (gy->m_mpu) {
			retVal = mpu60x0_Update(gy->m_mpu);
			if (retVal) {
				ERROR("Can not Update the MPU Chip");
				goto END;
			}
		} else {
			ERROR("Can not Update the MPU Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_HMC5883) {
		if (gy->m_hmc) {
			retVal = hmc5883_Update(gy->m_hmc);
			if (retVal) {
				ERROR("Can not Update the HMC Chip");
				goto END;
			}
		} else {
			ERROR("Can not Update the HMC Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_MS5611) {
		if (gy->m_ms) {
			retVal = ms5611_Update(gy->m_ms);
			if (retVal) {
				ERROR("Can not Update the MS Chip");
				goto END;
			}
		} else {
			ERROR("Can not Update the MS Chip, chip not initialized");
			goto END;
		}
	}

END:
	EXIT();
	return retVal;
}

/**
 * This function will RESET the GY-86 module or part of it
 * In case it is
 */
retcode gy86_Reset (gyHandle handle,
		            const Gy86_ChipType chip,
					const Mpu60x0_ResetMode mode) {

	ENTER();

	Gy86 *gy = (Gy86 *)handle;
	retcode retVal = 0;

	if (!gy) {
		NULL_POINTER("gy");
		retVal = -1;
		goto END;
	}

	if (chip & CHIP_TYPE_MPU60X0) {
		if (gy->m_mpu) {
			retVal = mpu60x0_Reset(gy->m_mpu, mode);
			if (retVal) {
				ERROR("Can not Reset the MPU Chip");
				goto END;
			}
		} else {
			ERROR("Can not Reset the MPU Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_HMC5883) {
		if (gy->m_hmc) {
			retVal = hmc5883_Reset(gy->m_hmc);
			if (retVal) {
				ERROR("Can not Reset the HMC Chip");
				goto END;
			}
		} else {
			ERROR("Can not Reset the HMC Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_MS5611) {
		if (gy->m_ms) {
			retVal = ms5611_Reset(gy->m_ms);
			if (retVal) {
				ERROR("Can not Reset the MS Chip");
				goto END;
			}
		} else {
			ERROR("Can not Reset the MS Chip, chip not initialized");
			goto END;
		}
	}

END:
	EXIT();
	return retVal;
}

retcode gy86_Read(gyHandle handle,
                  const Gy86_ChipType chip) {
	ENTER();

	Gy86 *gy = (Gy86 *)handle;
	retcode retVal = 0;

	if (!gy) {
		NULL_POINTER("gy");
		retVal = -1;
		goto END;
	}

	if (chip & CHIP_TYPE_MPU60X0) {
		if (gy->m_mpu) {
			//Trigger a reading

			retVal = mpu60x0_Update(gy->m_mpu);
			if (retVal) {
				ERROR("Can not Update the MPU Chip");
				goto END;
			}
		} else {
			ERROR("Can not Update the MPU Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_HMC5883) {
		if (gy->m_hmc) {
			retVal = hmc5883_Update(gy->m_hmc);
			if (retVal) {
				ERROR("Can not Update the HMC Chip");
				goto END;
			}
		} else {
			ERROR("Can not Update the HMC Chip, chip not initialized");
			goto END;
		}
	}

	if (chip & CHIP_TYPE_MS5611) {
		if (gy->m_ms) {
			retVal = ms5611_Update(gy->m_ms);
			if (retVal) {
				ERROR("Can not Update the MS Chip");
				goto END;
			}
		} else {
			ERROR("Can not Update the MS Chip, chip not initialized");
			goto END;
		}
	}

END:
	EXIT();
	return retVal;
}


mpuHandle gy86_getMpuChipHandle (gyHandle handle) {
	ENTER();

	Gy86 *gy = (Gy86 *)handle;

	if (!gy) {
		NULL_POINTER("gy");
		goto END;
	}

END:
	EXIT();
	return gy->m_mpu;
}

msHandle gy86_getMsChipHandle (gyHandle handle) {
	ENTER();

	Gy86 *gy = (Gy86 *)handle;

	if (!gy) {
		NULL_POINTER("gy");
		goto END;
	}

END:
	EXIT();
	return gy->m_ms;
}

hmcHandle gy86_getHmcChipHandle (gyHandle handle) {
	ENTER();

	Gy86 *gy = (Gy86 *)handle;

	if (!gy) {
		NULL_POINTER("gy");
		goto END;
	}

END:
	EXIT();
	return gy->m_hmc;
}


/**
 * Internal Functions
 */
static retcode resetGyData(Gy86 *gy) {
	ENTER();

	retcode retVal = 0;

	gy->m_mpu = NULL;
	gy->m_ms = NULL;
	gy->m_hmc = NULL;

//END:
	EXIT();
	return retVal;
}

static void mpuDataRdyIntHandler (void *handle, uint32 param) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)handle;
	float accData[3];
	float gyroData[3];
	float tempData[1];

	retVal = readMpuSensorData ((gyHandle)gy, accData, gyroData, tempData);
	if (retVal) {
		ERROR("Failed to Read the MPU Sensor Data");
	}

//END:
	EXIT();
	return;
}

static void mpuFifoIntHandler (void *handle, uint32 param) {
	ENTER();

	EXIT();
	return;
}

static void mpuMotDetIntHandler (void *handle, uint32 param) {
	ENTER();

	EXIT();
	return;
}

static void mpuFsyncIntHandler (void *handle, uint32 param) {
	ENTER();

	EXIT();
	return;
}

static void mpuAuxI2cIntHandler (void *handle, uint32 param) {
	ENTER();

	EXIT();
	return;
}


static void hmcDataRdyIntHandler (void *handle, uint32 param) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)handle;
	uint16 data[3];


	retVal = readHmcSensorData(gy, data);
	if (retVal) {
		ERROR("Failed to Collect the HMC Sensor Data");
	} else {
		INFO("Received Compass Sensor Data X = %d, Y= %d, Z=%d", data[0], data[1], data[2]);
	}

	EXIT();
	return;
}

static void hmcRegLockIntHandler (void *handle, uint32 param) {
	ENTER();

	EXIT();
	return;
}


static retcode readHmcSensorData ( gyHandle handle, uint16* data) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)handle;



	retVal = hmc5883_GetSensorData(gy->m_hmc, &data[0], &data[1], &data[2]);
	if (retVal) {
		ERROR("Failed to Collect the HMC Sensor Data");
		goto END;
	} else {
		INFO("Received Compass Sensor Data X = %d, Y= %d, Z=%d", data[0], data[1], data[2]);
	}

END:
	EXIT();
	return retVal;
}

static retcode readMpuSensorData ( gyHandle handle, float* accData, float* gyroData, float* tempData) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)handle;

	//First collect the data from the sensor registers or FIFO
	retVal = mpu60x0_ReadSensorData (gy->m_mpu);
	if (retVal) {
		ERROR("Failed to Collect the MPU Sensor Data");
		goto END;
	}

	//Calling to get the data
	retVal = mpu60x0_GetAccData(gy->m_mpu, &accData[0], &accData[1], &accData[2]);
	if (retVal) {
		ERROR("Failed to read Accelerometer Sensor Data");
	} else {
		INFO("Received Acc Sensor Data X = %d, Y= %d, Z=%d", accData[0], accData[1], data[2]);
	}

	retVal = mpu60x0_GetGyroData(gy->m_mpu, &gyroData[0], &gyroData[1], &gyroData[2]);
	if (retVal) {
		ERROR("Failed to read the Gyroscope Sensor Data");
	} else {
		INFO("Received Gyro Sensor Data X = %d, Y= %d, Z=%d", gyroData[0], gyroData[1], gyroData[2]);
	}

	retVal = mpu60x0_GetTempData (gy->m_mpu, &tempData[0]);
	if (retVal) {
		ERROR("Failed to read the Temperature Sensor Data");
	} else {
		INFO("Received Temperature Sensor Data Temp = %d", tempData[0]);
	}

//END:
	EXIT();
	return retVal;
}
