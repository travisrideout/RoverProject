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
extern bool server_connected;
extern bool transmitting;
extern PRU pru;
extern const char* ip;
extern const char* port;

class ClientSocket {
public:
	ClientSocket();
	int StartClient();
	int Connect();
	int Transmit();

	//Message modifiers
	int GetMessageVars(data_struct*);
	int SetMessageVars(data_struct*);

	~ClientSocket();

private:
	int UseMessageData();
	int SafeStop();

	data_struct msgSendData;
	data_struct msgRecvData;
	char msg[PACKET_SIZE];

	struct addrinfo host_info;      			// The struct that getaddrinfo() fills up with data.
	struct addrinfo *host_info_list; 			// Pointer to the linked list of host_info's.
	int socketfd;
};

#endif /* CLIENTSOCKET_H_ */
