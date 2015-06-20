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
extern bool ready4data;
extern pthread_mutex_t Lock;
extern PRU pru;
extern const char* ip;
extern const char* port;

class ClientSocket {
public:
	ClientSocket();

	void* StartClient();
	static void* ClientThreadStarter(void*);

	//Message modifiers
	int GetMessageVars(data_struct*);
	int SetMessageVars(data_struct*);

	virtual ~ClientSocket();

private:
	int UseMessageData();
	int SafeStop();

	data_struct msgSendData;
	data_struct msgRecvData;
	char msg[PACKET_SIZE];
};

#endif /* CLIENTSOCKET_H_ */
