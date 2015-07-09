//============================================================================
// Name        : Rover5.cpp
// Author      : Travis Rideout
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include "SimpleGPIO.h"
#include "PRU.h"
#include "IMU.h"
#include "ClientSocket.h"
#include "Menu.h"
#include <Rover5Coms.h>
#include <pthread.h>		// for threading
//TODO switch from pthread to thread
//TODO create task manager

//unsigned int LEDGPIO = 50;   // GPIO1_18 = (1x32) + 18 = 50

bool socketAlive = false;
bool communicating = false;
bool ready4data = true;
bool tryConnect = true;

pthread_mutex_t Lock;

PRU pru;
ClientSocket tcp;
Menu menu;
IMU imu (2, 0x68);
const char* ip = NULL;
const char* port = NULL;

void ForcedClose(int x){
	pru.~PRU();
	tcp.~ClientSocket();
	std::cout<<"Forced Close"<<std::endl;
}

int main(int argc, char *argv[]) {
	ip = argv[1];
	port = argv[2];
	void* status1;
	void* status2;

	pthread_t clientThread;				// create a thread id for coms
	pthread_t menuThread;				// create a thread id for menu
	pthread_mutex_init(&Lock, NULL);	// initialize thread mutex

	pthread_create(&clientThread, NULL, &ClientSocket::ClientThreadStarter, &tcp);	//start thread for tcp object
	pthread_create(&menuThread, NULL, &Menu::MenuThreadStarter, &menu);				//start thread for menu object

	//signal(SIGINT, &ForcedClose);	// to allow thread and pru deconstruction on ^c

	data_struct scratch_vars;

	int count = 0;
	int pingDelay = 15;
	while (socketAlive){
		while(communicating){
			if (ready4data){		//only pack data if ready to send
				pthread_mutex_lock(&Lock);
				tcp.GetMessageVars(&scratch_vars);
				if(count>pingDelay){									//time delay to avoid ping echo
					scratch_vars.pingDist = pru.GetPing();
					count = 0;
					/*if(scratch_vars.pingDist<5000){						//avoid bad ping delay
						pingDelay = scratch_vars.pingDist/20;			//dynamically set ping delay, close = short delay
					}*/
				}else{
					count++;
				}
				scratch_vars.imuXAccel = imu.getAccelerationX();
				scratch_vars.imuYAccel = imu.getAccelerationY();
				scratch_vars.imuZAccel = imu.getAccelerationZ();
				scratch_vars.imuXGyro = imu.getGyroX();
				scratch_vars.imuYGyro = -imu.getGyroY();
				scratch_vars.imuZGyro = -imu.getGyroZ();
				scratch_vars.lPos = pru.GetLeftPos();
				scratch_vars.rPos = pru.GetRightPos();

				/*std::cout << "X Gyro = " << scratch_vars.imuXGyro
					<< "\t Y Gyro = " << scratch_vars.imuYGyro
					<< "\t Z Gyro = " << scratch_vars.imuZGyro << std::endl;*/

				tcp.SetMessageVars(&scratch_vars);
				pthread_mutex_unlock(&Lock);
				imu.readFullSensorState();
				ready4data = false;
			}
		}
	}

	/*cout << "Preparing to flash an LED on pin P8_14" << endl;
		gpio_export(LEDGPIO);    // The LED
		gpio_set_dir(LEDGPIO, OUTPUT_PIN);   // The LED is an output

		// Flash the LED 5 times
		for (int i = 0; i < 5; i++) {
			cout << "Setting the LED on" << endl;
			gpio_set_value(LEDGPIO, HIGH);5
			usleep(500000);         // on for 500ms
			cout << "Setting the LED off" << endl;
			gpio_set_value(LEDGPIO, LOW);
			usleep(500000);         // off for 500ms
		}
		cout << "Finished Testing the GPIO Pins" << endl;*/
	//gpio_unexport(LEDGPIO);     // unexport the LED

	//to launch exe
	//system("./hellobone");

	pthread_join(clientThread,&status1);
	pthread_join(menuThread,&status2);
	std::cout << "Joining threads" << std::endl;
	pthread_mutex_destroy(&Lock);
	std::cout << "Destroying mutex" << std::endl;

	return 0;
}
