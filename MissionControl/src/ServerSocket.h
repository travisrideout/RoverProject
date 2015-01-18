/*
 * ServerSocket.h
 *
 *  Created on: Dec 30, 2014
 *      Author: User
 */

#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "PracticalSocket.h"  // For Socket, ServerSocket, and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <string.h>
#include <sstream>
#include <iomanip>			//for setw and setfill
#include <windows.h>		//for Sleep()
#include <Rover5Coms.h>
#include <pthread.h>		// for threading

class ServerSocket {
public:
	ServerSocket();
	void* OpenServer();
	static void* ServerThreadStarter(void*);
	void HandleTCPClient(TCPSocket*);
	int UseMessageData();

	//Message modifiers
	int GetSendMessageVars(data_struct*);
	int SetSendMessageVars(data_struct*);

	int GetRecvMessageVars(data_struct*);

	virtual ~ServerSocket();

private:
	data_struct msgSendData;
	data_struct msgRecvData;
	char msg[PACKET_SIZE];
	pthread_t serverThread;			// create a thread object for threading
	pthread_mutex_t serverLock;		// create a thread lock
};

#endif /* SERVERSOCKET_H_ */
