/*
 * ClientSocket.h
 *
 *  Created on: Dec 29, 2014
 *      Author: User
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>      	// Needed for memset
#include <sys/socket.h> 	// Needed for the socket functions
#include <netdb.h>      	// Needed for the socket functions
#include "PRU.h"			// to get/set PRU values
#include <Rover5Coms.h>
#include <pthread.h>		// for threading

extern bool socketAlive;
extern bool communicating;
extern pthread_mutex_t Lock;
extern PRU pru;

class ClientSocket {
public:
	ClientSocket();

	void* StartClient();
	static void* ClientThreadStarter(void*);
	int UseMessageData();

	//Message modifiers
	int GetMessageVars(data_struct*);
	int SetMessageVars(data_struct*);

	virtual ~ClientSocket();

private:
	data_struct msgSendData;
	data_struct msgRecvData;
	char msg[PACKET_SIZE];
};

#endif /* CLIENTSOCKET_H_ */
