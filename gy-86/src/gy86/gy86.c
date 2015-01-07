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
//////////////////////////////

/**
 * Basic Functions
 */

gyHandle gy86_Init (Gy86_ChipType chip) {

	ENTER();

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

	if (chip & CHIP_TYPE_HMC5883) {
		gy->m_hmc = hmc5883_Init();
		if (!gy->m_hmc) {
			ERROR("Failed to Initialize the HMC Chip");
			goto FAIL_HMC;
		}
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



