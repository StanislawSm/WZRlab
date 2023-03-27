#include<stdio.h>
#include<winsock2.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <iterator> 
#include <map>
#include <iostream>

#include "objects.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define BUFLEN 512	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data
#pragma warning( disable : 4996)


using namespace std;

struct Frame {
	int iID;                                      
	int type;                                     
	ObjectState state;
};

struct User {
	double lastContact;
	struct sockaddr_in addres;
	MovableObject* car;
};

map<int, User> allUsers;
double timeout = 1000;




int main() {
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	WSADATA wsa;
	Frame* frame = new Frame();

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");



	//keep listening for data
	while (1) {
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(frame, '\0', sizeof(Frame));

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, (char*)frame, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR && WSAGetLastError() != 10054) {
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		//print details of the client/peer and the data received
		printf("%s:%d    ", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("ID: %d    type: %d\n", frame->iID, frame->type);
		
		ObjectState state = frame->state;
		if (recv_len > 0) {
			if ((allUsers.size() == 0) || (allUsers[frame->iID].car == NULL)) {
				MovableObject* ob = new MovableObject();
				ob->iID = frame->iID;
				allUsers[frame->iID].car = ob;
				allUsers[frame->iID].lastContact = clock(); //dodajemy info o ostatniej aktywności klienta o danym id
				allUsers[frame->iID].addres = si_other;
			}

			allUsers[frame->iID].car->ChangeState(state);   // aktualizacja stateu obiektu obcego 
			allUsers[frame->iID].lastContact = clock(); //dodajemy info o ostatniej aktywności klienta o danym id

			//usuwamy wszystkich nieaktywnych klientów
			auto it = allUsers.begin();
			while (it != allUsers.end()) {
				if ((clock() - it->second.lastContact) > timeout) {
					printf("client %d was removed\n", it->first);
					it = allUsers.erase(it);
				} else {
					it++;
				}
			}

			//wysyłanie info o zmianie gry do każdego klienta
			auto it2 = allUsers.begin();
			while (it2 != allUsers.end()) {
				Frame frameToSend;
				frameToSend.state = state;               // state własnego obiektu 
				frameToSend.iID = frame->iID;
				sendto(s, (char*)&frameToSend, recv_len, 0, (struct sockaddr*) &it2->second.addres, slen);  // wysłanie komunikatu do pozostałych aplikacji
				it2++;
			}
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}