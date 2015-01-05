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
static retcode handle_get_i2c_clk_speed (void);
static retcode handle_get_oper_mode (void);
static retcode handle_get_data_rate (void);
static retcode handle_get_osr (void);
static retcode handle_get_measure_mode (void);
static retcode handle_get_measure_gain (void);


retcode (*hmcGetHandlers[])(void) = { handle_get_i2c_clk_speed,
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
		//Execute the corresponding Function
		retVal = hmcGetHandlers[configId]();
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


static retcode handle_get_i2c_clk_speed (void);
static retcode handle_get_oper_mode (void);
static retcode handle_get_data_rate (void);
static retcode handle_get_osr (void);
static retcode handle_get_measure_mode (void);
static retcode handle_get_measure_gain (void);

static retcode handle_get_i2c_clk_speed (void);
static retcode handle_get_oper_mode (void);
static retcode handle_get_data_rate (void);
static retcode handle_get_osr (void);
static retcode handle_get_measure_mode (void);
static retcode handle_get_measure_gain (void);


