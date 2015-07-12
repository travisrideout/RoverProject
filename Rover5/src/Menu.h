/*
 * Menu.h
 *
 *  Created on: Jul 6, 2015
 *      Author: travis
 */

#ifndef MENU_H_
#define MENU_H_

#include "Rover5.h"
#include "IMU.h"
#include "ClientSocket.h"

class Rover;
class ClientSocket;

extern Rover rover;
extern IMU imu;
extern ClientSocket tcp;

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
