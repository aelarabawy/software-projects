/*
 * mpu-config.c
 *
 *  Created on: Jan 4, 2015
 *      Author: aelarabawy
 */

#include "logger.h"
#include "gy86_api.h"
#include "gy86-cli.h"

char *mpuConfigs[] = { "i2c-addr",
		               "clk-src",
					   "sampling-rate",
					   "lpf",
					   "acc-fsr",
					   "gyro-fsr",
					   "fifo-enable",
					   "mot-det-thr",
					   "acc-enable",
					   "gyro-enable",
					   "temp-enable",
					   "int-lvl",
					   "int-open",
					   "int-latch",
					   "int-clear-on-read",
					   "aux-i2c-mode",
					   "aux-i2c-clk",
                       "LAST_CONFIG"};


/**
 * Slave Configuration
 * Note: In this release, I will not support Slaves
 * TODO support slaves
 */
char *mpuSlvConfigs[] = { "LAST_CONFIG"};


/**
 * Get functions prototypes
 */
static retcode handle_get_i2c_addr (ConfigDataSrc);
static retcode handle_get_clk_src (ConfigDataSrc);
static retcode handle_get_smpl_rate (ConfigDataSrc);
static retcode handle_get_lpf (ConfigDataSrc);
static retcode handle_get_acc_fsr (ConfigDataSrc);
static retcode handle_get_gyro_fsr (ConfigDataSrc);
static retcode handle_get_fifo_enable (ConfigDataSrc);
static retcode handle_get_mot_det_thr (ConfigDataSrc);
static retcode handle_get_acc_enable (ConfigDataSrc);
static retcode handle_get_gyro_enable (ConfigDataSrc);
static retcode handle_get_temp_enable (ConfigDataSrc);
static retcode handle_get_int_lvl (ConfigDataSrc);
static retcode handle_get_int_open (ConfigDataSrc);
static retcode handle_get_int_latch (ConfigDataSrc);
static retcode handle_get_int_clear_on_read (ConfigDataSrc);
static retcode handle_get_aux_i2c_mode (ConfigDataSrc);
static retcode handle_get_aux_i2c_clk (ConfigDataSrc);

retcode (*mpuGetHandlers[])(void) = { handle_get_i2c_addr,
		                              handle_get_clk_src,
								      handle_get_smpl_rate,
								      handle_get_lpf,
								      handle_get_acc_fsr,
								      handle_get_gyro_fsr,
									  handle_get_fifo_enable,
									  handle_get_mot_det_thr,
									  handle_get_acc_enable,
									  handle_get_gyro_enable,
									  handle_get_temp_enable,
									  handle_get_int_lvl,
									  handle_get_int_open,
									  handle_get_int_latch,
									  handle_get_int_clear_on_read,
									  handle_get_aux_i2c_mode,
									  handle_get_aux_i2c_clk};

/**
 * Set functions prototypes
 */
static retcode handle_set_i2c_addr (char *);
static retcode handle_set_clk_src (char *);
static retcode handle_set_smpl_rate (char *);
static retcode handle_set_lpf (char *);
static retcode handle_set_acc_fsr (char *);
static retcode handle_set_gyro_fsr (char *);
static retcode handle_set_fifo_enable (char *);
static retcode handle_set_mot_det_thr (char *);
static retcode handle_set_acc_enable (char *);
static retcode handle_set_gyro_enable (char *);
static retcode handle_set_temp_enable (char *);
static retcode handle_set_int_lvl (char *);
static retcode handle_set_int_open (char *);
static retcode handle_set_int_latch (char *);
static retcode handle_set_int_clear_on_read (char *);
static retcode handle_set_aux_i2c_mode (char *);
static retcode handle_set_aux_i2c_clk (char *);

retcode (*mpuSetHandlers[])(char *) = { handle_set_i2c_addr,
		                                handle_set_clk_src,
		 							    handle_set_smpl_rate,
									    handle_set_lpf,
									    handle_set_acc_fsr,
									    handle_set_gyro_fsr,
									    handle_set_fifo_enable,
									    handle_set_mot_det_thr,
									    handle_set_acc_enable,
									    handle_set_gyro_enable,
									    handle_set_temp_enable,
									    handle_set_int_lvl,
									    handle_set_int_open,
									    handle_set_int_latch,
									    handle_set_int_clear_on_read,
									    handle_set_aux_i2c_mode,
									    handle_set_aux_i2c_clk};


static uint8 getConfigId (char *configStr) {
	ENTER();

	uint8 configId = 0;

	while (strcmp(mpuConfigs[configId], "LAST_CONFIG") != 0) {
		if (strcmp(configStr, mpuConfigs[configId]) != 0) {
			configId++;
		} else {
			break;
		}
	}

	if (strcmp(mpuConfigs[configId], "LAST_CONFIG") == 0) {
		ERROR("Can not find the configuration %s", configStr);
		PRINT_CLI("ERROR: Can not find the configuration %s", configStr);
		configId = CONFIG_INVALID;
		goto END;
	}

END:
	EXIT();
	return configId;
}

retcode handle_mpu_get (void) {
	ENTER();

	retcode retVal = 0;

	//Get the config
	char *configStr = strtok(NULL, " ");
	if (configStr != NULL) {
		uint8 configId = getConfigId(configStr);
		if (configId == CONFIG_INVALID) {
			ERROR("Invalid Config %s", configStr);
			PRINT_CLI("ERROR: Invalid Config %s", configStr);
			retVal = -1;
			goto END;
		}

		//Getting the source of configuration
		ConfigDataSrc src;
		char *srcStr = strtok(NULL, " ");
		if (srcStr != NULL) {
			if (strcmp(srcStr, "configs") == 0) {
				src = DATA_SRC_CONFIG;
			} else if (strcmp(srcStr, "active") == 0) {
				src = DATA_SRC_ACTIVE;
			} else if (strcmp(srcStr, "hw") == 0) {
				src = DATA_SRC_HW;
			} else {
				ERROR("Invalid Configuration source %s .... Exiting", srcStr);
				retVal = -1;
				goto END;
			}
		} else {
			WARN("No Source of Configuration Provided, assuming from new configs");
			src = DATA_SRC_CONFIG;
		}

		//Execute the corresponding Function
		retVal = mpuGetHandlers[configId](src);
		if (retVal) {
			ERROR("Failed to Get the configuration %s", configStr);
			PRINT_CLI("ERROR: Failed to Get the configuration %s", configStr);
			goto END;
		}
	} else {
		ERROR("No Config name in param String");
		PRINT_CLI("ERROR: No Config name in param String");
		retVal = -1;
		goto END;
	}

END:
	EXIT();
	return retVal;
}

retcode handle_mpu_set (void) {
	ENTER();

	retcode retVal = 0;

	//Get the config
	char *configStr = strtok(NULL, "=");
	if (configStr != NULL) {
		uint8 configId = getConfigId(configStr);
		if (configId == CONFIG_INVALID) {
			ERROR("Invalid Config %s", configStr);
			PRINT_CLI("ERROR: Invalid Config %s", configStr);
			retVal = -1;
			goto END;
		}

		//Now get the parameter value
		char *configVal = strtok(NULL, " ");
		if (configVal != NULL) {
			retVal = mpuSetHandlers[configId](configVal);
			if (retVal) {
				ERROR("Failed to Set the configuration %s to the value %s", configStr, configVal);
				PRINT_CLI("ERROR: Failed to Set the configuration %s to the value %s", configStr, configVal);
				goto END;
			}
		} else {
			ERROR("No Config name in param String");
			PRINT_CLI("ERROR: No Config name in param String");
			retVal = -1;
			goto END;
		}
	}

END:
	EXIT();
	return retVal;

}




#if 0

	//Get the config
	char *configStr = strtok(NULL, " ");
	if (configStr != NULL){
		uint8 configId = getConfigId(configStr);
		if (configId == CONFIG_INVALID) {
			ERROR("Invalid Config %s", configStr);
			goto END;
		}

		//Execute the corresponding Function
		getConfigHandlers[configId]();
	} else {
		ERROR("No Config name in param String");
		goto END;
	}

	switch (chip) {
	case CHIP_TYPE_MPU60X0:

		break;

	case CHIP_TYPE_MPU60X0:
		break;

	case CHIP_TYPE_MPU60X0:
		break;

	default:
		ERROR("Invalid Chip type ");
	}


	if (chip == CHIP_TYPE_MPU60X0) {
		//Get the part to reset (or all)
		char *token = strtok(NULL, " ");
		if (token != NULL){
			mode = mpu60x0_parseResetMode(token);

			if (mode == RESET_MODE_INVALID) {
				ERROR("Invalid Reset Mode ...Exiting");
				goto END;
			}
		}
	}

	retcode retVal = gy86_Reset(g_gyHandle, chip, mode);
	if (retVal) {
		ERROR("Failed to Reset the GY-86 Module chips");
		goto END;
	}
#endif



#if 0
//Getting the Parameter Name
char *param = strtok(NULL, "=");
if (param != NULL){
	INFO("Parameter Name is BEGIN%sEND" , param);
} else {
	ERROR("No Parameter Name Passed");
	goto END;
}

//Get the chip to start
token = strtok(NULL, " ");
if (token != NULL){
	INFO("Passed argument is %s", token);
}

	//Get the Argument of the command
	token = strtok(NULL," ");
	if (token != NULL){
		func1 (token);
	} else {
		printf("Invalid Entry, argument not provided\n");
	}
#endif



static retcode handle_get_i2c_addr (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char addrStr [100];
	Mpu60x0_I2cAddr addr;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		addr = mpu60x0_GetI2cAddr(gy->m_mpu);
		retVal = hmc5883_ConvertI2cSpeed2String(clk, &clkStr);
		if (retVal) {
			ERROR("Failed to convert %d into an CLK Value", clk);
			PRINT_CLI("ERROR: Failed to convert %d into an CLK Value", clk);
			goto END;
		}

		INFO("I2C Addr = 0x%x",addr);
		PRINT_CLI("I2C Addr = 0x%x",addr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_clk_src (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char clkStr [100];
	Mpu60x0_ClkSrc clk;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		clk = mpu60x0_GetClkSrc(gy->m_mpu, src);
		retVal = mpu60x0_ConvertClkSrc2String(clk, &clkStr);
		if (retVal) {
			ERROR("Failed to convert %d into an CLK Src", clk);
			PRINT_CLI("ERROR: Failed to convert %d into an CLK src", clk);
			goto END;
		}

		INFO("MPU CLK Source = %s",clkStr);
		PRINT_CLI("I2C Bus CLK Source = %s",clkStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_smpl_rate (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint32 rate;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		rate = mpu60x0_GetSamplingRate(gy->m_mpu, src);

		INFO("MPU Sampling Rate = %d",rate);
		PRINT_CLI("MPU Sampling Rate = %d",rate);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_lpf (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char lpfStr [100];
	Mpu60x0_Lpf lpf;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		lpf = mpu60x0_GetLpf(gy->m_mpu, src);
		retVal = mpu60x0_ConvertLpf2String(lpf, &lpfStr);
		if (retVal) {
			ERROR("Failed to convert %d into a LPF Value", lpf);
			PRINT_CLI("ERROR: Failed to convert %d into a LPF value", lpf);
			goto END;
		}

		INFO("MPU CLK Source = %s",lpfStr);
		PRINT_CLI("I2C Bus CLK Source = %s",lpfStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_acc_fsr (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char fsrStr [100];
	Mpu60x0_AccFullScale fsr;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		fsr = mpu60x0_GetAccFullScale(gy->m_mpu, src);
		retVal = mpu60x0_ConvertAccFsr2String(fsr, &fsrStr);
		if (retVal) {
			ERROR("Failed to convert %d into a Accelerometer Full Scale Range Value", fsr);
			PRINT_CLI("ERROR: Failed to convert %d into a Accelerometer Full Scale Range value", fsr);
			goto END;
		}

		INFO("MPU Accelerometer Full Scale Range = %s",fsrStr);
		PRINT_CLI("Accelerometer Full Scale Range = %s",fsrStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_gyro_fsr (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char fsrStr [100];
	Mpu60x0_GyroFullScale fsr;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		fsr = mpu60x0_GetGyroFullScale(gy->m_mpu, src);
		retVal = mpu60x0_ConvertGyroFsr2String(fsr, &fsrStr);
		if (retVal) {
			ERROR("Failed to convert %d into a Gyroscope Full Scale Range Value", fsr);
			PRINT_CLI("ERROR: Failed to convert %d into a Gyroscope Full Scale Range value", fsr);
			goto END;
		}

		INFO("MPU Gyroscope Full Scale Range = %s",fsrStr);
		PRINT_CLI("Gyroscope Full Scale Range = %s",fsrStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_fifo_enable (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool fifo;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		fifo = mpu60x0_GetFifoUsage(gy->m_mpu, src);
		if (fifo) {
			INFO("MPU FIFO is Enabled");
			PRINT_CLI("MPU FIFO is Enabled");
		} else {
			INFO("MPU FIFO is Disabled");
			PRINT_CLI("MPU FIFO is Disabled");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_mot_det_thr (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint8 thr;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		thr = mpu60x0_GetMotDetThr(gy->m_mpu, src);

		INFO("MPU Motion Detection Threshold = %d",thr);
		PRINT_CLI("MPU Motion Detection Threshold = %d",thr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_acc_enable (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint8 sensors;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	}


	sensors = mpu60x0_GetActiveSensors(gy->m_mpu, src);
	sensors &= SENSOR_ACC_ALL;
	INFO("Accelerometer Support is 0x%x", sensors);
	PRINT_CLI("Accelerometer Support is 0x%x", sensors);

END:
	EXIT();
	return retVal;
}

static retcode handle_get_gyro_enable (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint8 sensors;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	}

	sensors = mpu60x0_GetActiveSensors(gy->m_mpu, src);
	sensors &= SENSOR_GYRO_ALL;
	sensors >>= 3;
	INFO("Gyroscope Support is 0x%x", sensors);
	PRINT_CLI("Gyroscope Support is 0x%x", sensors);

END:
	EXIT();
	return retVal;
}

static retcode handle_get_temp_enable (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint8 sensors;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	}

	sensors = mpu60x0_GetActiveSensors(gy->m_mpu, src);
	if (sensors & SENSOR_TEMP) {
		INFO("Temperature Sensor is Supported");
		PRINT_CLI("Temperature Sensor is Supported");
	} else {
		INFO("Temperature Sensor is Not Supported");
		PRINT_CLI("Temperature Sensor is Not Supported");
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_int_lvl (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char lvlStr [100];
	Mpu60x0_IntLvl lvl;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		lvl = mpu60x0_GetIntLvl(gy->m_mpu, src);
		retVal = mpu60x0_ConvertIntLvl2String(lvl, &lvlStr);
		if (retVal) {
			ERROR("Failed to convert %d into an Interrupt Level ", lvl);
			PRINT_CLI("ERROR: Failed to convert %d into an Interrupt Level ", fsr);
			goto END;
		}

		INFO("MPU Interrupt Level = %s",lvlStr);
		PRINT_CLI("MPU Interrupt Level = %s",lvlStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_int_open (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool isOpen;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		isOpen = mpu60x0_GetIntOpen(gy->m_mpu, src);
		if (isOpen) {
			INFO("Interrupt Signal is Open Drain");
			PRINT_CLI("Interrupt Signal is Open Drain");
		} else {
			INFO("Interrupt Signal is Not Open Drain");
			PRINT_CLI("Interrupt Signal is Not Open Drain");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_int_latch (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool isLatch;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		isLatch = mpu60x0_GetIntLatch(gy->m_mpu, src);
		if (isLatch) {
			INFO("Interrupt Signal is Latched until cleared");
			PRINT_CLI("Interrupt Signal is Latched until cleared");
		} else {
			INFO("Interrupt Signal is Not Latched (simple 250 us Pulse)");
			PRINT_CLI("Interrupt Signal is Not Latched (simple 250 us Pulse)");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_int_clear_on_read (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool clearOnRead;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		clearOnRead = mpu60x0_GetIntClearOnRead(gy->m_mpu, src);
		if (clearOnRead) {
			INFO("Interrupt Signal is Cleared on any Register read");
			PRINT_CLI("Interrupt Signal is Cleared on any Register read");
		} else {
			INFO("Interrupt Signal is only Cleared on Reading the Interrupt Status Register");
			PRINT_CLI("Interrupt Signal is only Cleared on Reading the Interrupt Status Register");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_aux_i2c_mode (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char modeStr [100];
	Mpu60x0_AuxI2cMode mode;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		mode = mpu60x0_GetAuxI2c(gy->m_mpu, src);
		retVal = mpu60x0_ConvertAuxI2cMode2String(mode, &modeStr);
		if (retVal) {
			ERROR("Failed to convert %d into a Auxiliary I2C Mode Value", mode);
			PRINT_CLI("ERROR: Failed to convert %d into a Auxiliary I2C Mode value", mode);
			goto END;
		}

		INFO("Auxiliary I2C Bus Mode = %s",modeStr);
		PRINT_CLI("Auxiliary I2C Bus Mode = %s",modeStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_aux_i2c_clk (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char clkStr [100];
	Mpu60x0_AuxI2cClk clk;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		clk = mpu60x0_GetAuxI2cClk(gy->m_mpu, src);
		retVal = mpu60x0_ConvertAuxI2cClk2String(clk, &clkStr);
		if (retVal) {
			ERROR("Failed to convert %d into a Auxiliary I2C Clk Value", clk);
			PRINT_CLI("ERROR: Failed to convert %d into a Auxiliary I2C Clk value", clk);
			goto END;
		}

		INFO("Auxiliary I2C Bus Clock = %s",clkStr);
		PRINT_CLI("Auxiliary I2C Bus Clock = %s",clkStr);
	}

END:
	EXIT();
	return retVal;
}


static retcode handle_set_i2c_addr (char *addrStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_I2cAddr addr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		addr = mpu60x0_ParseI2cAddr(addrStr);
		if (addr == I2C_ADDR_INVALID) {
			ERROR("Invalid I2C Address %s", addrStr);
			PRINT_CLI("ERROR: Invalid I2C Address %s", addrStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetI2cAddr(gy->m_mpu, addr);
		if (retVal) {
			ERROR("Failed to set the I2C Address ");
			PRINT_CLI("ERROR: Failed to set the I2C Address ");
			goto END;
		} else {
			INFO("Setting the I2C Address for the MPU-60X0 Chip");
			PRINT_CLI("Setting the I2C Address for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_clk_src (char *clkStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_ClkSrc clk;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		clk = mpu60x0_ParseClkSrc(clkStr);
		if (clk == CLK_SRC_INVALID) {
			ERROR("Invalid clk source %s", clkStr);
			PRINT_CLI("ERROR: Invalid Clock Source %s", clkStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetClkSrc(gy->m_mpu, clk);
		if (retVal) {
			ERROR("Failed to set the Clock Source ");
			PRINT_CLI("ERROR: Failed to set the Clock Source ");
			goto END;
		} else {
			INFO("Setting the Clock Source for the MPU-60X0 Chip");
			PRINT_CLI("Setting the Clock Source for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_smpl_rate (char *rateStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint32 rate;
	char *endPtr;
	Mpu60x0_Lpf lpf;

	if (!gy->m_mpu) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		lpf = mpu60x0_GetLpf(gy->m_mpu, DATA_SRC_CONFIG);
		rate = (uint32) strtol(rateStr, &endPtr, 10);
		if (endPtr == rateStr) {
			ERROR("Invalid value for Sampling rate %s", rateStr);
			PRINT_CLI("ERROR: Invalid value for Sampling rate %s", rateStr);
			retVal = -1;
			goto END;
		}

		rate = mpu60x0_SetSamplingRate(gy->m_mpu, rate, lpf);
		INFO("Setting the Sampling Rate for the MPU-60X0 Chip to %d", rate);
		PRINT_CLI("Setting the Sampling Rate for the MPU-60X0 Chip to %d", rate);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_lpf (char *lpfStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_Lpf lpf;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		lpf = mpu60x0_ParseLpf(lpfStr);
		if (lpf == LPF_INVALID) {
			ERROR("Invalid LPF %s", lpfStr);
			PRINT_CLI("ERROR: Invalid LPF %s", lpfStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetLpf(gy->m_mpu, lpf);
		if (retVal) {
			ERROR("Failed to set the LPF ");
			PRINT_CLI("ERROR: Failed to set the LPF ");
			goto END;
		} else {
			INFO("Setting the LPF for the MPU-60X0 Chip");
			PRINT_CLI("Setting the LPF for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_acc_fsr (char *fsrStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_GyroFullScale fsr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		fsr = mpu60x0_ParseGyroFsr(fsrStr);
		if (fsr == GYRO_FULL_SCALE_INVALID) {
			ERROR("Invalid Gyro FSR %s", fsrStr);
			PRINT_CLI("ERROR: Invalid Gyro FSR %s", fsrStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetGyroFullScale(gy->m_mpu, fsr);
		if (retVal) {
			ERROR("Failed to set the Gyro FSR ");
			PRINT_CLI("ERROR: Failed to set the Gyro FSR ");
			goto END;
		} else {
			INFO("Setting the Gyro FSR for the MPU-60X0 Chip");
			PRINT_CLI("Setting the Gyro FSR for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_gyro_fsr (char *fsrStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_AccFullScale fsr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		fsr = mpu60x0_ParseAccFsr(fsrStr);
		if (fsr == ACC_FULL_SCALE_INVALID) {
			ERROR("Invalid ACC FSR %s", fsrStr);
			PRINT_CLI("ERROR: Invalid ACC FSR %s", fsrStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetAccFullScale(gy->m_mpu, fsr);
		if (retVal) {
			ERROR("Failed to set the ACC FSR ");
			PRINT_CLI("ERROR: Failed to set the ACC FSR ");
			goto END;
		} else {
			INFO("Setting the ACC FSR for the MPU-60X0 Chip");
			PRINT_CLI("Setting the ACC FSR for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_fifo_enable (char *fifoStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool fifo;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		if (strcmp(fifoStr, "true")) {
			fifo = TRUE;
		} else {
			fifo = FALSE;
		}

		retVal = mpu60x0_SetFifoUsage(gy->m_mpu, fifo);
		if (retVal) {
			ERROR("Failed to Enable/Disable the FIFO ");
			PRINT_CLI("ERROR: Failed to Enable/Disable the FIFO ");
			goto END;
		} else {
			INFO("Setting the FIFO Flag for the MPU-60X0 Chip");
			PRINT_CLI("Setting the FIFO Flag for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_mot_det_thr (char *) {
	//TODO
	return 0;
}
static retcode handle_set_acc_enable (char *) {
	//TODO

	return 0;
}

static retcode handle_set_gyro_enable (char *) {
	//TODO
	return 0;
}

static retcode handle_set_temp_enable (char *) {
	//TODO
	return 0;
}


static retcode handle_set_int_lvl (char *lvlStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_IntLvl lvl;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		lvl = mpu60x0_ParseIntLvl(lvlStr);
		if (lvl == INT_LVL_INVALID) {
			ERROR("Invalid Interrupt Level %s", lvlStr);
			PRINT_CLI("ERROR: Invalid Interrupt Level %s", lvlStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetIntSignalLvl(gy->m_mpu, lvl);
		if (retVal) {
			ERROR("Failed to set the Interrupt Signal Level ");
			PRINT_CLI("ERROR: Failed to set the Interrupt Signal Level ");
			goto END;
		} else {
			INFO("Setting Interrupt Signal Level for the MPU-60X0 Chip");
			PRINT_CLI("Setting Interrupt Signal Level for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}


static retcode handle_set_int_open (char *openStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool isOpen;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		if (strcmp(openStr, "true")) {
			isOpen = TRUE;
		} else {
			isOpen = FALSE;
		}

		retVal = mpu60x0_SetIntOpenDrain(gy->m_mpu, isOpen);
		if (retVal) {
			ERROR("Failed to set the Open Drain Flag for the Interrupt Signal ");
			PRINT_CLI("ERROR: Failed to set the Open Drain Flag for the Interrupt Signal ");
			goto END;
		} else {
			INFO("Setting the Open Drain Flag for the Interrupt Signal for the MPU-60X0 Chip");
			PRINT_CLI("Setting the Open Drain Flag for the Interrupt Signal for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_int_latch (char *latchStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool latch;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		if (strcmp(latchStr, "true")) {
			latch = TRUE;
		} else {
			latch = FALSE;
		}

		retVal = mpu60x0_SetIntLatch(gy->m_mpu, latch);
		if (retVal) {
			ERROR("Failed to set the Latch Flag for the Interrupt Signal ");
			PRINT_CLI("ERROR: Failed to set the Latch Flag for the Interrupt Signal ");
			goto END;
		} else {
			INFO("Setting the Latch Flag for the Interrupt Signal for the MPU-60X0 Chip");
			PRINT_CLI("Setting the Latch Flag for the Interrupt Signal for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_int_clear_on_read (char *clearStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	bool clearOnRead;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		if (strcmp(clearStr, "true")) {
			clearOnRead = TRUE;
		} else {
			clearOnRead = FALSE;
		}

		retVal = mpu60x0_SetIntClearOnRead(gy->m_mpu, clearOnRead);
		if (retVal) {
			ERROR("Failed to set the Clear on Read Flag for the Interrupt Signal ");
			PRINT_CLI("ERROR: Failed to set the Clear on Read Flag for the Interrupt Signal ");
			goto END;
		} else {
			INFO("Setting the Clear on Read Flag for the Interrupt Signal for the MPU-60X0 Chip");
			PRINT_CLI("Setting the Clear on Read Flag for the Interrupt Signal for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_aux_i2c_mode (char *modeStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_AuxI2cMode mode;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		mode = mpu60x0_ParseAuxI2cMode(modeStr);
		if (mode == AUX_I2C_INVALID) {
			ERROR("Invalid Auxiliary I2C Mode %s", modeStr);
			PRINT_CLI("ERROR: Invalid Auxiliary I2C Mode %s", modeStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetAuxI2c(gy->m_mpu, mode);
		if (retVal) {
			ERROR("Failed to set the Auxiliary I2C Bus Mode ");
			PRINT_CLI("ERROR: Failed to set the Auxiliary I2C Bus Mode ");
			goto END;
		} else {
			INFO("Setting Auxiliary I2C Bus Mode for the MPU-60X0 Chip");
			PRINT_CLI("Setting Auxiliary I2C Bus Mode for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}


static retcode handle_set_aux_i2c_clk (char *clkStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Mpu60x0_AuxI2cClk clk;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		clk = mpu60x0_ParseAuxI2cClk(clkStr);
		if (clk == AUX_I2C_CLK_INVALID) {
			ERROR("Invalid Auxiliary I2C Clock %s", clkStr);
			PRINT_CLI("ERROR: Invalid Auxiliary I2C Clock %s", clkStr);
			retVal = -1;
			goto END;
		}

		retVal = mpu60x0_SetAuxI2cClk(gy->m_mpu, clk);
		if (retVal) {
			ERROR("Failed to set the Auxiliary I2C Bus Clock Speed ");
			PRINT_CLI("ERROR: Failed to set the Auxiliary I2C Bus Clock Speed ");
			goto END;
		} else {
			INFO("Setting Auxiliary I2C Bus Clock Speed for the MPU-60X0 Chip");
			PRINT_CLI("Setting Auxiliary I2C Bus Clock Speed for the MPU-60X0 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}
