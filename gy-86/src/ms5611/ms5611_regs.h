/*
 * ms5611_regs.h
 *
 *  Created on: Dec 29, 2014
 *      Author: aelarabawy
 */

#ifndef MS5611_MS5611_REGS_H_
#define MS5611_MS5611_REGS_H_

/**
 * This file Contains the constants used in MS-5611 chip
 */

//Commands
#define MS5611_COMMAND_RESET                (0x1E)
#define MS5611_COMMAND_CONVERT_D1_256       (0x40)
#define MS5611_COMMAND_CONVERT_D1_512       (0x42)
#define MS5611_COMMAND_CONVERT_D1_1024      (0x44)
#define MS5611_COMMAND_CONVERT_D1_2048      (0x46)
#define MS5611_COMMAND_CONVERT_D1_4096      (0x48)
#define MS5611_COMMAND_CONVERT_D2_256       (0x50)
#define MS5611_COMMAND_CONVERT_D2_512       (0x52)
#define MS5611_COMMAND_CONVERT_D2_1024      (0x54)
#define MS5611_COMMAND_CONVERT_D2_2048      (0x56)
#define MS5611_COMMAND_CONVERT_D2_4096      (0x58)
#define MS5611_COMMAND_ADC_READ             (0x00)
#define MS5611_COMMAND_READ_PROM            (0xA0)

//I2C Addresses
#define MS5611_I2C_ADDRESS_CSB_LOW          (0x77)
#define MS5611_I2C_ADDRESS_CSB_HIGH         (0x76)



#endif /* MS5611_MS5611_REGS_H_ */
