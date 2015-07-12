/*
 * Menu.cpp
 *
 *  Created on: Jul 6, 2015
 *      Author: travis
 */

#include "Menu.h"

Menu::Menu(Rover *rover_ref, IMU *imu_ref, ClientSocket *tcp_ref): roverRef(*rover_ref), imuRef(*imu_ref), tcpRef(*tcp_ref){
}

void* Menu::MenuThreadStarter(void* context){
	return ((Menu *)context)->Query();
}

void Menu::PrintMenu(){
	std::cout << std::endl;
	std::cout << "**************************************************" << std::endl;
	std::cout << "\t" << "i" << "\t" << "Calibrate IMU" << std::endl;
	std::cout << "\t" << "h" << "\t" << "Print Menu" << std::endl;
	std::cout << "\t" << "c" << "\t" << "Retry Connect With Server" << std::endl;
	std::cout << "\t" << "t" << "\t" << "Retry Transmit Message" << std::endl;
	std::cout << "\t" << "q" << "\t" << "Exit program" << std::endl;
	std::cout << "**************************************************" << std::endl;
	std::cout << std::endl;
	std::cout << "Select menu item: press <ENTER>" <<  std::endl;
}

void* Menu::Query(){
	Rover::flow_gates fg_local;
	char input = 'q';
		do{
			std::cin >> input;
			roverRef.get_flow_gates(&fg_local);
			switch (input) {
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
			case 'h':
				PrintMenu();
				break;
			case 'c':
				fg_local.server_connected = tcpRef.Connect();
				fg_local.error1_printed = false;
				break;
			case 't':
				if(fg_local.server_connected){
					fg_local.transmitting = tcpRef.Transmit();
					fg_local.error2_printed = false;
				}else{
					std::cerr << "Cannot Transmit - No connection with server" << std::endl;
				}
				break;
			case 'q':
				fg_local.quit = true;
				break;
			default:
				std::cout << "bad input, try again" << std::endl;
			}
			roverRef.set_flow_gates(&fg_local);
		}while(input!='q');
		return 0 ;
}

Menu::~Menu(){
	std::cout << "Menu object terminated" << std::endl;
}

