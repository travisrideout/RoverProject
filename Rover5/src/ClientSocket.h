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
#include "Rover5.h"
#include "Menu.h"

class Rover;		//forward declared, not sure why i had to do this

class ClientSocket {
public:
	ClientSocket(Rover *rover_ref, PRU *pru_ref);
	int CreateSocket(const char* ip,const char* port);
	int Connect();
	int Transmit();

	//Message modifiers
	int GetMessageVars(data_struct*);
	int SetMessageVars(data_struct*);

	~ClientSocket();

private:
	int UseMessageData();
	int SafeStop();

	Rover &roverRef;
	PRU &pruRef;

	PRU::motion_struct scratch_vars;

	data_struct msgSendData;
	data_struct msgRecvData;
	char msg[PACKET_SIZE];

	struct addrinfo host_info;      	// The struct that getaddrinfo() fills up with data.
	struct addrinfo *host_info_list; 	// Pointer to the linked list of host_info's.
	int socketfd;
};

#endif /* CLIENTSOCKET_H_ */
