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
 * Get functions prototypes
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

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		clk = hmc5883_GetI2cBusSpeed(gy->m_ms, src);
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

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		mode = hmc5883_GetOperMode(gy->m_ms, src);
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

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		rate = hmc5883_GetDataOutRate(gy->m_ms, src);
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

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		osr = hmc5883_GetOverSmplRatio(gy->m_ms, src);
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

static retcode handle_get_measure_mode (ConfigDataSrc src);
static retcode handle_get_measure_gain (ConfigDataSrc src);

static retcode handle_get_i2c_clk_speed (void);
static retcode handle_get_oper_mode (void);
static retcode handle_get_data_rate (void);
static retcode handle_get_osr (void);
static retcode handle_get_measure_mode (void);
static retcode handle_get_measure_gain (void);


