/*
 * IMU.h
 *
 *  Created on: Dec 23, 2014
 *      Author: User
 */

#ifndef IMU_H_
#define IMU_H_

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <fstream>

#define BMA180_I2C_BUFFER 0x75	//was 0x80
#define MAX_BUS 	64
#define ACC_X_MSB 	0x3B
#define ACC_X_LSB 	0x3C
#define ACC_Y_MSB 	0x3D
#define ACC_Y_LSB 	0x3E
#define ACC_Z_MSB 	0x3F
#define ACC_Z_LSB 	0x40
#define TEMP_MSB  	0x41
#define TEMP_LSB  	0x42  //Temperature
#define GYRO_X_MSB	0x43
#define GYRO_X_LSB	0x44
#define GYRO_Y_MSB	0x45
#define GYRO_Y_LSB	0x46
#define GYRO_Z_MSB	0x47
#define GYRO_Z_LSB	0x48

#define	GYRO_RANGE	0x1b	//bits 4,3
#define ACCEL_RANGE	0x1C  	//bits 4,3
#define BANDWIDTH 	0x20  	//bits 7,6,5,4
#define MODE_CONFIG 0x30  	//bits 1,0
#define SLEEP		0x6B

enum IMU_SCALE_ACCEL {
	PLUSMINUS_2G 		= 0,
	PLUSMINUS_4G 		= 1,
	PLUSMINUS_8G 		= 2,
	PLUSMINUS_16G 		= 3
};

enum IMU_SCALE_GYRO {
	PLUSMINUS_250 		= 0,
	PLUSMINUS_500 		= 1,
	PLUSMINUS_1000 		= 2,
	PLUSMINUS_2000 		= 3
};

//this is sample rate register 25 in IMU
enum BMA180_BANDWIDTH {
	BW_10HZ 	= 0,
	BW_20HZ 	= 1,
	BW_40HZ 	= 2,
	BW_75HZ 	= 3,
	BW_150HZ 	= 4,
	BW_300HZ 	= 5,
	BW_600HZ 	= 6,
	BW_12OOHZ 	= 7,
	BW_HIGHPASS = 8,
	BW_BANDPASS = 9
};

enum BMA180_MODECONFIG {
	MODE_LOW_NOISE = 0,
	MODE_LOW_POWER = 3
};

class IMU {

private:
	int I2CBus, I2CAddress;
	char dataBuffer[BMA180_I2C_BUFFER];

	int accelerationX, accelerationY, accelerationZ;
	int gyroX, gyroY, gyroZ;

	double pitch;  //in degrees
	double roll;   //in degrees

	float temperature; //accurate to 0.5C
	IMU_SCALE_ACCEL accel_range;
	IMU_SCALE_GYRO gyro_range;
	BMA180_BANDWIDTH bandwidth;
	BMA180_MODECONFIG modeConfig;

	int  fuseBytes(int msb_addr, int lsb_addr);
	int  writeI2CDeviceByte(char address, char value);
	//char readI2CDeviceByte(char address);
	void calculatePitchAndRoll();
	void calibrateFromFile();

	int gyroXoffset, gyroYoffset, gyroZoffset, accelXoffset, accelYoffset, accelZoffset;

public:
	IMU(int bus, int address);
	int wake();
	void calibrate();
	void displayMode(int iterations);

	int  readFullSensorState();
	// The following do physical reads and writes of the sensors
	int setAccelRange(IMU_SCALE_ACCEL accel_range);
	IMU_SCALE_ACCEL getAccelRange();
	int setGyroRange(IMU_SCALE_GYRO gyro_range);
	IMU_SCALE_GYRO getGyroRange();

	int setBandwidth(BMA180_BANDWIDTH bandwidth);
	BMA180_BANDWIDTH getBandwidth();
	int setModeConfig(BMA180_MODECONFIG mode);
	BMA180_MODECONFIG getModeConfig();
	float getTemperature();

	int getAccelerationX() {return accelerationX - accelXoffset;}
	int getAccelerationY() {return accelerationY - accelYoffset;}
	int getAccelerationZ() {return accelerationZ - accelZoffset;}

	int getGyroX() {return gyroX - gyroXoffset;}
	int getGyroY() {return gyroY - gyroYoffset;}
	int getGyroZ() {return gyroZ - gyroZoffset;}

	float getPitch() { return pitch; }  // in degrees
	float getRoll() { return roll; }  // in degrees

	~IMU();		//virtual
};

#endif /* IMU_H_ */
