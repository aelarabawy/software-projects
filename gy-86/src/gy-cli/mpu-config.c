/*
 * mpu-config.c
 *
 *  Created on: Jan 4, 2015
 *      Author: aelarabawy
 */

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
					   "aux-i2c-mode",
					   "aux-i2c-clk",
					   "aux-i2c-reduced-sampling-factor",
                       "LAST_CONFIG"};


/**
 * Get functions prototypes
 */
static retcode handle_get_i2c_addr (void);
static retcode handle_get_clk_src (void);
static retcode handle_get_smpl_rate (void);
static retcode handle_get_lpf (void);
static retcode handle_get_acc_fsr (void);
static retcode handle_get_gyro_fsr (void);
static retcode handle_get_fifo_enable (void);
static retcode handle_get_mot_det_thr (void);
static retcode handle_get_acc_enable (void);
static retcode handle_get_gyro_enable (void);
static retcode handle_get_temp_enable (void);
static retcode handle_get_int_lvl (void);
static retcode handle_get_aux_i2c_mode (void);
static retcode handle_get_aux_i2c_clk (void);
static retcode handle_get_aux_i2c_reduced_smpl (void);

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
									  handle_get_aux_i2c_mode,
									  handle_get_aux_i2c_clk,
									  handle_get_aux_i2c_reduced_smpl};

/**
 * Set functions prototypes
 */
static retcode handle_set_i2c_addr (void);
static retcode handle_set_clk_src (void);
static retcode handle_set_smpl_rate (void);
static retcode handle_set_lpf (void);
static retcode handle_set_acc_fsr (void);
static retcode handle_set_gyro_fsr (void);
static retcode handle_set_fifo_enable (void);
static retcode handle_set_mot_det_thr (void);
static retcode handle_set_acc_enable (void);
static retcode handle_set_gyro_enable (void);
static retcode handle_set_temp_enable (void);
static retcode handle_set_int_lvl (void);
static retcode handle_set_aux_i2c_mode (void);
static retcode handle_set_aux_i2c_clk (void);
static retcode handle_set_aux_i2c_reduced_smpl (void);

void * (*mpuSetHandlers[])(char *) = { handle_set_i2c_addr,
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
									   handle_set_aux_i2c_mode,
									   handle_set_aux_i2c_clk,
									   handle_set_aux_i2c_reduced_smpl};


static char * handle_get_i2c_addr (char *str) {
	ENTER();
	char *result;

END:
	EXIT();
	return result;
}

static retcode handle_get_clk_src (char *);
static retcode handle_get_smpl_rate (char *);
static retcode handle_get_lpf (char *);
static retcode handle_get_acc_fsr (char *);
static retcode handle_get_gyro_fsr (char *);
static retcode handle_get_fifo_enable (char *);
static retcode handle_get_mot_det_thr (char *);



void handle_mpu_get (void) {
	ENTER();

END:
	EXIT();
	return;
}

void handle_mpu_set (void);


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


static retcode handle_set_acc_enable (char *);
static retcode handle_set_gyro_enable (char *);
static retcode handle_set_temp_enable (char *);
static retcode handle_set_int_lvl (char *);
static retcode handle_set_aux_i2c_mode (char *);
static retcode handle_set_aux_i2c_clk (char *);
static retcode handle_set_aux_i2c_reduced_smpl (char *);
