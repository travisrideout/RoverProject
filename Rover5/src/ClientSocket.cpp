/*
 * ClientSocket.cpp
 *
 *  Created on: Dec 29, 2014
 *      Author: User
 */

#include "ClientSocket.h"

ClientSocket::ClientSocket() {
	InitializeMessageData(msgSendData);
	socketAlive = true;
}

void* ClientSocket::ClientThreadStarter(void* context){
	return ((ClientSocket *)context)->StartClient();
}

void* ClientSocket::StartClient(){
	std::cout << "Starting Client" << std::endl;

	int status;
	struct addrinfo host_info;      			// The struct that getaddrinfo() fills up with data.
	struct addrinfo *host_info_list; 			// Pointer to the linked list of host_info's.
	memset(&host_info, 0, sizeof host_info);	//clear memory location assigned to host_info

	host_info.ai_family = AF_UNSPEC;     		// IP version not specified. Can be both.
	host_info.ai_socktype = SOCK_STREAM; 		// Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
	status = getaddrinfo("192.168.200.102", "12345",
			&host_info, &host_info_list); 		//assign IP and port values
	if (status != 0){							// check if the address was assigned correctly
		std::cout << "getaddrinfo error" << gai_strerror(status) ;
	}

	std::cout << "Creating a socket..."  << std::endl;
	int socketfd; 								// The socket descriptor
	socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
			host_info_list->ai_protocol);
	if (socketfd == -1){
		std::cout << "socket error " ;
	}

	std::cout << "Connecting..."  << std::endl;
	status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen); //wait here for connection
	if (status == -1){
		std::cout << "connect error";
	}else if (status == 0){
		std::cout << "Connected" << std::endl;
		communicating = true;
	}

	ssize_t bytes_recieved;
	do{
		// TODO need to add error check here
		pthread_mutex_lock(&Lock);
		PrepareSendPacket(msg, msgSendData);
		pthread_mutex_unlock(&Lock);
		//std::cout << "Send message = " << msg << std::endl;
		send(socketfd, msg, PACKET_SIZE, 0);
		bytes_recieved = recv(socketfd, msg, PACKET_SIZE, 0);
		// If no data arrives, the program will just wait here until some data arrives.
		if (bytes_recieved == 0) {
			std::cout << "host shut down." << std::endl;
			communicating = false;
			break;
		}
		if (bytes_recieved == -1){
			std::cout << "Receive error!" << std::endl;
			communicating = false;
			break;
		}
		//std::cout << "Recv message = " << msg << std::endl;
		ParseRecvPacket(msg, msgRecvData);
		UseMessageData();
	}while(bytes_recieved>0);

	std::cout << "Receiving complete. Closing socket..." << std::endl;
	freeaddrinfo(host_info_list);
	close(socketfd);

	socketAlive = false;
	std::cout << "Thread ending" << std::endl;

	return 0;
}

int ClientSocket::UseMessageData(){
	PRU::motion_struct scratch_vars;
	pru.GetMotionVars(&scratch_vars);
	scratch_vars.lDir = msgRecvData.lDirCmd;
	scratch_vars.rDir = msgRecvData.rDirCmd;
	scratch_vars.lPWMDuty = msgRecvData.lDutyCmd;
	scratch_vars.rPWMDuty = msgRecvData.rDutyCmd;
	pru.SetMotionVars(&scratch_vars);
	std::cout << "L_DIR = " << msgRecvData.lDirCmd << "\tL_POS = " << pru.GetLeftPos()
				<< "\tR_DIR = " << msgRecvData.rDirCmd << "\tR_POS = " << pru.GetRightPos()
				<< "\tL_Duty = " << msgRecvData.lDutyCmd << "\tR_Duty = " << msgRecvData.rDutyCmd
				<< std::endl;
	return 0;
}

int ClientSocket::GetMessageVars(data_struct *new_vars){
	*new_vars = msgSendData;
	return 1;
}

int ClientSocket::SetMessageVars(data_struct *new_vars){
	msgSendData = *new_vars;
	return 1;
}

ClientSocket::~ClientSocket() {
	std::cout << "ClientSocket object terminated" << std::endl;
}


