/*
 * hmc-config.c
 *
 *  Created on: Jan 4, 2015
 *      Author: aelarabawy
 */

#include "logger.h"
#include "gy86_api.h"
#include "gy86-cli.h"

char *hmcConfigs[] = { "i2c-clk-speed",
		               "oper-mode",
		               "data-rate",
		               "osr",  //Over Sampling Ratio
		               "measure-mode",
		               "measure-gain",
		               "LAST_CONFIG" };

/**
 * Get functions prototypes
 */
static retcode handle_get_i2c_clk_speed (ConfigDataSrc);
static retcode handle_get_oper_mode (ConfigDataSrc);
static retcode handle_get_data_rate (ConfigDataSrc);
static retcode handle_get_osr (ConfigDataSrc);
static retcode handle_get_measure_mode (ConfigDataSrc);
static retcode handle_get_measure_gain (ConfigDataSrc);


retcode (*hmcGetHandlers[])(ConfigDataSrc) = { handle_get_i2c_clk_speed,
								               handle_get_oper_mode,
								               handle_get_data_rate,
									           handle_get_osr,
									           handle_get_measure_mode,
									           handle_get_measure_gain };

/**
 * Set functions prototypes
 */
static retcode handle_set_i2c_clk_speed (char *);
static retcode handle_set_oper_mode (char *);
static retcode handle_set_data_rate (char *);
static retcode handle_set_osr (char *);
static retcode handle_set_measure_mode (char *);
static retcode handle_set_measure_gain (char *);


retcode (*hmcSetHandlers[])(char *) = {	handle_set_i2c_clk_speed,
										handle_set_oper_mode,
										handle_set_data_rate,
										handle_set_osr,
										handle_set_measure_mode,
										handle_set_measure_gain };


static uint8 getConfigId (char *configStr) {
	ENTER();

	uint8 configId = 0;

	while (strcmp(hmcConfigs[configId], "LAST_CONFIG") != 0) {
		if (strcmp(configStr, hmcConfigs[configId]) != 0) {
			configId++;
		} else {
			break;
		}
	}

	if (strcmp(hmcConfigs[configId], "LAST_CONFIG") == 0) {
		ERROR("Can not find the configuration %s", configStr);
		PRINT_CLI("ERROR: Can not find the configuration %s", configStr);
		configId = CONFIG_INVALID;
		goto END;
	}

END:
	EXIT();
	return configId;
}

retcode handle_hmc_get (void) {
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
		retVal = hmcGetHandlers[configId](src);
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

retcode handle_hmc_set (void) {
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
			retVal = hmcSetHandlers[configId](configVal);
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


static retcode handle_get_i2c_clk_speed (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char clkStr [100];
	Hmc5883_I2cBusSpeed clk;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		clk = hmc5883_GetI2cBusSpeed(gy->m_hmc, src);
		retVal = hmc5883_ConvertI2cSpeed2String(clk, &clkStr);
		if (retVal) {
			ERROR("Failed to convert %d into an CLK Value", clk);
			PRINT_CLI("ERROR: Failed to convert %d into an CLK Value", clk);
			goto END;
		}

		INFO("I2C Bus CLK Speed = %d",clkStr);
		PRINT_CLI("I2C Bus CLK Speed = %d",clkStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_oper_mode (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char modeStr [100];
	Hmc5883_OperMode mode;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		mode = hmc5883_GetOperMode(gy->m_hmc, src);
		retVal = hmc5883_ConvertOperMode2String(mode, &modeStr);
		if (retVal) {
			ERROR("Failed to convert %d into an Operating Mode Value", mode);
			PRINT_CLI("ERROR: Failed to convert %d into an Operating Mode Value", mode);
			goto END;
		}

		INFO("Operating Mode = %d",modeStr);
		PRINT_CLI("Operating Mode = %d",modeStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_data_rate (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char rateStr [100];
	Hmc5883_DataOutRate rate;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		rate = hmc5883_GetDataOutRate(gy->m_hmc, src);
		retVal = hmc5883_ConvertDataRate2String(rate, &rateStr);
		if (retVal) {
			ERROR("Failed to convert %d into an Data Rate Value", rate);
			PRINT_CLI("ERROR: Failed to convert %d into an Data Rate Value", rate);
			goto END;
		}

		INFO("Data Rate = %d",rateStr);
		PRINT_CLI("Data Rate = %d",rateStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_osr (ConfigDataSrc src)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char osrStr [100];
	Hmc5883_OverSmplRatio osr;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		osr = hmc5883_GetOverSmplRatio(gy->m_hmc, src);
		retVal = hmc5883_ConvertOsr2String(osr, &osrStr);
		if (retVal) {
			ERROR("Failed to convert %d into an OSR Value", osr);
			PRINT_CLI("ERROR: Failed to convert %d into an OSR Value", osr);
			goto END;
		}

		INFO("Over Sampling Ratio = %d",osrStr);
		PRINT_CLI("Over Sampling Ratio = %d",osrStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_measure_mode (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char modeStr [100];
	Hmc5883_MeasureMode mode;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		mode = hmc5883_GetMeasureMode(gy->m_hmc, src);
		retVal = hmc5883_ConvertMeasureMode2String(mode, &modeStr);
		if (retVal) {
			ERROR("Failed to convert %d into an Measure Mode Value", mode);
			PRINT_CLI("ERROR: Failed to convert %d into an Measure Mode Value", mode);
			goto END;
		}

		INFO("Over Sampling Ratio = %d",modeStr);
		PRINT_CLI("Over Sampling Ratio = %d",modeStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_measure_gain (ConfigDataSrc src) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char gainStr [100];
	Hmc5883_MeasureGain gain;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		gain = hmc5883_GetMeasureGain(gy->m_hmc, src);
		retVal = hmc5883_ConvertMeasureGain2String(gain, &gainStr);
		if (retVal) {
			ERROR("Failed to convert %d into an Measure Gain Value", gain);
			PRINT_CLI("ERROR: Failed to convert %d into an Measure Gain Value", gain);
			goto END;
		}

		INFO("Over Sampling Ratio = %d",gainStr);
		PRINT_CLI("Over Sampling Ratio = %d",gainStr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_i2c_clk_speed (char *speedStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Hmc5883_I2cBusSpeed speed;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		speed = hmc5883_ParseBusSpeed(speedStr);
		if (speed == I2C_BUS_SPEED_INVALID) {
			ERROR("Invalid Bus Speed %s", speedStr);
			PRINT_CLI("ERROR: Invalid Bus Speed %s", speedStr);
			retVal = -1;
			goto END;
		}

		retVal = hmc5883_SetI2cBusSpeed(gy->m_hmc, speed);
		if (retVal) {
			ERROR("Failed to set the I2C Bus Speed ");
			PRINT_CLI("ERROR: Failed to set the I2C Bus Speed ");
			goto END;
		} else {
			INFO("Setting I2C Bus Speed for the HMC-5883 Chip");
			PRINT_CLI("Setting I2C Bus Speed for the HMC-5883 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_oper_mode (char *modeStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Hmc5883_OperMode mode;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		mode = hmc5883_ParseOperMode(modeStr);
		if (mode == OPER_MODE_INVALID) {
			ERROR("Invalid Operation Mode %s", modeStr);
			PRINT_CLI("ERROR: Invalid Operation Mode %s", modeStr);
			retVal = -1;
			goto END;
		}

		retVal = hmc5883_SetOperMode(gy->m_hmc, mode);
		if (retVal) {
			ERROR("Failed to set the Operating Mode ");
			PRINT_CLI("ERROR: Failed to set the Operating Mode ");
			goto END;
		} else {
			INFO("Setting Operating Mode for the HMC-5883 Chip");
			PRINT_CLI("Setting Operating Mode for the HMC-5883 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_data_rate (char *rateStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Hmc5883_DataOutRate rate;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		rate = hmc5883_ParseDataRate(rateStr);
		if (rate == DATA_OUT_RATE_INVALID) {
			ERROR("Invalid Data Rate %s", rateStr);
			PRINT_CLI("ERROR: Invalid Data Rate %s", rateStr);
			retVal = -1;
			goto END;
		}

		retVal = hmc5883_SetDataOutRate(gy->m_hmc, rate);
		if (retVal) {
			ERROR("Failed to set the Data Rate ");
			PRINT_CLI("ERROR: Failed to set the Data Rate ");
			goto END;
		} else {
			INFO("Setting Data Rate for the HMC-5883 Chip");
			PRINT_CLI("Setting Data Rate for the HMC-5883 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_osr (char *osrStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Hmc5883_OverSmplRatio osr;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		osr = hmc5883_ParseOsr(osrStr);
		if (osr == OVER_SMPL_RATIO_INVALID) {
			ERROR("Invalid Over Sampling Ratio %s", osrStr);
			PRINT_CLI("ERROR: Invalid Over Sampling Ratio %s", osrStr);
			retVal = -1;
			goto END;
		}

		retVal = hmc5883_SetOverSmplRatio(gy->m_hmc, osr);
		if (retVal) {
			ERROR("Failed to set the Over Sampling Ratio ");
			PRINT_CLI("ERROR: Failed to set the Over Sampling Ratio ");
			goto END;
		} else {
			INFO("Setting Over Sampling Ratio for the HMC-5883 Chip");
			PRINT_CLI("Setting Over Sampling Ratio for the HMC-5883 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_measure_mode (char *modeStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Hmc5883_MeasureMode mode;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		mode = hmc5883_ParseMeasureMode(modeStr);
		if (mode == MEASURE_MODE_INVALID) {
			ERROR("Invalid Measurement Mode %s", modeStr);
			PRINT_CLI("ERROR: Invalid Measurement Mode %s", modeStr);
			retVal = -1;
			goto END;
		}

		retVal = hmc5883_SetMeasureMode(gy->m_hmc, mode);
		if (retVal) {
			ERROR("Failed to set the Measurement Mode ");
			PRINT_CLI("ERROR: Failed to set the Measurement Mode ");
			goto END;
		} else {
			INFO("Setting Measurement Mode for the HMC-5883 Chip");
			PRINT_CLI("Setting Measurement Mode for the HMC-5883 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_measure_gain (char *gainStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Hmc5883_MeasureGain gain;

	if (!gy->m_hmc) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		gain = hmc5883_ParseMeasureGain(gainStr);
		if (gain == MEASURE_GAIN_INVALID) {
			ERROR("Invalid Measurement Gain %s", gainStr);
			PRINT_CLI("ERROR: Invalid Measurement Gain %s", gainStr);
			retVal = -1;
			goto END;
		}

		retVal = hmc5883_SetMeasureGain(gy->m_hmc, gain);
		if (retVal) {
			ERROR("Failed to set the Measurement Gain ");
			PRINT_CLI("ERROR: Failed to set the Measurement Gain ");
			goto END;
		} else {
			INFO("Setting Measurement Gain for the HMC-5883 Chip");
			PRINT_CLI("Setting Measurement Gain for the HMC-5883 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}


