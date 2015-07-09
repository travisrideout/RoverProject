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
	std::cout << "\t" << "c" << "\t" << "Calibrate IMU" << std::endl;
	std::cout << "\t" << "h" << "\t" << "Print Menu" << std::endl;
	std::cout << "\t" << "t" << "\t" << "Connect Coms" << std::endl;
	std::cout << "**************************************************" << std::endl;
	std::cout << std::endl;
	std::cout << "Select menu item: press <ENTER>" <<  std::endl;
}

void* Menu::Query(){
	char input = 'q';
		do{
			std::cin >> input;
			switch (input) {
			case 'c':
				communicating = false;
				imu.calibrate();
				communicating = true;
				break;
			case 'h':
				PrintMenu();
				break;
			case 't':
				ready4data = false;
				tryConnect = true;
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

