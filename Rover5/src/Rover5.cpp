//============================================================================
// Name        : Rover5.cpp
// Author      : Travis Rideout
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Rover5.h"

Rover rover;
PRU pru;
ClientSocket tcp;
Menu menu;
IMU imu (2, 0x68);
const char* ip = NULL;
const char* port = NULL;

pthread_t menuThread;		// create a thread id for menu

void ForcedClose(int x){
	pthread_cancel(menuThread);
	rover.fg_vals.quit = 1;
}

int main(int argc, char *argv[]) {
	ip = argv[1];
	port = argv[2];

	rover.init_flow_gates(rover.fg_vals);

	void* status;
	pthread_create(&menuThread, NULL, &Menu::MenuThreadStarter, &menu);		//start thread for menu object

	signal(SIGINT, &ForcedClose);

	data_struct scratch_vars;

	rover.fg_vals.socketAlive = tcp.StartClient();
	rover.fg_vals.server_connected = tcp.Connect();

	short count = 0;
	short pingFreq = 5;
	short pingDelay = 30/pingFreq;		//TODO based on ROS frequency for now, dynamically set

	//Main loop
	while (rover.fg_vals.socketAlive){
		while(rover.fg_vals.server_connected){
			rover.fg_vals.error1_printed=false;
			while (rover.fg_vals.transmitting){
				rover.fg_vals.error2_printed=false;
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
				rover.fg_vals.transmitting = tcp.Transmit();
				imu.readFullSensorState();

				if(rover.fg_vals.quit||rover.fg_vals.stop_transmitting) {
					rover.fg_vals.transmitting = false;
					break;
				}
			}
			if(!rover.fg_vals.error2_printed){
				std::cerr << "Server no longer transmitting. Use menu to retry transmitting" << std::endl;
				rover.fg_vals.error2_printed=true;
			}
			if(rover.fg_vals.quit) break;
			sleep(1);
		}

		if(!rover.fg_vals.error1_printed){
			std::cerr << "Connection with Server failed. Use menu to retry connection" << std::endl;
			rover.fg_vals.error1_printed = true;
		}
		if(rover.fg_vals.quit) break;
		sleep(1);
	}

	pthread_join(menuThread,&status);
	std::cout << "Joining threads" << std::endl;

	return 0;
}

Rover::Rover(){
}

void Rover::init_flow_gates(flow_gates &ref){
	ref.socketAlive = false;
	ref.server_connected = false;
	ref.transmitting = true;
	ref.quit = false;
	ref.stop_transmitting = false;
	ref.error1_printed = false;
	ref.error2_printed = false;
}

void Rover::set_flow_gates(flow_gates *ref){
	fg_vals = *ref;
}

void Rover::get_flow_gates(flow_gates *ref){
	*ref = fg_vals;
}

Rover::~Rover(){
}
