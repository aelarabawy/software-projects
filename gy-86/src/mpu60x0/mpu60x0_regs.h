/*
 * mpu60x0_regs.h
 *
 *  Created on: Dec 24, 2014
 *      Author: aelarabawy
 */

#ifndef MPU60X0_REGS_H_
#define MPU60X0_REGS_H_

/**
 * This file Contains the names and addresses of the registers in the MPU-60x0 chipset
 * This includes both the MPU-6000 and MPU-6050 chips
 * It also includes defines for the contents of these registers
 * All the numbers in this file conform to the chip information
 */

//Register Names & numbers
#define MPU60X0_REG_XA_OFFS_USRH	   (0x06)  //ACCEL X-axis Offset Cancellation Register High (from separate spec)
#define MPU60X0_REG_XA_OFFS_USRL	   (0x07)   //ACCEL X-axis Offset Cancellation Register Low  (from separate spec)
#define MPU60X0_REG_YA_OFFS_USRH	   (0x08)   //ACCEL Y-axis Offset Cancellation Register High (from separate spec)
#define MPU60X0_REG_YA_OFFS_USRL	   (0x09)   //ACCEL Y-axis Offset Cancellation Register Low  (from separate spec)
#define MPU60X0_REG_ZA_OFFS_USRH	   (0x0A)   //ACCEL Z-axis Offset Cancellation Register High (from separate spec)
#define MPU60X0_REG_ZA_OFFS_USRL	   (0x0B)   //ACCEL Z-axis Offset Cancellation Register Low  (from separate spec)
#define MPU60X0_REG_PROD_ID            (0x0C)   //??From Code
#define MPU60X0_REG_SELF_TEST_X        (0x0D)
#define MPU60X0_REG_SELF_TEST_Y        (0x0E)
#define MPU60X0_REG_SELF_TEST_Z        (0x0F)
#define MPU60X0_REG_SELF_TEST_A        (0x10)


#define MPU60X0_REG_XG_OFFS_USRH	   (0x13)   //Gyro X-axis Offset Cancellation Register High  (from separate spec)
#define MPU60X0_REG_XG_OFFS_USRL	   (0x14)   //Gyro X-axis Offset Cancellation Register Low   (from separate spec)
#define MPU60X0_REG_YG_OFFS_USRH	   (0x15)   //Gyro Y-axis Offset Cancellation Register High  (from separate spec)
#define MPU60X0_REG_YG_OFFS_USRL	   (0x16)  //Gyro Y-axis Offset Cancellation Register Low   (from separate spec)
#define MPU60X0_REG_ZG_OFFS_USRH	   (0x17)   //Gyro Z-axis Offset Cancellation Register High  (from separate spec)
#define MPU60X0_REG_ZG_OFFS_USRL	   (0x18)   //Gyro Z-axis Offset Cancellation Register Low   (from separate spec)
#define MPU60X0_REG_SMPLRT_DIV         (0x19)
#define MPU60X0_REG_CONFIG             (0x1A)
#define MPU60X0_REG_GYRO_CONFIG        (0x1B)
#define MPU60X0_REG_ACCEL_CONFIG       (0x1C)
#define MPU60X0_REG_ACCEL_CONFIG2      (0x1D)   //Only Applicable to MPU6500, so should not be used
#define MPU60X0_REG_LP_ACCEL_ODR       (0x1E)   //Only Applicable to MPU6500, so should not be used
#define MPU60X0_REG_MOT_THR            (0x1F)
#define MPU60X0_REG_MOT_DUR			   (0x20)   //??From Code

#define MPU60X0_REG_FIFO_EN            (0x23)
#define MPU60X0_REG_I2C_MST_CTRL       (0x24)
#define MPU60X0_REG_I2C_SLV0_ADDR      (0x25)
#define MPU60X0_REG_I2C_SLV0_REG       (0x26)
#define MPU60X0_REG_I2C_SLV0_CTRL      (0x27)
#define MPU60X0_REG_I2C_SLV1_ADDR      (0x28)
#define MPU60X0_REG_I2C_SLV1_REG       (0x29)
#define MPU60X0_REG_I2C_SLV1_CTRL      (0x2A)
#define MPU60X0_REG_I2C_SLV2_ADDR      (0x2B)
#define MPU60X0_REG_I2C_SLV2_REG       (0x2C)
#define MPU60X0_REG_I2C_SLV2_CTRL      (0x2D)
#define MPU60X0_REG_I2C_SLV3_ADDR      (0x2E)
#define MPU60X0_REG_I2C_SLV3_REG       (0x2F)
#define MPU60X0_REG_I2C_SLV3_CTRL      (0x30)
#define MPU60X0_REG_I2C_SLV4_ADDR      (0x31)
#define MPU60X0_REG_I2C_SLV4_REG       (0x32)
#define MPU60X0_REG_I2C_SLV4_DO        (0x33)
#define MPU60X0_REG_I2C_SLV4_CTRL      (0x34)
#define MPU60X0_REG_I2C_SLV4_DI        (0x35)
#define MPU60X0_REG_I2C_MST_STATUS     (0x36)
#define MPU60X0_REG_INT_PIN_CFG        (0x37)
#define MPU60X0_REG_INT_ENABLE         (0x38)
#define MPU60X0_REG_DMP_INT_STATUS     (0x39)    //??From Code
#define MPU60X0_REG_INT_STATUS         (0x3A)
#define MPU60X0_REG_ACCEL_XOUT_H       (0x3B)
#define MPU60X0_REG_ACCEL_XOUT_L       (0x3C)
#define MPU60X0_REG_ACCEL_YOUT_H       (0x3D)
#define MPU60X0_REG_ACCEL_YOUT_L       (0x3E)
#define MPU60X0_REG_ACCEL_ZOUT_H       (0x3F)
#define MPU60X0_REG_ACCEL_ZOUT_L       (0x40)
#define MPU60X0_REG_TEMP_OUT_H         (0x41)
#define MPU60X0_REG_TEMP_OUT_L         (0x42)
#define MPU60X0_REG_GYRO_XOUT_H        (0x43)
#define MPU60X0_REG_GYRO_XOUT_L        (0x44)
#define MPU60X0_REG_GYRO_YOUT_H        (0x45)
#define MPU60X0_REG_GYRO_YOUT_L        (0x46)
#define MPU60X0_REG_GYRO_ZOUT_H        (0x47)
#define MPU60X0_REG_GYRO_ZOUT_L        (0x48)
#define MPU60X0_REG_EXT_SENS_DATA_00   (0x49)
#define MPU60X0_REG_EXT_SENS_DATA_01   (0x4A)
#define MPU60X0_REG_EXT_SENS_DATA_02   (0x4B)
#define MPU60X0_REG_EXT_SENS_DATA_03   (0x4C)
#define MPU60X0_REG_EXT_SENS_DATA_04   (0x4D)
#define MPU60X0_REG_EXT_SENS_DATA_05   (0x4E)
#define MPU60X0_REG_EXT_SENS_DATA_06   (0x4F)
#define MPU60X0_REG_EXT_SENS_DATA_07   (0x50)
#define MPU60X0_REG_EXT_SENS_DATA_08   (0x51)
#define MPU60X0_REG_EXT_SENS_DATA_09   (0x52)
#define MPU60X0_REG_EXT_SENS_DATA_10   (0x53)
#define MPU60X0_REG_EXT_SENS_DATA_11   (0x54)
#define MPU60X0_REG_EXT_SENS_DATA_12   (0x55)
#define MPU60X0_REG_EXT_SENS_DATA_13   (0x56)
#define MPU60X0_REG_EXT_SENS_DATA_14   (0x57)
#define MPU60X0_REG_EXT_SENS_DATA_15   (0x58)
#define MPU60X0_REG_EXT_SENS_DATA_16   (0x59)
#define MPU60X0_REG_EXT_SENS_DATA_17   (0x5A)
#define MPU60X0_REG_EXT_SENS_DATA_18   (0x5B)
#define MPU60X0_REG_EXT_SENS_DATA_19   (0x5C)
#define MPU60X0_REG_EXT_SENS_DATA_20   (0x5D)
#define MPU60X0_REG_EXT_SENS_DATA_21   (0x5E)
#define MPU60X0_REG_EXT_SENS_DATA_22   (0x5F)
#define MPU60X0_REG_EXT_SENS_DATA_23   (0x60)

#define MPU60X0_REG_I2C_SLV0_DO        (0x63)
#define MPU60X0_REG_I2C_SLV1_DO        (0x64)
#define MPU60X0_REG_I2C_SLV2_DO        (0x65)
#define MPU60X0_REG_I2C_SLV3_DO        (0x66)
#define MPU60X0_REG_I2C_MST_DELAY_CTRL (0x67)
#define MPU60X0_REG_SIGNAL_PATH_RESET  (0x68)
#define MPU60X0_REG_MOT_DET_CTRL       (0x69)
#define MPU60X0_REG_ACCEL_INTEL        (0x69) //Only Applicable to MPU6500, so should not be used
#define MPU60X0_REG_USER_CTRL		   (0x6A)
#define MPU60X0_REG_PWR_MGMT_1         (0x6B)
#define MPU60X0_REG_PWR_MGMT_2         (0x6C)
#define MPU60X0_REG_BANK_SEL           (0x6D)   //??From Code
#define MPU60X0_REG_MEM_START_ADDR     (0x6E)   //??From Code
#define MPU60X0_REG_MEM_R_W			   (0x6F)   //??From Code

#define MPU60X0_REG_FIFO_COUNT_H       (0x72)
#define MPU60X0_REG_FIFO_COUNT_L       (0x73)
#define MPU60X0_REG_FIFO_R_W           (0x74)
#define MPU60X0_REG_WHO_AM_I           (0x75)


//I2C Addresses
#define MPU60X0_I2C_ADDRESS_LOW        (0x68)
#define MPU60X0_I2C_ADDRESS_HIGH       (0x69)


/**
 * Format of Registers
 */
//MPU60X0_REG_XA_OFFS_USRH   TODO
//MPU60X0_REG_XA_OFFS_USRL   TODO
//MPU60X0_REG_YA_OFFS_USRH   TODO
//MPU60X0_REG_YA_OFFS_USRL   TODO
//MPU60X0_REG_ZA_OFFS_USRH   TODO
//MPU60X0_REG_ZA_OFFS_USRL   TODO
//MPU60X0_REG_PROD_ID        TODO

//MPU60X0_REG_SELF_TEST_X
#define MPU60X0_FIELD_XA_TEST_H        (0xE0)
#define MPU60X0_FIELD_XG_TEST          (0x1F)

//MPU60X0_REG_SELF_TEST_Y
#define MPU60X0_FIELD_YA_TEST_H        (0xE0)
#define MPU60X0_FIELD_YG_TEST          (0x1F)

//MPU60X0_REG_SELF_TEST_Z
#define MPU60X0_FIELD_ZA_TEST_H        (0xE0)
#define MPU60X0_FIELD_ZG_TEST          (0x1F)

//MPU60X0_REG_SELF_TEST_A
#define MPU60X0_FIELD_ZA_TEST_L        (0x03)
#define MPU60X0_FIELD_YA_TEST_L        (0x0C)
#define MPU60X0_FIELD_XA_TEST_L        (0x30)

//MPU60X0_REG_XG_OFFS_USRH   TODO
//MPU60X0_REG_XG_OFFS_USRL   TODO
//MPU60X0_REG_YG_OFFS_USRH   TODO
//MPU60X0_REG_YG_OFFS_USRL   TODO
//MPU60X0_REG_ZG_OFFS_USRH   TODO
//MPU60X0_REG_ZG_OFFS_USRL   TODO

//MPU60X0_REG_SMPLRT_DIV   (Nothing to detail)
//MPU60X0_REG_CONFIG
#define MPU60X0_FIELD_DLPF_CFG         (0x07)
#define MPU60X0_FIELD_EXT_SYNC_SET     (0x38)

//MPU60X0_REG_GYRO_CONFIG
#define MPU60X0_FIELD_FS_SEL           (0x18)
#define MPU60X0_FIELD_ZG_ST            (0x20)
#define MPU60X0_FIELD_YG_ST            (0x40)
#define MPU60X0_FIELD_XG_ST            (0x80)

//MPU60X0_REG_ACCEL_CONFIG
#define MPU60X0_FIELD_AFS_SEL          (0x18)
#define MPU60X0_FIELD_ZA_ST            (0x20)
#define MPU60X0_FIELD_YA_ST            (0x40)
#define MPU60X0_FIELD_XA_ST            (0x80)

//MPU60X0_REG_ACCEL_CONFIG2    TODO
//MPU60X0_REG_LP_ACCEL_ODR     TODO

//MPU60X0_REG_MOT_DUR          TODO

//MPU60X0_REG_FIFO_EN
#define MPU60X0_FIELD_SLV0_FIFO_EN     (0x01)
#define MPU60X0_FIELD_SLV1_FIFO_EN     (0x02)
#define MPU60X0_FIELD_SLV2_FIFO_EN     (0x04)
#define MPU60X0_FIELD_ACCEL_FIFO_EN    (0x08)
#define MPU60X0_FIELD_ZG_FIFO_EN       (0x10)
#define MPU60X0_FIELD_YG_FIFO_EN       (0x20)
#define MPU60X0_FIELD_XG_FIFO_EN       (0x40)
#define MPU60X0_FIELD_TEMP_FIFO_EN     (0x80)

//MPU60X0_REG_I2C_MST_CTRL
#define MPU60X0_FIELD_I2C_MST_CLK      (0x0F)
#define MPU60X0_FIELD_I2C_MST_P_NSR    (0x10)
#define MPU60X0_FIELD_SLV3_FIFO_EN     (0x20)
#define MPU60X0_FIELD_WAIT_FOR_ES      (0x40)
#define MPU60X0_FIELD_MULT_MST_EN      (0x80)

//MPU60X0_REG_I2C_SLV0_ADDR
//MPU60X0_REG_I2C_SLV1_ADDR
//MPU60X0_REG_I2C_SLV2_ADDR
//MPU60X0_REG_I2C_SLV3_ADDR
#define MPU60X0_FIELD_I2C_SLVn_ADDR    (0x7F)
#define MPU60X0_FIELD_I2C_SLVn_RW      (0x80)

//MPU60X0_REG_I2C_SLV0_CTRL
//MPU60X0_REG_I2C_SLV1_CTRL
//MPU60X0_REG_I2C_SLV2_CTRL
//MPU60X0_REG_I2C_SLV3_CTRL
#define MPU60X0_FIELD_I2C_SLVn_LEN     (0x0F)
#define MPU60X0_FIELD_I2C_SLVn_GRP     (0x10)
#define MPU60X0_FIELD_I2C_SLVn_REG_DIS (0x20)
#define MPU60X0_FIELD_I2C_SLVn_BYTE_SW (0x40)
#define MPU60X0_FIELD_I2C_SLVn_EN      (0x80)

//MPU60X0_REG_I2C_SLV4_ADDR
#define MPU60X0_FIELD_I2C_SLV4_ADDR    (0x7F)
#define MPU60X0_FIELD_I2C_SLV4_RW      (0x80)

//MPU60X0_REG_I2C_SLV4_REG     (Nothing to Detail)
//MPU60X0_REG_I2C_SLV4_DO      (Nothing to Detail)

//MPU60X0_REG_I2C_SLV4_CTRL
#define MPU60X0_FIELD_I2C_MST_DLY      (0x1F)
#define MPU60X0_FIELD_I2C_SLV4_REG_DIS (0x20)
#define MPU60X0_FIELD_I2C_SLV4_INT_EN  (0x40)
#define MPU60X0_FIELD_I2C_SLV4_EN      (0x80)

//MPU60X0_REG_I2C_SLV4_DI      (Nothing to Detail)

//MPU60X0_REG_I2C_MST_STATUS
#define MPU60X0_FIELD_I2C_SLV0_NACK    (0x01)
#define MPU60X0_FIELD_I2C_SLV1_NACK    (0x02)
#define MPU60X0_FIELD_I2C_SLV2_NACK    (0x04)
#define MPU60X0_FIELD_I2C_SLV3_NACK    (0x08)
#define MPU60X0_FIELD_I2C_SLV4_NACK    (0x10)
#define MPU60X0_FIELD_I2C_LOST_ARB     (0x20)
#define MPU60X0_FIELD_I2C_SLV4_DONE    (0x40)
#define MPU60X0_FIELD_PASS_THROUGH     (0x80)

//MPU60X0_REG_INT_PIN_CFG
#define MPU60X0_FIELD_I2C_BYPASS_EN    (0x02)
#define MPU60X0_FIELD_FSYNC_INT_EN     (0x04)
#define MPU60X0_FIELD_FSYNC_INT_LEVEL  (0x08)
#define MPU60X0_FIELD_INT_RD_CLEAR     (0x10)
#define MPU60X0_FIELD_LATCH_INT_EN     (0x20)
#define MPU60X0_FIELD_INT_OPEN         (0x40)
#define MPU60X0_FIELD_INT_LEVEL        (0x80)

//MPU60X0_REG_INT_ENABLE
#define MPU60X0_FIELD_DATA_RDY_EN      (0x01)
#define MPU60X0_FIELD_I2C_MST_INT_EN   (0x08)
#define MPU60X0_FIELD_FIFO_OFLOW_EN    (0x10)
#define MPU60X0_FIELD_MOT_EN           (0x40)

//MPU60X0_REG_DMP_INT_STATUS    TODO

//MPU60X0_REG_INT_STATUS
#define MPU60X0_FIELD_DATA_RDY_INT     (0x01)
#define MPU60X0_FIELD_I2C_MST_INT      (0x08)
#define MPU60X0_FIELD_FIFO_OFLOW_INT   (0x10)
#define MPU60X0_FIELD_MOT_INT          (0x40)

//MPU60X0_REG_I2C_MST_DELAY_CTRL
#define MPU60X0_FIELD_I2C_SLV0_DLY_EN  (0x01)
#define MPU60X0_FIELD_I2C_SLV1_DLY_EN  (0x02)
#define MPU60X0_FIELD_I2C_SLV2_DLY_EN  (0x04)
#define MPU60X0_FIELD_I2C_SLV3_DLY_EN  (0x08)
#define MPU60X0_FIELD_I2C_SLV4_DLY_EN  (0x10)
#define MPU60X0_FIELD_DELAY_ES_SHADOW  (0x80)

//MPU60X0_REG_SIGNAL_PATH_RESET
#define MPU60X0_FIELD_TEMP_RESET       (0x01)
#define MPU60X0_FIELD_ACCEL_RESET      (0x02)
#define MPU60X0_FIELD_GYRO_RESET       (0x04)

//MPU60X0_REG_MOT_DET_CTRL
#define MPU60X0_FIELD_ACCEL_ON_DELAY   (0x30)

//MPU60X0_REG_ACCEL_INTEL   TODO

//MPU60X0_REG_USER_CTRL
#define MPU60X0_FIELD_SIG_COND_RESET   (0x01)
#define MPU60X0_FIELD_I2C_MST_RESET    (0x02)
#define MPU60X0_FIELD_FIFO_RESET       (0x04)
#define MPU60X0_FIELD_I2C_IF_DIS       (0x10)
#define MPU60X0_FIELD_I2C_MST_EN       (0x20)
#define MPU60X0_FIELD_FIFO_EN          (0x40)

//MPU60X0_REG_PWR_MGMT_1
#define MPU60X0_FIELD_CLKSEL           (0x07)
#define MPU60X0_FIELD_TEMP_DIS         (0x08)
#define MPU60X0_FIELD_CYCLE            (0x20)
#define MPU60X0_FIELD_SLEEP            (0x40)
#define MPU60X0_FIELD_DEVICE_RESET     (0x80)

//MPU60X0_REG_PWR_MGMT_2
#define MPU60X0_FIELD_STBY_ZG          (0x01)
#define MPU60X0_FIELD_STBY_YG          (0x02)
#define MPU60X0_FIELD_STBY_XG          (0x04)
#define MPU60X0_FIELD_STBY_ZA          (0x08)
#define MPU60X0_FIELD_STBY_YA          (0x10)
#define MPU60X0_FIELD_STBY_XA          (0x20)
#define MPU60X0_FIELD_LP_WAKE_CTRL     (0xC0)

//MPU60X0_REG_BANK_SEL             TODO
//MPU60X0_REG_MEM_START_ADDR       TODO
//MPU60X0_REG_MEM_R_W              TODO


//////////////////////////////////////////////////////////////////////////////////

//Default value for WHO_AM_I
#define MPU60X0_REG_WHO_AM_I_VAL       (0x68)

//Clock Source
#define MPU60X0_CLK_SRC_INT_OSC   (0x00)
#define MPU60X0_CLK_SRC_GYRO_X    (0x01)
#define MPU60X0_CLK_SRC_GYRO_Y    (0x02)
#define MPU60X0_CLK_SRC_GYRO_Z    (0x03)
#define MPU60X0_CLK_SRC_EXT_L     (0x04)
#define MPU60X0_CLK_SRC_EXT_H     (0x05)

//LPF
#define MPU60X0_LPF_NONE          (0x00)
#define MPU60X0_LPF_184_188       (0x01)
#define MPU60X0_LPF_94_98         (0x02)
#define MPU60X0_LPF_44_42         (0x03)
#define MPU60X0_LPF_21_20         (0x04)
#define MPU60X0_LPF_10_10         (0x05)
#define MPU60X0_LPF_5_5           (0x06)
#define MPU60X0_LPF_RESERVED      (0x07)

//Gyroscope Full Scale Range
#define MPU60X0_GYRO_SCALE_250    (0x00)
#define MPU60X0_GYRO_SCALE_500    (0x08)
#define MPU60X0_GYRO_SCALE_1000   (0x10)
#define MPU60X0_GYRO_SCALE_2000   (0x18)

//Accelerometer Full Scale Range
#define MPU60X0_ACC_SCALE_2G      (0x00)
#define MPU60X0_ACC_SCALE_4G      (0x08)
#define MPU60X0_ACC_SCALE_8G      (0x10)
#define MPU60X0_ACC_SCALE_16G     (0x18)

//Power Management
#define MPU60X0_LP_WAKE_FREQ_1    (0x00)
#define MPU60X0_LP_WAKE_FREQ_5    (0x40)
#define MPU60X0_LP_WAKE_FREQ_20   (0x80)
#define MPU60X0_LP_WAKE_FREQ_40   (0xC0)

//Auxiliary I2C Clock Speed
#define MPU60X0_CLK_258           (0x08)
#define MPU60X0_CLK_267           (0x07)
#define MPU60X0_CLK_276           (0x06)
#define MPU60X0_CLK_286           (0x05)
#define MPU60X0_CLK_296           (0x04)
#define MPU60X0_CLK_308           (0x03)
#define MPU60X0_CLK_320           (0x02)
#define MPU60X0_CLK_333           (0x01)
#define MPU60X0_CLK_348           (0x00)
#define MPU60X0_CLK_364           (0x0F)
#define MPU60X0_CLK_381           (0x0E)
#define MPU60X0_CLK_400           (0x0D)
#define MPU60X0_CLK_421           (0x0C)
#define MPU60X0_CLK_444           (0x0B)
#define MPU60X0_CLK_471           (0x0A)
#define MPU60X0_CLK_500           (0x09)

//FSYNC EXT
#define MPU60X0_FSYNCH_EXT_NONE    (0x00)
#define MPU60X0_FSYNCH_EXT_TEMP    (0x08)
#define MPU60X0_FSYNCH_EXT_GYRO_X  (0x10)
#define MPU60X0_FSYNCH_EXT_GYRO_Y  (0x18)
#define MPU60X0_FSYNCH_EXT_GYRO_Z  (0x20)
#define MPU60X0_FSYNCH_EXT_ACC_X   (0x28)
#define MPU60X0_FSYNCH_EXT_ACC_Y   (0x30)
#define MPU60X0_FSYNCH_EXT_ACC_Z   (0x38)



#endif /* MPU60X0_REGS_H_ */
