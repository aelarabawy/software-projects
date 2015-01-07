/*
 * ms-config.c
 *
 *  Created on: Jan 4, 2015
 *      Author: aelarabawy
 */

#include "logger.h"
#include "gy86_api.h"
#include "gy86-cli.h"


char *msConfigs[] = {"i2c-addr",
		             "sens", //Pressure Sensitivity
					 "off",  //Pressure Offset
					 "tcs",  //Temperature Coefficient of Pressure Sensitivity
					 "tco",  //Temperature Coefficient of Pressure Offset
					 "tref", //Reference Temperature
					 "tempsens", //Temperature Coefficient of the Temperature
					 "p-osr",
					 "temp-osr",
					 "LAST_CONFIG"};


/**
 * Get functions prototypes
 */
static retcode handle_get_i2c_addr (void);
static retcode handle_get_sens (void);
static retcode handle_get_off (void);
static retcode handle_get_tcs (void);
static retcode handle_get_tco (void);
static retcode handle_get_tref (void);
static retcode handle_get_tempsens (void);
static retcode handle_get_pOsr (void);
static retcode handle_get_tempOsr (void);


retcode (*msGetHandlers[])(void) = {handle_get_i2c_addr,
									handle_get_sens,
									handle_get_off,
									handle_get_tcs,
									handle_get_tco,
									handle_get_tref,
									handle_get_tempsens,
									handle_get_pOsr,
									handle_get_tempOsr };

/**
 * Set functions prototypes
 */
static retcode handle_set_i2c_addr (char *);
static retcode handle_set_sens (char *);
static retcode handle_set_off (char *);
static retcode handle_set_tcs (char *);
static retcode handle_set_tco (char *);
static retcode handle_set_tref (char *);
static retcode handle_set_tempsens (char *);
static retcode handle_set_pOsr (char *);
static retcode handle_set_tempOsr (char *);


retcode (*msSetHandlers[])(char *) = {handle_set_i2c_addr,
									  handle_set_sens,
									  handle_set_off,
									  handle_set_tcs,
									  handle_set_tco,
									  handle_set_tref,
									  handle_set_tempsens,
									  handle_set_pOsr,
									  handle_set_tempOsr };


static uint8 getConfigId (char *configStr) {
	ENTER();

	uint8 configId = 0;

	while (strcmp(msConfigs[configId], "LAST_CONFIG") != 0) {
		if (strcmp(configStr, msConfigs[configId]) != 0) {
			configId++;
		} else {
			break;
		}
	}

	if (strcmp(msConfigs[configId], "LAST_CONFIG") == 0) {
		ERROR("Can not find the configuration %s", configStr);
		PRINT_CLI("ERROR: Can not find the configuration %s", configStr);
		configId = CONFIG_INVALID;
		goto END;
	}

END:
	EXIT();
	return configId;
}

retcode handle_ms_get (void) {
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
		retVal = msGetHandlers[configId]();
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

retcode handle_ms_set (void) {
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
			retVal = msSetHandlers[configId](configVal);
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


static retcode handle_get_i2c_addr (void) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint8 addr;


	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		addr = ms5611_GetI2cAddr(gy->m_ms);
		INFO("I2C Address is 0x%x", addr);
		PRINT_CLI("I2C Address is 0x%x", addr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_sens (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint16 sens;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		sens = ms5611_GetPressureSens(gy->m_ms);
		INFO("Pressure Sensitivity(SENS) = %d",sens);
		PRINT_CLI("Pressure Sensitivity(SENS) = %d",sens);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_off (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint16 off;


	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		off = ms5611_GetPressureOffset(gy->m_ms);
		INFO("Pressure Offset (OFF) = %d" ,off);
		PRINT_CLI("Pressure Offset (OFF) = %d" ,off);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_tcs (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint16 tcs;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		tcs = ms5611_GetTempCoffPressureSens(gy->m_ms);
		INFO("Temperature Coefficient of Pressure Sensitivity (TCS) = %d",tcs);
		PRINT_CLI("Temperature Coefficient of Pressure Sensitivity (TCS) = %d",tcs);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_tco (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint16 tco;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		tco = ms5611_GetTempCoffPressureOffset(gy->m_ms);
		INFO("Temperature Coefficient of Pressure Offset(TCO) = %d",tco);
		PRINT_CLI("Temperature Coefficient of Pressure Offset (TCO) = %d",tco);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_tref (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint16 tref;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		tref = ms5611_GetRefTemp(gy->m_ms);
		INFO("Reference Temperature (TREF) = %d",tref);
		PRINT_CLI("Reference Temperature (TREF) = %d",tref);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_tempsens (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	uint16 tempsens;

	if (!gy->m_ms) {
		ERROR("ERROR: MS chip is not initialized");
		PRINT_CLI("MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		tempsens = ms5611_GetTempCoffTemp(gy->m_ms);
		INFO("Temperature Coefficient of Temperature Sensitivity (TEMPSENS) = %d",tempsens);
		PRINT_CLI("Temperature Coefficient of Temperature Sensitivity (TEMPSENS) = %d",tempsens);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_pOsr (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char osrStr [100];
	Ms5611_Osr osr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		osr = ms5611_GetPressureOsr(gy->m_ms);
		retVal = ms5611_ConvertOsr2String(osr, &osrStr);
		if (retVal) {
			ERROR("Failed to convert %d into an OSR Value", osr);
			PRINT_CLI("ERROR: Failed to convert %d into an OSR Value", osr);
			goto END;
		}

		INFO("Pressure OSR(OVer Sampling Ratio) = %d",osr);
		PRINT_CLI("Pressure OSR(OVer Sampling Ratio) = %d",osr);
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_get_tempOsr (void)  {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	char osrStr [100];
	msHandle handle = gy->m_ms;
	Ms5611_Osr osr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		osr = ms5611_GetTempOsr(gy->m_ms);
		retVal = ms5611_ConvertOsr2String(osr, &osrStr);
		if (retVal) {
			ERROR("Failed to convert %d into an OSR Value", osr);
			PRINT_CLI("ERROR: Failed to convert %d into an OSR Value", osr);
			goto END;
		}

		INFO("Temperature OSR(OVer Sampling Ratio) = %d",osr);
		PRINT_CLI("Temperature OSR(OVer Sampling Ratio) = %d",osr);
	}

END:
	EXIT();
	return retVal;
}


static retcode handle_set_i2c_addr (char *addrStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Ms5611_I2cAddr addr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		addr = ms5611_ParseI2cAddr(addrStr);
		if (addr == I2C_ADDR_CSB_INVALID) {
			ERROR("Invalid Address %s", addrStr);
			PRINT_CLI("ERROR: Invalid Address %s", addrStr);
			retVal = -1;
			goto END;
		}

		retVal = ms5611_SetI2cAddr(gy->m_ms, addr);
		if (retVal) {
			ERROR("Failed to set I2C Address");
			PRINT_CLI("ERROR: Failed to set I2C Address");
			goto END;
		} else {
			INFO("Setting I2C Address for the MS-5611 Chip");
			PRINT_CLI("Setting I2C Address for the MS-5611 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_sens (char *str) {
	ENTER();

	retcode retVal = -1;

	ERROR("Can not Set SENS ... This is a read only parameter");
	PRINT_CLI("ERROR: Can not Set SENS ... This is a read only parameter");

END:
	EXIT();
	return retVal;
}

static retcode handle_set_off (char *str) {
	ENTER();

	retcode retVal = -1;

	ERROR("Can not Set OFF ... This is a read only parameter");
	PRINT_CLI("ERROR: Can not Set OFF ... This is a read only parameter");

END:
	EXIT();
	return retVal;
}

static retcode handle_set_tcs (char *) {
	ENTER();

	retcode retVal = -1;

	ERROR("Can not Set TCS ... This is a read only parameter");
	PRINT_CLI("ERROR: Can not Set TCS ... This is a read only parameter");

END:
	EXIT();
	return retVal;
}

static retcode handle_set_tco (char *) {
	ENTER();

	retcode retVal = -1;

	ERROR("Can not Set TCO ... This is a read only parameter");
	PRINT_CLI("ERROR: Can not Set TCO ... This is a read only parameter");

END:
	EXIT();
	return retVal;
}

static retcode handle_set_tref (char *) {
	ENTER();

	retcode retVal = -1;

	ERROR("Can not Set TREF ... This is a read only parameter");
	PRINT_CLI("ERROR: Can not Set TREF ... This is a read only parameter");

END:
	EXIT();
	return retVal;
}

static retcode handle_set_tempsens (char *) {
	ENTER();

	retcode retVal = -1;

	ERROR("Can not Set TEMPSENS ... This is a read only parameter");
	PRINT_CLI("ERROR: Can not Set TEMPSENS ... This is a read only parameter");

END:
	EXIT();
	return retVal;
}

static retcode handle_set_pOsr (char *osrStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Ms5611_Osr osr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		osr = ms5611_ParseOsr(osrStr);
		if (osr == OSR_INVALID) {
			ERROR("Invalid OSR %s", osrStr);
			PRINT_CLI("ERROR: Invalid OSR %s", osrStr);
			retVal = -1;
			goto END;
		}

		retVal = ms5611_SetPressureOsr(gy->m_ms, osr);
		if (retVal) {
			ERROR("Failed to set the Pressure OSR ");
			PRINT_CLI("ERROR: Failed to set the Pressure OSR ");
			goto END;
		} else {
			INFO("Setting Pressure Over Sampling Ratio (OSR) for the MS-5611 Chip");
			PRINT_CLI("Setting Pressure Over Sampling Ratio (OSR) for the MS-5611 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_set_tempOsr (char *osrStr) {
	ENTER();

	retcode retVal = 0;
	Gy86 *gy = (Gy86 *)g_gyHandle;
	Ms5611_Osr osr;

	if (!gy->m_ms) {
		ERROR("MS chip is not initialized");
		PRINT_CLI("ERROR: MS chip is not initialized");
		retVal = -1;
		goto END;
	} else {
		osr = ms5611_ParseOsr(osrStr);
		if (osr == OSR_INVALID) {
			ERROR("Invalid OSR %s", osrStr);
			PRINT_CLI("ERROR: Invalid OSR %s", osrStr);
			retVal = -1;
			goto END;
		}

		retVal = ms5611_SetTempOsr(gy->m_ms, osr);
		if (retVal) {
			ERROR("Failed to set the Temperature OSR ");
			PRINT_CLI("ERROR: Failed to set the Temperature OSR ");
			goto END;
		} else {
			INFO("Setting Temperature Over Sampling Ratio (OSR) for the MS-5611 Chip");
			PRINT_CLI("Setting Temperature Over Sampling Ratio (OSR) for the MS-5611 Chip");
		}
	}

END:
	EXIT();
	return retVal;
}

