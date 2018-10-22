//#include "stdafx.h"
#include "pch.h"
#include <stdio.h>
#include "Network.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LEN 512

Network::Network()
{
	listenSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;
}

Network::~Network()
{

}

void Network::init()
{
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		assert(!"WSAStartup failed");
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		assert(!"getaddrinfo failed");
	}

	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		WSACleanup();
		assert(!"socket failed");
	}

	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		assert(!"bind error");
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(listenSocket);
		WSACleanup();
		assert(!"listen failed");
	}

}


void Network::update()
{
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		closesocket(listenSocket);
		WSACleanup();
		assert(!"accept failed");
	}

	closesocket(listenSocket);

	char recvbuf[DEFAULT_BUFFER_LEN];
	int iResult, iSendResut;
	int recvbuflen = DEFAULT_BUFFER_LEN;

	do {
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			iSendResut = send(clientSocket, recvbuf, iResult, 0);
			if (iSendResut == SOCKET_ERROR)
			{
				closesocket(clientSocket);
				WSACleanup();
				assert(!"send failed");
			}
			printf("Bytes sent: %d\n", iSendResut);
		}
		else if (iResult==0) {
			printf("Connection closing...\n");
		}
		else {
			closesocket(clientSocket);
			WSACleanup();
			assert(!"recv failed");
		}
	} while (iResult > 0);

	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(clientSocket);
		WSACleanup();
		assert(!"shutdown failed");
	}

	closesocket(clientSocket);
	WSACleanup();
}



