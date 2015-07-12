/*
 * Rover5.h
 *
 *  Created on: Jul 11, 2015
 *      Author: travis
 */

#ifndef ROVER5_H_
#define ROVER5_H_

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <Rover5Coms.h>
#include <pthread.h>		//TODO switch from pthread to thread
#include "PRU.h"
#include "IMU.h"
#include "ClientSocket.h"
#include "Menu.h"

class Rover{
public:
	struct flow_gates{
		bool socketAlive;
		bool server_connected;
		bool transmitting;
		bool quit;
		bool stop_transmitting;
		bool error1_printed;
		bool error2_printed;
	};

	Rover();
	void init_flow_gates(flow_gates &ref);
	void set_flow_gates(flow_gates *ref);
	void get_flow_gates(flow_gates *ref);
	~Rover();

	flow_gates fg_vals;

private:
};

#endif /* ROVER5_H_ */
