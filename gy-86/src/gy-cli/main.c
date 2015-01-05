/*
 * main.c
 *
 *  Created on: Jan 2, 2015
 *      Author: aelarabawy
 */

#include "logger.h"
#include "gy86_api.h"

/**
 * Internal Function Prototypes
 */
static retcode handle_gy86_init (void);
static retcode handle_gy86_start (void);
static retcode handle_gy86_stop (void);
static retcode handle_gy86_destroy (void);
static retcode handle_gy86_update (void);
static retcode handle_gy86_reset (void);
static retcode handle_gy86_get (void);
static retcode handle_gy86_set (void);

/**
 * Internal Functions for specific chips
 */
retcode handle_mpu_get (void);
retcode handle_hmc_get (void);
retcode handle_ms_get (void);

retcode handle_mpu_set (void);
retcode handle_hmc_set (void);
retcode handle_ms_set (void);
///////////////////////////////////////////////////

/**
 * Globals
 */

gyHandle g_gyHandle = NULL;

char *commands[] = {"gy-init" ,
		            "gy-start",
					"gy-stop",
					"gy-destroy",
					"gy-update",
					"gy-reset",
					"gy-get",
					"gy-set",
                    "LAST_COMMAND"};


retcode (*commandHandlers[])(void)  = {handle_gy86_init,
                                       handle_gy86_start,
			  				           handle_gy86_stop,
								       handle_gy86_destroy,
									   handle_gy86_update,
									   handle_gy86_reset,
							           handle_gy86_get,
							           handle_gy86_set};

//main function
int main (int argc, char* argv[]) {
	ENTER();

	PROGRESS("ENTERING THE MAIN FOR gy86-cli application");

	retcode retVal = 0;
	char command [100]="";
	char* token;

	while (strcmp(command,"exit") != 0) {
		printf("> ");
		fgets(command, sizeof (command), stdin);

		//Remove the Terminating EOL
		strtok(command,"\n");

		//Get the command
		token = strtok(command, " ");

		int i = 0;

		while (strcmp(commands[i], "LAST_COMMAND") != 0) {
			if (strcmp(token, commands[i]) != 0) {
				i++;
			} else {
				retVal = commandHandlers[i]();
				if (retVal) {
					ERROR("Can not Handle the given Command %s", commands[i]);
				}
				break;
			}
		}
		if (strcmp(commands[i], "LAST_COMMAND") == 0) {
			ERROR("Invalid Command %s", token);
		}
	}

	EXIT();
	return 0;
}

static Gy86_ChipType getChipType (char *separator) {
	ENTER();

	char *token;
	Gy86_ChipType chip;

	//Get the chip from Arguments
    token = strtok(NULL, separator);
	if (token != NULL){
		if (strcmp(token, "mpu") == 0) {
			chip = CHIP_TYPE_MPU60X0;
		} else if (strcmp(token, "ms") == 0) {
			chip = CHIP_TYPE_MS5611;
		} else if (strcmp(token, "hmc") == 0) {
			chip = CHIP_TYPE_HMC5883;
		} else if (strcmp(token, "all") == 0) {
			chip = CHIP_TYPE_ALL;
		} else {
			ERROR("Invalid Chip %s", token);
			chip = CHIP_TYPE_INVALID;
		}
	} else {
		WARN("No chip specified, assume all");
		chip = CHIP_TYPE_ALL;
	}

	EXIT();
	return chip;
}

static retcode handle_gy86_init(void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-init Command ");

	//Get the chips to initialize
	Gy86_ChipType chip = getChipType(" ");
	if (chip == CHIP_TYPE_INVALID) {
		ERROR("Invalid Chip Name ... Exiting");
		retVal = -1;
		goto END;
	}

	g_gyHandle = gy86_Init(chip);
	if (!g_gyHandle) {
		ERROR("Failed to Initialize the GY-86 Module");
		retVal = -1;
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_gy86_start(void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-start Command ");

	//Get the chips to start
	Gy86_ChipType chip = getChipType(" ");
	if (chip == CHIP_TYPE_INVALID) {
		ERROR("Invalid Chip Name ... Exiting");
		retVal = -1;
		goto END;
	}

	retcode retVal = gy86_Start(g_gyHandle, chip);
	if (retVal) {
		ERROR("Failed to Start the GY-86 Module chips");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_gy86_stop(void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-stop Command ");

	//Get the chips to stop
	Gy86_ChipType chip = getChipType(" ");
	if (chip == CHIP_TYPE_INVALID) {
		ERROR("Invalid Chip Name ... Exiting");
		retVal = -1;
		goto END;
	}

    retVal = gy86_Stop(g_gyHandle, chip);
	if (retVal) {
		ERROR("Failed to Stop the GY-86 Module chips");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_gy86_destroy(void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-destroy Command ");

    //Destroying the whole module, no need for further arguments
	retVal = gy86_Destroy(g_gyHandle);
	if (retVal) {
		ERROR("Failed to Destroy the GY-86 Module");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_gy86_update(void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-update Command ");

	//Get the chips to update
	Gy86_ChipType chip = getChipType(" ");
	if (chip == CHIP_TYPE_INVALID) {
		ERROR("Invalid Chip Name ... Exiting");
		retVal = -1;
		goto END;
	}

	retcode retVal = gy86_Update(g_gyHandle, chip);
	if (retVal) {
		ERROR("Failed to Update the GY-86 Module chips");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_gy86_reset (void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-destroy Command ");

	//Reset Mode in case of MPU Chip only
	Mpu60x0_ResetMode mode = RESET_MODE_ALL;

	//Get the chips to reset
	Gy86_ChipType chip = getChipType(" ");
	if (chip == CHIP_TYPE_INVALID) {
		ERROR("Invalid Chip Name ... Exiting");
		retVal = -1;
		goto END;
	}

	if (chip == CHIP_TYPE_MPU60X0) {
		//Get the part to reset (or all)
		char *token = strtok(NULL, " ");
		if (token != NULL){
			mode = mpu60x0_parseResetMode(token);

			if (mode == RESET_MODE_INVALID) {
				ERROR("Invalid Reset Mode ...Exiting");
				retVal = -1;
				goto END;
			}
		}
	}

	retVal = gy86_Reset(g_gyHandle, chip, mode);
	if (retVal) {
		ERROR("Failed to Reset the GY-86 Module chips");
		goto END;
	}

END:
	EXIT();
	return retVal;
}

static retcode handle_gy86_get(void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-get Command ");

	//Get the chip to read Configuration from
	Gy86_ChipType chip = getChipType(".");
	if (chip == CHIP_TYPE_INVALID) {
		ERROR("Invalid Chip Name ... Exiting");
		goto END;
	}

	switch (chip) {
	case CHIP_TYPE_MPU60X0:
		retVal = handle_mpu_get();
		if (retVal) {
			ERROR("Failed to Handle MPU Get Command");
			goto END;
		}
		break;

	case CHIP_TYPE_MS5611:
		retVal = handle_ms_get();
		if (retVal) {
			ERROR("Failed to Handle MS Get Command");
			goto END;
		}
		break;

	case CHIP_TYPE_HMC5883:
		retVal = handle_hmc_get();
		if (retVal) {
			ERROR("Failed to Handle HMC Get Command");
			goto END;
		}
		break;

	default:
		ERROR("Invalid Chip");
		retVal = -1;
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}


static retcode handle_gy86_set(void) {
	ENTER();

	retcode retVal = 0;

	PROGRESS("Handling gy86-set Command ");

	//Get the chip to set Configuration into
	Gy86_ChipType chip = getChipType(".");
	if (chip == CHIP_TYPE_INVALID) {
		ERROR("Invalid Chip Name ... Exiting");
		retVal = -1;
		goto END;
	}

	switch (chip) {
	case CHIP_TYPE_MPU60X0:
		retVal = handle_mpu_set();
		if (retVal) {
			ERROR("Failed to Handle MPU Set Command");
			goto END;
		}
		break;

	case CHIP_TYPE_MS5611:
		handle_ms_set();
		if (retVal) {
			ERROR("Failed to Handle MS Set Command");
			goto END;
		}
		break;

	case CHIP_TYPE_HMC5883:
		retVal = handle_hmc_set();
		if (retVal) {
			ERROR("Failed to Handle HMC Set Command");
			goto END;
		}
		break;

	default:
		ERROR("Invalid Chip");
		goto END;
		break;
	}

END:
	EXIT();
	return retVal;
}


