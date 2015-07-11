/*
 * Menu.cpp
 *
 *  Created on: Jul 6, 2015
 *      Author: travis
 */

#include "Menu.h"

Menu::Menu(){
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
	char input = 'q';
		do{
			std::cin >> input;
			switch (input) {
			case 'i':
				if(transmitting){
					std::cout << "Stopping communication to Calibrate IMU" << std::endl;
					stop_transmitting = true;
					sleep(1);
				}
				imu.calibrate();
				stop_transmitting = false;
				transmitting = true;
				break;
			case 'h':
				PrintMenu();
				break;
			case 'c':
				server_connected = tcp.Connect();
				error1_printed = false;
				break;
			case 't':
				if(server_connected){
					transmitting = tcp.Transmit();
					error2_printed = false;
				}else{
					std::cerr << "Cannot Transmit - No connection with server" << std::endl;
				}
				break;
			case 'q':
				quit = true;
				break;
			default:
				std::cout << "bad input, try again" << std::endl;
			}
		}while(input!='q');
		return 0 ;
}

Menu::~Menu(){
	std::cout << "Menu object terminated" << std::endl;
}

