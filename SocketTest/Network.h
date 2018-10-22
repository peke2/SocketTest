#pragma once

#include <winsock2.h>
#include <WS2tcpip.h>

class Network
{
	WSADATA wsaData;
	SOCKET listenSocket;
	SOCKET clientSocket;

public:
	Network();
	~Network();

	void init();
	void update();
};
