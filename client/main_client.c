#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "../shared/helper_function.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*
 This file was written for instruction purposes for the
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering.
Last updated by Amnon Drory, Winter 2011.
 */
 /*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

SOCKET m_socket;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Reading data coming from the server
static DWORD RecvDataThread(char *server_message)
{
	TransferResult_t RecvRes;

	while (1)
	{
		
		RecvRes = ReceiveString(&server_message, m_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			printf("%s\n", server_message);
		}

		return 0; 
	}

	return 0;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Sending data to the server
static DWORD SendDataThread(char *argv[])
{
	char SendStr[256];
	TransferResult_t SendRes;
	
	
	while (1)
	{

		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard

		if (STRINGS_ARE_EQUAL(SendStr, "quit"))
			return 0x555; //"quit" signals an exit from the client side

		SendRes = SendString(SendStr, m_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainClient(char *ip,int port,char *argv[])
{
	SOCKADDR_IN clientService;
	HANDLE hThread[2];
	int state = 0;
	// Initialize Winsock.
	WSADATA wsaData; //Create a WSADATA object called wsaData.
	//The WSADATA structure contains information about the Windows Sockets implementation.

	//Call WSAStartup and check for errors.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.

	// Create a socket.
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip); //Setting the IP address to connect to
	clientService.sin_port = htons(port); //Setting the port to connect to.


	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		printf("Failed to connect.\n");
		WSACleanup();
		return;
	}
	printf("connect.\n");

	char SendStr[256], *recv = NULL, *input_client=NULL;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	int i = 1;
	while (i) {
		switch (state)
		{
		case 0:
			sprintf(SendStr, "%s:%s", CLIENT_REQUEST, argv[3]);
			SendRes = SendString(SendStr, m_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return 0x555;
			}
			RecvRes = ReceiveString(&recv, m_socket);
			if (RecvRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return 0x555;
			}
			else if (RecvRes == TRNS_DISCONNECTED)
			{
				printf("Server closed connection. Bye!\n");
				i = 0;
				return 0x555;
			}
			else
			{
				printf("thie messafe from server is:%s", recv);
			}
			if (strstr(recv, SERVER_APPROVED)) {
				state = 1;
				free(recv);
			}
			break;
		case 1:
			RecvRes = ReceiveString(&recv, m_socket);
			if (RecvRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return 0x555;
			}
			else if (RecvRes == TRNS_DISCONNECTED)
			{
				printf("Server closed connection. Bye!\n");
				i = 0;
				return 0x555;
			}
			else
			{
				printf("thie messafe from server is:%s", recv);

				if (strstr(recv, SERVER_MAIN_MENU)) {

					gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard

					if (STRINGS_ARE_EQUAL(SendStr, "1"))
					{
						state = 2;
						free(recv);
						break;
					}
					if (STRINGS_ARE_EQUAL(SendStr, "2"))
					{

						free(recv);
						break;
					}
					printf("this messase not good pls type again\n");
					free(recv);
					break;
				}
			}
			break;
		case 2:
			hThread[0] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)SendDataThread,
				argv,
				0,
				NULL
			);
			hThread[1] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)RecvDataThread,
				0,
				0,
				NULL
			);
			break;
		default:
			break;
		}
	}
	WaitForMultipleObjects(2, hThread, FALSE, INFINITE);

	TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	closesocket(m_socket);

	WSACleanup();

	return;
}






int main(int argc, char* argv[]) {
	int exitcode = -1;
	MainClient(argv[1],atoi(argv[2]),argv);
	return exitcode; // Returns 0 if returned sucsessfuly, 0x555 else
}



