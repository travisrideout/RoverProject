/*
 * CollisionAvoidance.cpp
 *
 *  Created on: Jan 17, 2015
 *      Author: User
 */

#include "CollisionAvoidance.h"

CollisionAvoidance::CollisionAvoidance(ServerSocket &sr) : serverRef(sr){
}

// Checks to see if close to object, if so then stops forward motion
int CollisionAvoidance::ProximityHalt(data_struct *scratchSend){
	serverRef.GetRecvMessageVars(&msgRecvData);
	if(msgRecvData.pingDist<10 && (scratchSend->lDirCmd==1 || scratchSend->rDirCmd==1)){
		scratchSend->lDutyCmd = 0;
		scratchSend->rDutyCmd = 0;
	}
	return 1;
}

CollisionAvoidance::~CollisionAvoidance() {
}

