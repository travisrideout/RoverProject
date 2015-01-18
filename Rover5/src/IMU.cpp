/*
 * IMU.cpp
 *
 *  Created on: Dec 23, 2014
 *      Author: User
 */

#include "IMU.h"

IMU::IMU(int bus, int address) {
	I2CBus = bus;
	I2CAddress = address;
	wake();
	readFullSensorState();
}

int IMU::wake(){
	if(this->writeI2CDeviceByte(SLEEP, 0x00)!=0){
		std::cout << "Failure to wake IMU" << std::endl;
		return 1;
	}
	return 0;
}

void IMU::calculatePitchAndRoll() {

	double accelerationXSquared = this->accelerationX * this->accelerationX;
	double accelerationYSquared = this->accelerationY * this->accelerationY;
	double accelerationZSquared = this->accelerationZ * this->accelerationZ;
	this->pitch = 180 * atan(accelerationX/sqrt(accelerationYSquared + accelerationZSquared))/M_PI;
	this->roll = 180 * atan(accelerationY/sqrt(accelerationXSquared + accelerationZSquared))/M_PI;
}

int IMU::readFullSensorState(){
    //std::cout << "Starting IMU I2C sensor state read" << std::endl;
    char namebuf[MAX_BUS];
   	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
    int file;
    if ((file = open(namebuf, O_RDWR)) < 0){
            std::cout << "Failed to open IMU Sensor on " << namebuf << " I2C Bus" << std::endl;
            return(1);
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){
            std::cout << "I2C_SLAVE address " << I2CAddress << " failed..." << std::endl;
            return(2);
    }

    // According to the BMA180 datasheet on page 59, you need to send the first address
    // in write mode and then a stop/start condition is issued. Data bytes are
    // transferred with automatic address increment.
    char buf[1] = { 0x00 };
    if(write(file, buf, 1) != 1){
    	std::cout << "Failed to Reset Address in readFullSensorState() " << std::endl;
    }

    int numberBytes = BMA180_I2C_BUFFER;
    int bytesRead = read(file, this->dataBuffer, numberBytes);
    if (bytesRead == -1){
    	std::cout << "Failure to read Byte Stream in readFullSensorState()" << std::endl;
    }
    close(file);

    if (this->dataBuffer[0]!= 0xfd){
    	std::cout << "MAJOR FAILURE: DATA WITH IMU HAS LOST SYNC! " << std::endl;
    }

    //cout << "Number of bytes read was " << bytesRead << endl;
    //for (int i=0; i<8; i++){
    //       printf("Byte %02d is 0x%02x\n", i, dataBuffer[i]);
    //}
    //cout << "Closing BMA180 I2C sensor state read" << endl;

    this->accelerationX = convertAcceleration(ACC_X_MSB, ACC_X_LSB);
    this->accelerationY = convertAcceleration(ACC_Y_MSB, ACC_Y_LSB);
    this->accelerationZ = convertAcceleration(ACC_Z_MSB, ACC_Z_LSB);

    this->calculatePitchAndRoll();
    //cout << "Pitch:" << this->getPitch() << "   Roll:" << this->getRoll() <<  endl;
    return 0;
}

int IMU::convertAcceleration(int msb_reg_addr, int lsb_reg_addr){
	//std::cout << "Converting " << (int) dataBuffer[msb_reg_addr] << " and " << (int) dataBuffer[lsb_reg_addr] << std::endl;
	short temp = dataBuffer[msb_reg_addr];
	temp = (temp<<8) | dataBuffer[lsb_reg_addr];
	//temp = temp>>2;
	temp = ~temp + 1;		//convert from 2's compliment
//	cout << "The X acceleration is " << temp << endl;
	return temp;
}

void IMU::displayMode(int iterations){
	for(int i=0; i<iterations; i++){
		this->readFullSensorState();
		printf("Rotation (%d, %d, %d)", accelerationX, accelerationY, accelerationZ);
	}
}

//  Temperature in 2's complement has a resolution of 0.5K/LSB
//  80h is lowest temp - approx -40C and 00000010 is 25C in 2's complement
//  this value is offset at room temperature - 25C and accurate to 0.5K

//need to update to match IMU registers and add lsb and hsb
float IMU::getTemperature(){
	int offset = -40;  // -40 degrees C
	this->readFullSensorState();
	char temp = dataBuffer[TEMP_LSB]; // = -80C 0b10000000  0b00000010; = +25C
	//char temp = this->readI2CDeviceByte(TEMP);
	//this->readFullSensorState();
    //char temp = dataBuffer[TEMP];
	int temperature;
	if(temp&0x80)	{
		temp = ~temp + 0b00000001;
		temperature = 128 - temp;
	}
	else {
		temperature = 128 + temp;
	}
	this->temperature = offset + ((float)temperature*0.5f);
	//cout << "The temperature is " << this->temperature << endl;
	//int temp_off = dataBuffer[0x37]>>1;
	//cout << "Temperature offset raw value is: " << temp_off << endl;
	return this->temperature;
}

/*int IMU::getAccelerationX() {
	unsigned short x;
	x = (float)accelerationX/16.384;
	return x;
	return accelerationX;
}

int IMU::getAccelerationY() {
	unsigned short y;
	y = (float)accelerationY/16.384;
	return y;
	return accelerationY;
}

int IMU::getAccelerationZ() {
	unsigned short z;
	z = (float)accelerationZ/16.384;
	return z;
	return accelerationZ;
}*/

//update to match IMU registers
IMU_SCALE IMU::getRange(){
	this->readFullSensorState();
	char temp = dataBuffer[RANGE];
	//char temp = this->readI2CDeviceByte(RANGE);  //bits 3,2,1
	temp = temp & 0b00001110;
	temp = temp>>1;
	//cout << "The current range is: " << (int)temp << endl;
	this->range = (IMU_SCALE) temp;
	return this->range;
}

//update to match IMU registers
int IMU::setRange(IMU_SCALE range){
	//char current = this->readI2CDeviceByte(RANGE);  //bits 3,2,1
	this->readFullSensorState();
	char current = dataBuffer[RANGE];
	char temp = range << 1; //move value into bits 3,2,1
	current = current & 0b11110001; //clear the current bits 3,2,1
	temp = current | temp;
	if(this->writeI2CDeviceByte(RANGE, temp)!=0){
		std::cout << "Failure to update RANGE value" << std::endl;
		return 1;
	}
	return 0;
}

//update to match IMU registers, also called sample rate in IMU, register 25
BMA180_BANDWIDTH IMU::getBandwidth(){
	this->readFullSensorState();
	char temp = dataBuffer[BANDWIDTH];   //bits 7->4
	//char temp = this->readI2CDeviceByte(BANDWIDTH);  //bits 7,6,5,4
//	cout << "The value of bandwidth returned is: " << (int)temp << endl;
	temp = temp & 0b11110000;
	temp = temp>>4;
//	cout << "The current bandwidth is: " << (int)temp << endl;
	this->bandwidth = (BMA180_BANDWIDTH) temp;
	return this->bandwidth;
}

//update to match IMU registers, also called sample rate in IMU, register 25
int IMU::setBandwidth(BMA180_BANDWIDTH bandwidth){
	//char current = this->readI2CDeviceByte(BANDWIDTH);  //bits 7,6,5,4
	this->readFullSensorState();
    char current = dataBuffer[BANDWIDTH];   //bits 7->4
	char temp = bandwidth << 4; //move value into bits 7,6,5,4
	current = current & 0b00001111; //clear the current bits 7,6,5,4
	temp = current | temp;
	if(this->writeI2CDeviceByte(BANDWIDTH, temp)!=0){
		std::cout << "Failure to update BANDWIDTH value" << std::endl;
		return 1;
	}
	return 0;
}

//update to match IMU registers
BMA180_MODECONFIG IMU::getModeConfig(){
	//char temp = dataBuffer[MODE_CONFIG];   //bits 1,0
	//char temp = this->readI2CDeviceByte(MODE_CONFIG);  //bits 1,0
	this->readFullSensorState();
    char temp = dataBuffer[MODE_CONFIG];
	temp = temp & 0b00000011;
	//cout << "The current mode config is: " << (int)temp << endl;
	this->modeConfig = (BMA180_MODECONFIG) temp;
	return this->modeConfig;
}

int IMU::writeI2CDeviceByte(char address, char value){
    std::cout << "Starting BMA180 I2C sensor state write" << std::endl;
    char namebuf[MAX_BUS];
   	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
    int file;
    if ((file = open(namebuf, O_RDWR)) < 0){
            std::cout << "Failed to open IMU Sensor on " << namebuf << " I2C Bus" << std::endl;
            return(1);
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){
            std::cout << "I2C_SLAVE address " << I2CAddress << " failed..." << std::endl;
            return(2);
    }

    // need to set the ctrl_reg0 ee_w bit. With that set the image registers change properly.
    // need to do this or can't write to 20H ... 3Bh
    // Very Important... wrote a 0x10 to 0x0D and it worked!!!
    //   char buf[2];
    //     buf[0] = BANDWIDTH;
    //     buf[1] = 0x28;
    //     buf[2] = 0x65;
    //  if ( write(file,buf,2) != 2) {
    //	  cout << "Failure to write values to I2C Device " << endl;
    //  }

    char buffer[2];
    	buffer[0] = address;
    	buffer[1] = value;
    if ( write(file, buffer, 2) != 2) {
        std::cout << "Failure to write values to I2C Device address." << std::endl;
        return(3);
    }
    close(file);
    std::cout << "Finished IMU I2C sensor state write" << std::endl;
    return 0;
}

/*
char BMA180Accelerometer::readI2CDeviceByte(char address){
  //  cout << "Starting BMA180 I2C sensor state byte read" << endl;
    char namebuf[MAX_BUS];
   	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
    int file;
    if ((file = open(namebuf, O_RDWR)) < 0){
            cout << "Failed to open BMA180 Sensor on " << namebuf << " I2C Bus" << endl;
            return(1);
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){
            cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;
            return(2);
    }
    // According to the BMA180 datasheet on page 59, you need to send the first address
    // in write mode and then a stop/start condition is issued. Data bytes are
    // transferred with automatic address increment.
    char buf[1] = { 0x00 };
    if(write(file, buf, 1) !=1){
    	cout << "Failed to Reset Address in readFullSensorState() " << endl;
    }
    char buffer[1];
    	buffer[0] = address;
    if ( read(file, buffer, 2) != 2) {
        cout << "Failure to read value from I2C Device address." << endl;
    }
    close(file);
   // cout << "Finished BMA180 I2C sensor state read" << endl;
    return buffer[0];
}*/


IMU::~IMU() {
	// TODO Auto-generated destructor stub
}
