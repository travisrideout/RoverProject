/*
 * ConsoleController.h
 *
 *  Created on: Jan 2, 2015
 *      Author: User
 */

#ifndef CONSOLECONTROLLER_H_
#define CONSOLECONTROLLER_H_

#include "PRU.h"

class ConsoleController {
public:
	ConsoleController();

	int MotionValueModifier (PRU&);
	int KeyboardController (PRU&);

	virtual ~ConsoleController();

private:
	PRU::motion_struct scratch_motion_vars;
};

#endif /* CONSOLECONTROLLER_H_ */
