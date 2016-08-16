#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "AES.h"

using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27007"

int __cdecl main(void) 
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE)
	{
		cout << "Error while getting input handle" << endl;
		return EXIT_FAILURE;
	}
	string KeySchedule[4][44];
	string Key[4][4] = 
	{
		"2b", "28", "ab", "09",
		"7e", "ae", "f7", "cf",
		"15", "d2", "15", "4f",
		"16", "a6", "34", "3c"
	};
	KeyExpansion(KeySchedule, Key);
	char Name[200];
	char ClientName[200];
	cout << "Enter your name: ";
	cin >> Name;
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
			cout << recvbuf << " is connected" << endl;
			strcpy_s(ClientName, recvbuf);
			char buff2[] = "sent message";

        // Echo the buffer back to the sender
            iSendResult = send( ClientSocket, Name, strlen(Name) + 1, 0 );
            if (iSendResult == SOCKET_ERROR) {
                //printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            //printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while(false);
	string msg;
	int i = 0;
	while(true)
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		string dec;
		dec = (const char*) recvbuf;
		string exit = ClientName;
		dec = Decrypt(dec, KeySchedule);
		exit += " has been disconnected";
		if (iResult == 0 || dec == exit)
		{
			printf("Connection closing...\n");
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		else if (iResult > 0) 
		{
			if(i != 0)
			{
				SetConsoleTextAttribute(hStdout, 0x0C);
				cout << ClientName << ": " ;
				SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				cout << dec << endl;
			}
		}
		else  {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		/*iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult == 0)
		{
			break;
		}*/
		}
		if(i != 0)
		{
			SetConsoleTextAttribute(hStdout, 0x09);
			cout << Name << ": ";
			SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
		getline(cin, msg);
		if(msg == "Exit")
		{
			msg = Name;
			msg += " has been disconnected";
			msg = Encrypt(msg, KeySchedule);
			iSendResult = send(ClientSocket, msg.c_str(), msg.size() + 1, 0 );
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		msg = Encrypt(msg, KeySchedule);
		iSendResult = send( ClientSocket, msg.c_str(), msg.size() + 1, 0 );
		if (iSendResult == SOCKET_ERROR) {
			//printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		i++;
	}

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
	system("pause");
    return 0;
}

