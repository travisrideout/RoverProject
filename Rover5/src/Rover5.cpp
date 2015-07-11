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

bool socketAlive = false;
bool server_connected = false;
bool transmitting = true;
bool quit = false;
bool stop_transmitting = false;
bool error1_printed = false;
bool error2_printed = false;

pthread_t menuThread;		// create a thread id for menu

PRU pru;
ClientSocket tcp;
Menu menu;
IMU imu (2, 0x68);
const char* ip = NULL;
const char* port = NULL;

void ForcedClose(int x){
	pthread_cancel(menuThread);
	quit = 1;
}

int main(int argc, char *argv[]) {
	ip = argv[1];
	port = argv[2];
	void* status;
	pthread_create(&menuThread, NULL, &Menu::MenuThreadStarter, &menu);		//start thread for menu object

	signal(SIGINT, &ForcedClose);

	data_struct scratch_vars;

	socketAlive = tcp.StartClient();
	server_connected = tcp.Connect();

	short count = 0;
	short pingFreq = 5;
	short pingDelay = 30/pingFreq;		//TODO based on ROS frequency for now, dynamically set

	//Main loop
	while (socketAlive){
		while(server_connected){
			error1_printed=false;
			while (transmitting){
				error2_printed=false;
				tcp.GetMessageVars(&scratch_vars);
				if(count>pingDelay){						//ensure ping frequency
					scratch_vars.pingDist = pru.GetPing();
					count = 0;
				}else{
					count++;
					scratch_vars.pingDist = 0;
				}
				scratch_vars.imuXAccel = imu.getAccelerationX();
				scratch_vars.imuYAccel = imu.getAccelerationY();
				scratch_vars.imuZAccel = imu.getAccelerationZ();
				scratch_vars.imuXGyro = imu.getGyroX();
				scratch_vars.imuYGyro = imu.getGyroY();
				scratch_vars.imuZGyro = imu.getGyroZ();
				scratch_vars.lPos = pru.GetLeftPos();
				scratch_vars.rPos = pru.GetRightPos();

				tcp.SetMessageVars(&scratch_vars);
				transmitting = tcp.Transmit();
				imu.readFullSensorState();

				if(quit||stop_transmitting) {
					transmitting = false;
					break;
				}
			}
			if(!error2_printed){
				std::cerr << "Server no longer transmitting. Use menu to retry transmitting" << std::endl;
				error2_printed=true;
			}
			if(quit) break;
			sleep(1);
		}

		if(!error1_printed){
			std::cerr << "Connection with Server failed. Use menu to retry connection" << std::endl;
			error1_printed = true;
		}
		if(quit) break;
		sleep(1);
	}

	pthread_join(menuThread,&status);
	std::cout << "Joining threads" << std::endl;

	return 0;
}
