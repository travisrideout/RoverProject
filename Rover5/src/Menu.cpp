/*
 * Menu.cpp
 *
 *  Created on: Jul 6, 2015
 *      Author: travis
 */

#include "Menu.h"

Menu::Menu(Rover *rover_ref, IMU *imu_ref, ClientSocket *tcp_ref, PRU *pru_ref):
	roverRef(*rover_ref), imuRef(*imu_ref), tcpRef(*tcp_ref), pruRef(*pru_ref){
}

void* Menu::MenuThreadStarter(void* context){
	return ((Menu *)context)->Query();
}

void Menu::PrintMenu(){
	std::cout << std::endl;
	std::cout << "**************************************************" << std::endl;
	std::cout << "\t" << "c" << "\t" << "Retry Connect With Server" << std::endl;
	std::cout << "\t" << "h" << "\t" << "Print Menu" << std::endl;
	std::cout << "\t" << "k" << "\t" << "Keyboard Controller" << std::endl;
	std::cout << "\t" << "i" << "\t" << "Calibrate IMU" << std::endl;
	std::cout << "\t" << "q" << "\t" << "Exit program" << std::endl;
	std::cout << "\t" << "s" << "\t" << "Stop Transmitting" << std::endl;
	std::cout << "\t" << "t" << "\t" << "Retry Transmit Message" << std::endl;
	std::cout << "**************************************************" << std::endl;
	std::cout << std::endl;
	std::cout << "Select menu item: press <ENTER>" <<  std::endl;
}

void* Menu::Query(){
	Rover::flow_gates fg_local;
	PrintMenu();
	char input = 'q';
	do{
		std::cin >> input;
		roverRef.get_flow_gates(&fg_local);
		switch (input) {
		case 'c':
			if(!fg_local.server_connected){
				fg_local.server_connected = tcpRef.Connect();
				fg_local.error1_printed = false;
			}else{
				std::cerr << "Already Connected" << std::endl;
			}
			break;
		case 'h':
			PrintMenu();
			break;
		case 'i':
			if(fg_local.transmitting){
				std::cout << "Stopping communication to Calibrate IMU" << std::endl;
				fg_local.stop_transmitting = true;
				roverRef.set_flow_gates(&fg_local);
				sleep(1);
			}
			imuRef.calibrate();
			fg_local.stop_transmitting = false;
			fg_local.transmitting = true;
			break;
		case 'k':
			if(fg_local.transmitting){
				std::cout << "Stopping communication to Utilize Keyboard Controller" << std::endl;
				fg_local.stop_transmitting = true;
				roverRef.set_flow_gates(&fg_local);
				sleep(1);
			}
			KeyboardController();
			break;
		case 'q':
			fg_local.quit = true;
			break;
		case 's':
			if(fg_local.transmitting){
				std::cout << "Stopping Communication" << std::endl;
				fg_local.stop_transmitting = true;
			}else{
				std::cerr << "Not Currently Transmitting" << std::endl;
			}
			break;
		case 't':
			if(fg_local.server_connected){
				fg_local.transmitting = tcpRef.Transmit();
				fg_local.error2_printed = false;
			}else{
				std::cerr << "Cannot Transmit - No connection with server" << std::endl;
			}
			break;
		default:
			std::cout << "bad input, try again" << std::endl;
		}
		roverRef.set_flow_gates(&fg_local);
	}while(input!='q');
	return 0 ;
}

void Menu::KeyboardController (){
	std::cout << "use num pad for movement, 0 to exit" << std::endl;
	int input = 0;
	do{
		pruRef.GetMotionVars(&scratch_motion_vars);
		std::cout << 	" Left Duty = " << scratch_motion_vars.lPWMDuty <<
				"% Right Duty = " << scratch_motion_vars.rPWMDuty <<
				"% Left Direction = " << scratch_motion_vars.lDir <<
				" Right Direction = " << scratch_motion_vars.rDir <<  std::endl;
		std::cin >> input;
		switch (input) {
		case 8:
			scratch_motion_vars.lDir = FORWARD;
			scratch_motion_vars.rDir = FORWARD;
			break;
		case 2:
			scratch_motion_vars.lDir = REVERSE;
			scratch_motion_vars.rDir = REVERSE;
			break;
		case 4:
			scratch_motion_vars.lDir = REVERSE;
			scratch_motion_vars.rDir = FORWARD;
			break;
		case 6:
			scratch_motion_vars.lDir = FORWARD;
			scratch_motion_vars.rDir = REVERSE;
			break;
		case 5:
			scratch_motion_vars.lDir = FORWARD;
			scratch_motion_vars.rDir = FORWARD;
			scratch_motion_vars.lPWMDuty = 0;
			scratch_motion_vars.rPWMDuty = 0;
			break;
		case 0:
			scratch_motion_vars.lDir = FORWARD;
			scratch_motion_vars.rDir = FORWARD;
			scratch_motion_vars.lPWMDuty = 0;
			scratch_motion_vars.rPWMDuty = 0;
			break;
		case 7:
			if(scratch_motion_vars.lPWMDuty >= 10){
				scratch_motion_vars.lPWMDuty -= 10;
				scratch_motion_vars.rPWMDuty -= 10;
			}
			break;
		case 9:
			if(scratch_motion_vars.lPWMDuty <= 90){
				scratch_motion_vars.lPWMDuty += 10;
				scratch_motion_vars.rPWMDuty += 10;
			}
			break;
		default:
			scratch_motion_vars.lDir = FORWARD;
			scratch_motion_vars.rDir = FORWARD;
			scratch_motion_vars.lPWMDuty = 0;
			scratch_motion_vars.rPWMDuty = 0;
			break;
		}
		pruRef.SetMotionVars(&scratch_motion_vars);
	}while(input != 0);
	PrintMenu();
}

Menu::~Menu(){
	std::cout << "Menu object terminated" << std::endl;
}

