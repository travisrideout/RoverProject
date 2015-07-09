/*
 * PRU.h
 *
 *  Created on: Dec 21, 2014
 *      Author: User
 */

#ifndef PRU_H_
#define PRU_H_

// Standard header files
#include <cstdio>
#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string>

// Driver header file
#include "prussdrv.h"
#include <pruss_intc_mapping.h>

#define PRU_NUM0 	 			0
#define PRU_NUM1 	 			1
#define DDR_BASEADDR     		0x80000000
#define OFFSET_DDR	 			0x00001000
#define OFFSET_SHAREDRAM 		2048		//equivalent with 0x00002000
#define PRUSS0_SHARED_DATARAM   4
#define FORWARD					1
#define REVERSE					0

class PRU {
public:
	//not used yet but may in future
	typedef struct sonar_struct{
		unsigned int duration;
		int distance;
		int ping_ready;
		int ping_counter;
		std::string name;
	} sonar_struct;

	typedef struct motion_struct{
		int period;
		int lDir;
		int rDir;
		int lPWMDuty;
		int rPWMDuty;
	} motion_struct;

	PRU();

	int GetPing();
	int GetLeftPos();
	int GetRightPos();

	//Motion modifiers
	int GetMotionVars(motion_struct*);
	int SetMotionVars(motion_struct*);

	~PRU();		//virtual

private:
	sonar_struct sonar_vars;
	motion_struct motion_vars;

	int error_cnt;
};

#endif /* PRU_H_ */
