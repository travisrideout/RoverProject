/*
 * CollisionAvoidance.h
 *
 *  Created on: Jan 17, 2015
 *      Author: User
 */

#ifndef COLLISIONAVOIDANCE_H_
#define COLLISIONAVOIDANCE_H_

#include "ServerSocket.h"
#include <iostream>
#include <Rover5Coms.h>

class CollisionAvoidance {
public:
	CollisionAvoidance(ServerSocket&);

	int ProximityHalt(data_struct*);

	virtual ~CollisionAvoidance();

private:
	data_struct msgRecvData;
	data_struct msgSendData;
	ServerSocket &serverRef;

};

#endif /* COLLISIONAVOIDANCE_H_ */
