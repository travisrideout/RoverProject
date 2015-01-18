/*
 * ConsoleController.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: User
 */

#include "ConsoleController.h"

ConsoleController::ConsoleController() {
}

int ConsoleController::MotionValueModifier (PRU &pru){
	int exit = 0;
	do{
		pru.GetMotionVars(&scratch_motion_vars);
		std::cout << "Period = " << scratch_motion_vars.period <<
				" Left Duty = " << scratch_motion_vars.lPWMDuty <<
				"% Right Duty = " << scratch_motion_vars.rPWMDuty <<
				"% Left Direction = " << scratch_motion_vars.lDir <<
				" Right Direction = " << scratch_motion_vars.rDir <<  std::endl;
		std::cout << "Enter Period in Hz" << std::endl;
		std::cin >> scratch_motion_vars.period;
		std::cout << "Enter Left PWM Duty" << std::endl;
		std::cin >> scratch_motion_vars.lPWMDuty;
		std::cout << "Enter Right PWM Duty" << std::endl;
		std::cin >> scratch_motion_vars.rPWMDuty;
		std::cout << "Enter Left Direction" << std::endl;
		std::cin >> scratch_motion_vars.lDir;
		std::cout << "Enter Right Direction" << std::endl;
		std::cin >> scratch_motion_vars.rDir;
		std::cout << "Enter 0 to quit, any other number to continue" << std::endl;
		std::cin >> exit;
		pru.SetMotionVars(&scratch_motion_vars);
	}while(exit!=0);
	return 0;
}

int ConsoleController::KeyboardController (PRU &pru){
	std::cout << "use num pad for movement, 0 to exit" << std::endl;
	int input = 0;
	do{
		pru.GetMotionVars(&scratch_motion_vars);
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
			if(scratch_motion_vars.lPWMDuty <= 90){
				scratch_motion_vars.lPWMDuty += 10;
				scratch_motion_vars.rPWMDuty += 10;
			}
			break;
		case 9:
			if(scratch_motion_vars.lPWMDuty >= 10){
				scratch_motion_vars.lPWMDuty -= 10;
				scratch_motion_vars.rPWMDuty -= 10;
			}
			break;
		default:
			scratch_motion_vars.lDir = FORWARD;
			scratch_motion_vars.rDir = FORWARD;
			scratch_motion_vars.lPWMDuty = 0;
			scratch_motion_vars.rPWMDuty = 0;
			break;
		}
		pru.SetMotionVars(&scratch_motion_vars);
	}while(input != 0);
	return 0;
}

ConsoleController::~ConsoleController() {
}

