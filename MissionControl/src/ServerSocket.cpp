/*
 * ServerSocket.cpp
 *
 *  Created on: Dec 30, 2014
 *      Author: User
 */

#include "ServerSocket.h"

ServerSocket::ServerSocket() {
	InitializeMessageData(msgSendData);
	pthread_mutex_init(&serverLock, NULL);		// initialize thread mutex
	pthread_create(&serverThread, NULL, &ServerSocket::ServerThreadStarter, this);	//start thread for object
}

void* ServerSocket::OpenServer() {
	unsigned short echoServPort = PORT;  		// First arg: local port
	std::cout << "Waiting for client to connect" << std::endl;

	try {
		TCPServerSocket servSock(echoServPort);     // Server Socket object

		for (;;) {   // Run forever
			HandleTCPClient(servSock.accept());       // Wait for a client to connect
		}
	} catch (SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	// NOT REACHED

	return 0;
}

void* ServerSocket::ServerThreadStarter(void* context){
	return ((ServerSocket *)context)->OpenServer();
}

// TCP client handling function
void ServerSocket::HandleTCPClient(TCPSocket *sock) {
	cout << "Handling client ";
	try {
		std::cout << sock->getForeignAddress() << ":";
	} catch (SocketException &e) {
		std::cerr << "Unable to get foreign address" << std::endl;
	}
	try {
		cout << sock->getForeignPort();
	} catch (SocketException &e) {
		std::cerr << "Unable to get foreign port" << std::endl;
	}
	std::cout << std::endl;

	// Receive and Send until the end of transmission
	int recvMsgSize;
	while ((recvMsgSize = sock->recv(msg, PACKET_SIZE)) > 0) { // Zero means end of transmission
		//cout << "Recv message = " << msg << endl;
		pthread_mutex_lock(&serverLock);
		ParseRecvPacket(msg, msgRecvData);
		pthread_mutex_unlock(&serverLock);
		UseMessageData();
		Sleep(30);// Sleep for 30ms to reduce CPU usage
		pthread_mutex_lock(&serverLock);
		PrepareSendPacket(msg, msgSendData);
		pthread_mutex_unlock(&serverLock);
		//cout << "Send message = " << msg << endl;
		sock->send(msg, PACKET_SIZE); 					//(echoBuffer, recvMsgSize)
	}
	delete sock;
	std::cout << "end of transmission" << std::endl;
}

int ServerSocket::UseMessageData(){
	std::cout << "\tPing = " << msgRecvData.pingDist
			<< "\tX accel = " <<  (float)msgRecvData.imuXAccel/16384
			<< "\tY accel = " << (float)msgRecvData.imuYAccel/16384
			<< "\tZ accel = " << (float)msgRecvData.imuZAccel/16384 << std::endl;
	return 0;
}


int ServerSocket::GetSendMessageVars(data_struct *new_vars){
	*new_vars = msgSendData;
	return 1;
}

int ServerSocket::SetSendMessageVars(data_struct *new_vars){
	msgSendData = *new_vars;
	return 1;
}

int ServerSocket::GetRecvMessageVars(data_struct *new_vars){
	*new_vars = msgRecvData;
	return 1;
}

ServerSocket::~ServerSocket() {
	pthread_cancel(serverThread);
	pthread_mutex_destroy(&serverLock);
}

