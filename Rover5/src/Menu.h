/*
 * Menu.h
 *
 *  Created on: Jul 6, 2015
 *      Author: travis
 */

#ifndef MENU_H_
#define MENU_H_

#include "IMU.h"
#include "ClientSocket.h"

extern IMU imu;
extern ClientSocket tcp;
extern bool transmitting;
extern bool server_connected;
extern bool error1_printed;
extern bool error2_printed;
extern bool quit;
extern bool stop_transmitting;

class Menu{
public:
	Menu();
	static void* MenuThreadStarter(void*);
	~Menu();

private:
	void PrintMenu();
	void* Query();

};



#endif /* MENU_H_ */
