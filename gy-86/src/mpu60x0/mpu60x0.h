/*
 * mpu60x0.h
 *
 *  Created on: Dec 23, 2014
 *      Author: aelarabawy
 */

#ifndef MPU60X0_H_
#define MPU60X0_H_

#include "mpu60x0_common.h"

/**
 * Some Useful includes
 */

/**
 * Max Sampling Rate (since the Gyroscope output rate is 8KHz
 * Then with the lowest divisor, the maximum sampling rate is 8KHz
 */
#define MPU60X0_MAX_SMPL_RATE (8000)
#define MPU60X0_MIN_SMPL_RATE (15)


/**
 * FIFO Size
 */
#define MPU60X0_FIFO_SIZE  (1024)


/**
 * Temperature Offset
 * Not used yet in code
 */
#define MPU60X0_TEMP_OFF   (-521)

/**
 * Bank Size
 * Not used yet in code
 */
#define MPU60X0_BANK_SIZE  (256)


/**
 * Temperature Sensitivity
 */
#define MPU60X0_TEMP_SENS  (340.0f)

/**
 * This define is to help select the proper LPF for the desired sampling rate
 * Nyquest states that sampling freq must be >= 2 x maximum freq in the signal
 * Accordingly the value in this define must be at least 2
 */
#define MPU60X0_NYQUEST_RATE     (3)


/**
 * Maximum value for Motion Detection Threshold
 * Since it is a 8 bit value
 */
#define MPU60X0_MAX_MOT_DET_THR  (255)

/**
 * Number of slaves supported by the chip
 * SLV-0 to SLV-4
 */
#define MPU60X0_SLV_COUNT       (5)

/**
 * Size of array to hold the external sensor data
 */
#define MPU60X0_MAX_EXT_SENSOR_REG_COUNT 100

/**
 * Self Test Tolerance
 */
#define MPU60X0_SELF_TEST_TOL (0.14)


/**
 * Sensor Reg offset
 */
#define MPU60X0_REG_ACC_X_OFFSET  (0)
#define MPU60X0_REG_ACC_Y_OFFSET  (2)
#define MPU60X0_REG_ACC_Z_OFFSET  (4)
#define MPU60X0_REG_TEMP_OFFSET   (6)
#define MPU60X0_REG_GYRO_X_OFFSET (8)
#define MPU60X0_REG_GYRO_Y_OFFSET (10)
#define MPU60X0_REG_GYRO_Z_OFFSET (12)


/**
 * The operating state of the chip
 */
typedef enum {
	MPU60X0_STATE_INIT = 0,
	MPU60X0_STATE_STARTING,
	MPU60X0_STATE_STOPPING,
	MPU60X0_STATE_STARTED
} Mpu60x0State;

typedef enum {
	MPU60X0_OPER_MODE_SLEEP = 0,
	MPU60X0_OPER_MODE_WAKE,
	MPU60X0_OPER_MODE_CYCLE
} Mpu60x0OperMode;

/**
 * Information for each Slave
 * Some of these info come from configuration, others are internal variables
 */
typedef struct {
	bool m_slvIsEnabled;

	Mpu60x0_AccessOper  m_slvOper;
	uint8 m_slvI2cAddr;
	uint8 m_slvStartRegNum;
	uint8 m_slvRegCount;


	bool m_slvIsSwap;
	bool m_slvIsEvengroup;
	bool m_slvIsRegNumDisable;

	bool m_slvUseReducedSampling;
	bool m_slvIntEnable; //Only applicable for Slv-4

	uint8 m_slvExtSensorRegOffset;  //Not applicable for Slv-4
	uint8 m_padding[1];  //For Word Alignment purpose
} Mpu60x0SlvInfo;


typedef struct {
	//Basic Configurations
	uint8 m_activeSensors;
	Mpu60x0_ClkSrc  m_clkSrc;
	uint8 m_samplingDiv;
	Mpu60x0_Lpf m_dlpf;
	Mpu60x0_AccFullScale m_accScale;
	Mpu60x0_GyroFullScale m_gyroScale;

	//Interrupt Signal
	Mpu60x0_IntLvl m_intLvl;
    bool m_intIsOpenDrain;
    bool m_intIsLatchOn;
    bool m_intClearOnAnyRead;

	//Fifo
	bool m_fifoEnable;

	//Motion Detection
	bool m_motDetEnable;
	uint8 m_motDetThr;

	//Power Management
	uint8 m_pwrMgmtAccOnDelay;
	Mpu60x0_LowPwrWakeCtrlFreq m_pwrMgmtLowPwrWakeCtrl;

	//FSync
	Mpu60x0_SensorId m_fsyncSensorId;
	Mpu60x0_IntLvl m_fsyncIntLvl;

	//Self Test
	uint8 m_selfTestSensors;

	//Auxiliary I2C Bus
	Mpu60x0_AuxI2cMode m_auxI2cMode;
	Mpu60x0_AuxI2cClk m_auxI2cClk;
	uint8 m_auxI2cReducedSmplFactor;
	bool m_auxI2cdelayEsShadow;
	uint8 m_padding[2];     //For word alignment
	Mpu60x0SlvInfo m_auxI2cSlv[MPU60X0_SLV_COUNT];
} Mpu60x0Config;

typedef struct {

	//Call Back functions and params
	void *m_intDataRdyCb;
	uint32 m_intDataRdyParam;

	void *m_intFifoOvlFlowCb;
	uint32 m_intFifoOvlFlowParam;

	void *m_intMotDetCb;
	uint32 m_intMotDetParam;

	void *m_intFsyncCb;
	uint32 m_intFsyncParam;

	void *m_intAuxI2cCb;
	uint32 m_intAuxI2cParam;

	//Configuration
	Mpu60x0Config m_configs;
	Mpu60x0Config m_activeConfigs;

	//Internal Data Array: Need 14 bytes for internal sensors + maximum of 25 bytes for SLV-0 to SLV-4
	//Will make it 40 for alignment purposes
	uint8 m_sensorData [40];


	Mpu60x0State m_state;
	Mpu60x0OperMode m_operMode;
	uint8 m_i2cAddr;

	uint8 m_sensorRegExtCount;
	uint8 m_activeSensorCount;

	uint8 m_padding[3];

} Mpu60x0;



#endif /* MPU60X0_H_ */
