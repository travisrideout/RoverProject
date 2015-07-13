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
#include "PRU.h"

class Rover;
class ClientSocket;

class Menu{
public:
	Menu(Rover *rover_ref, IMU *imu_ref, ClientSocket *tcp_ref, PRU *pru_ref);
	static void* MenuThreadStarter(void*);
	~Menu();

private:
	void PrintMenu();
	void* Query();
	void KeyboardController();

	Rover &roverRef;
	IMU &imuRef;
	ClientSocket &tcpRef;
	PRU &pruRef;

	PRU::motion_struct scratch_motion_vars;

};



#endif /* MENU_H_ */
