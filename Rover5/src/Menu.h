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
extern bool ready4data;
extern bool tryConnect;
extern bool communicating;

class Menu{
public:
	Menu();
	static void* MenuThreadStarter(void*);
	void PrintMenu();
	void* Query();
	~Menu();	//virtual
private:
};



#endif /* MENU_H_ */
