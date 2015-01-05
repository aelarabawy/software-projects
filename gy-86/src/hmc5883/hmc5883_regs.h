/*
 * hmc5883_regs.h
 *
 *  Created on: Dec 24, 2014
 *      Author: aelarabawy
 */

#ifndef HMC5883_REGS_H_
#define HMC5883_REGS_H_

/**
 * This file Contains the names and addresses of the registers in the HMC-5883 chip
 * It also includes defines for the contents of these registers
 * All the numbers in this file conform to the chip Specs
 */

//Register Names & numbers
#define HMC5883_REG_CONFIG_A	   (0x00)   //Read/Write
#define HMC5883_REG_CONFIG_B	   (0x01)   //Read/Write
#define HMC5883_REG_MODE    	   (0x02)   //Read/Write
#define HMC5883_REG_DATA_OUT_X_H   (0x03)   //Read Only
#define HMC5883_REG_DATA_OUT_X_L   (0x04)   //Read Only
#define HMC5883_REG_DATA_OUT_Z_H   (0x05)   //Read Only
#define HMC5883_REG_DATA_OUT_Z_L   (0x06)   //Read Only
#define HMC5883_REG_DATA_OUT_Y_H   (0x07)   //Read Only
#define HMC5883_REG_DATA_OUT_Y_L   (0x08)   //Read Only
#define HMC5883_REG_STATUS         (0x09)   //Read Only
#define HMC5883_REG_IDENT_A        (0x0A)   //Read Only
#define HMC5883_REG_IDENT_B        (0x0B)   //Read Only
#define HMC5883_REG_IDENT_C        (0x0C)   //Read Only


//Default Values
#define HMC5883_REG_CONFIG_A_DEFAULT  (0x10)
#define HMC5883_REG_CONFIG_B_DEFAULT  (0x20)
#define HMC5883_REG_MODE_DEFAULT      (0x01)

//I2C Addresses
#define HMC5883_I2C_ADDRESS_READ       (0x3D)
#define HMC5883_I2C_ADDRESS_WRITE      (0x3C)


/**
 * Mode Register
 */

//Operating Mode
#define HMC5883_OPER_MODE_MASK         (0x03)

#define HMC5883_OPER_MODE_CONTINUOUS   (0x00)
#define HMC5883_OPER_MODE_SINGLE       (0x01)
#define HMC5883_OPER_MODE_IDLE         (0x02)
#define HMC5883_OPER_MODE_IDLE_2       (0x03)

//I2C Bus Speed
#define HMC5883_BUS_SPEED_MASK         (0x80)

#define HMC5883_BUS_SPEED_STD          (0x00)
#define HMC5883_BUS_SPEED_HIGH         (0x80)

/**
 * Configuration Register A
 */
//Data Output Rate
#define HMC5883_DATA_OUT_RATE_MASK     (0x1C)

#define HMC5883_DATA_OUT_RATE_0_75     (0x00)
#define HMC5883_DATA_OUT_RATE_1_5      (0x04)
#define HMC5883_DATA_OUT_RATE_3        (0x08)
#define HMC5883_DATA_OUT_RATE_7_5      (0x0C)
#define HMC5883_DATA_OUT_RATE_15       (0x10)   //Default
#define HMC5883_DATA_OUT_RATE_30       (0x14)
#define HMC5883_DATA_OUT_RATE_75       (0x18)
#define HMC5883_DATA_OUT_RATE_RESERVED (0x1C)

//Over-Sampling (Number of samples per output)
#define HMC5883_OVER_SAMPL_MASK        (0x60)

#define HMC5883_OVER_SAMPL_1           (0x00)  //Default
#define HMC5883_OVER_SAMPL_2           (0x20)
#define HMC5883_OVER_SAMPL_4           (0x40)
#define HMC5883_OVER_SAMPL_8           (0x60)

//Measurement Mode
#define HMC5883_MEASURE_MODE_MASK      (0x03)

#define HMC5883_MEASURE_MODE_NORMAL    (0x00)  //Default
#define HMC5883_MEASURE_MODE_POS_BIAS  (0x01)  //Only used in self test
#define HMC5883_MEASURE_MODE_NEG_BIAS  (0x02)  //Only used in self test
#define HMC5883_MEASURE_MODE_RESERVED  (0x03)

/**
 * Configuration Register B
 */

//Measurement Gain
#define HMC5883_MEASURE_GAIN_MASK      (0xE0)

#define HMC5883_MEASURE_GAIN_1370      (0x00)
#define HMC5883_MEASURE_GAIN_1090      (0x20)   //Default
#define HMC5883_MEASURE_GAIN_820       (0x40)
#define HMC5883_MEASURE_GAIN_660       (0x60)
#define HMC5883_MEASURE_GAIN_440       (0x80)
#define HMC5883_MEASURE_GAIN_390       (0xA0)
#define HMC5883_MEASURE_GAIN_330       (0xC0)
#define HMC5883_MEASURE_GAIN_230       (0xE0)

/**
 * Status Register
 */
#define HMC5883_STATUS_RDY             (0x01)
#define HMC5883_STATUS_LOCK            (0x02)

/**
 * Identity Registers
 */
#define HMC5883_IDENTITY_REG_VAL_A     (0x48)
#define HMC5883_IDENTITY_REG_VAL_B     (0x34)
#define HMC5883_IDENTITY_REG_VAL_C     (0x33)

#endif /* HMC5883_REGS_H_ */
