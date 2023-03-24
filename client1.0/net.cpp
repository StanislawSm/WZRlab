#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <time.h>

#ifdef WIN32// VERSION FOR WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else // VERSION FOR LINUX
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#endif

#include "net.h"
#define SERVER "127.0.0.1"	//ip address of udp server
#define PORT 8888	//The port on which to listen for incoming data
extern FILE *f;
HANDLE threadSend;


void DieWithError(char *errorMessage)
{
	perror(errorMessage);
	//exit(1);
}
// #########################################
// ##############   UNICAST  ###############
// #########################################

unicast_net::unicast_net(int usPort) {
#ifdef WIN32
	WSAStartup(MAKEWORD(2, 2), &localWSA);
#endif
	/* Create a datagram/UDP socket */
	
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	memset(&udpServAddr, 0, sizeof(udpServAddr));   /* Zero out structure */
	udpServAddr.sin_family = AF_INET;                /* Internet address family */
	udpServAddr.sin_addr.s_addr = inet_addr(SERVER); /* Any incoming interface */
	udpServAddr.sin_port = htons(usPort);      /* Local port */
}

unicast_net::~unicast_net() {
#ifdef WIN32
	WSACleanup();
#endif
}


int unicast_net::send(char *buffer, char *servIP, unsigned short size) {

	udpServAddr.sin_addr.s_addr = inet_addr(servIP);
	sendto(sock, buffer, size, 0, (struct sockaddr*)&udpServAddr, sizeof(udpServAddr));
	return 0;
}

int unicast_net::reciv(char *buffer,unsigned long *IP_Sender,unsigned short size) {
	
	int ucleng = sizeof(udpClntAddr);
	int sSize = recvfrom(sock, buffer, size, 0, (struct sockaddr*)&udpServAddr, (socklen_t*)&ucleng);
	int resason = 0;
	if (sSize == SOCKET_ERROR) {
		resason = WSAGetLastError();
	}
	*IP_Sender= udpServAddr.sin_addr.s_addr;
	return sSize;
}
