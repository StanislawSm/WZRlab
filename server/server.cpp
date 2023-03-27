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
	int login;
};

struct User {
	double lastContact;
	struct sockaddr_in addres;
	MovableObject* car;
	int login;
};

map<int, User> allUsers;
map<int, User> savedUsers;
double timeout = 3000;

void sendFrameToAllUsers(Frame frameToSend, SOCKET s,  int slen) {
	int frameLenght = sizeof(frameToSend);
	auto it2 = allUsers.begin();
	while (it2 != allUsers.end()) {
		sendto(s, (char*)&frameToSend, frameLenght, 0, (struct sockaddr*)&it2->second.addres, slen);
		it2++;
	}
}

void deleteNonActivUsers(SOCKET s, int slen) {
	auto it = allUsers.begin();
	while (it != allUsers.end()) {
		if ((clock() - it->second.lastContact) > timeout) {
			Frame removalFrameToSend;
			removalFrameToSend.iID = it->first;
			removalFrameToSend.type = 1; //removal
			sendFrameToAllUsers(removalFrameToSend, s, slen);
			printf("client %d has been removed\n", it->first);
			savedUsers[it->first] = it->second;
			savedUsers[it->first].login = savedUsers.size();
			it = allUsers.erase(it);
		} else {
			it++;
		}
	}
}

pair<int, User> findSavedUserByLogin(int login) {
	pair<int, User> result;
	result.first = -1;
	auto user = savedUsers.begin();
	while(user != savedUsers.end()) {
		if (user->second.login == login) {
			result.first = user->first;
			result.second = user->second;
		}
		user++;
	}
	return result;
}

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
	while (true) {
		fflush(stdout);
		memset(frame, '\0', sizeof(Frame));

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, (char*)frame, sizeof(Frame), 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR && WSAGetLastError() != 10054) {
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		if (frame->type != 0) {
			printf("%s:%d    ", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			printf("ID: %d    type: %d\n", frame->iID, frame->type);
		}

		Frame frameToSend;
		memset(&frameToSend, '\0', sizeof(Frame));
		if (recv_len > 0) {
			switch (frame->type){

			case 0:
				if ((allUsers.size() == 0) || (allUsers[frame->iID].car == NULL)) {
					MovableObject* ob = new MovableObject();
					ob->iID = frame->iID;
					allUsers[frame->iID].car = ob;
					allUsers[frame->iID].lastContact = clock(); //dodajemy info o ostatniej aktywności klienta o danym id
					allUsers[frame->iID].addres = si_other;
					printf("client %d has been added\n", frame->iID);
				}

				allUsers[frame->iID].car->ChangeState(frame->state);   // aktualizacja stateu obiektu obcego 
				allUsers[frame->iID].lastContact = clock(); //dodajemy info o ostatniej aktywności klienta o danym id

				deleteNonActivUsers(s, slen);

				frameToSend.state = frame->state;
				frameToSend.iID = frame->iID;
				frameToSend.type = 0; //update stanu pojazdu
				sendFrameToAllUsers(frameToSend, s, slen);
				break;
			case 2:
				int login = frame->login;
				pair<int, User> userPair = findSavedUserByLogin(login);
				int id = userPair.first;
				User user = userPair.second;
				user.addres = si_other;

				if (id > 0) {
					frameToSend.state = user.car->state;
					frameToSend.iID = id;
					frameToSend.login = login;
					frameToSend.type = 2; //wysłanie zapisanego stateu
					sendto(s, (char*)&frameToSend, sizeof(frame), 0, (struct sockaddr*)&user.addres, slen);

					MovableObject* ob = new MovableObject();
					ob->iID = frame->iID;
					frameToSend.iID = frame->iID;
					frameToSend.type = 1; //removal
					sendFrameToAllUsers(frameToSend, s, slen);
					printf("client %d has been removed\n", frame->iID);
					savedUsers[frame->iID].addres = si_other;
					savedUsers[frame->iID].car = ob;
					savedUsers[frame->iID].lastContact = clock();
					savedUsers[frame->iID].login = savedUsers.size();
					allUsers.erase(frame->iID);
				}
				break;
			}
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}