#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "../server/main_server.h"
#include "game.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/*
 This file was written for instruction purposes for the
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering.
Last updated by Amnon Drory, Winter 2011.
 */
 /*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

int number_of_player=0 ;
int game_on;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define NUM_OF_WORKER_THREADS 2

#define MAX_LOOPS 2

#define SEND_STR_SIZE 35

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
static DWORD ServiceThread(thread_service_arg *thread_argv);

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int MainServer(int port)
{
	thread_service_arg player_array[2];
	int Ind;
	int Loop;
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;

	// Initialize Winsock.
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return;
	}

	/* The WinSock DLL is acceptable. Proceed. */

	// Create a socket.    
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		goto server_cleanup_1;
	}

	// Bind the socket.
	/*
		For a server to accept client connections, it must be bound to a network address within the system.
		The following code demonstrates how to bind a socket that has already been created to an IP address
		and port.
		Client applications use the IP address and port to connect to the host network.
		The sockaddr structure holds information regarding the address family, IP address, and port number.
		sockaddr_in is a subset of sockaddr and is used for IP version 4 applications.
   */
   // Create a sockaddr_in object and set its values.
   // Declare variables

	Address = inet_addr(SERVER_ADDRESS_STR);
	if (Address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
			SERVER_ADDRESS_STR);
		goto server_cleanup_2;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = Address;
	service.sin_port = htons(port);
	bindRes = bind(MainSocket, (SOCKADDR*)&service, sizeof(service));
	if (bindRes == SOCKET_ERROR)
	{
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Listen on the Socket.
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;

	printf("Waiting for a client to connect...\n");

	for (Loop = 0; Loop < MAX_LOOPS; Loop++)
	{
		player_array[Loop].player_socket = accept(MainSocket, NULL, NULL);
		if (player_array[Loop].player_socket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			goto server_cleanup_3;
		}

		printf("Clients Connected game start.\n");
		game_start();
		Ind = FindFirstUnusedThreadSlot();

		if (Ind == NUM_OF_WORKER_THREADS) //no slot is available
		{
			printf("No slots available for client, dropping the connection.\n");
			closesocket(player_array[Loop].player_socket); //Closing the socket, dropping the connection.
		}
		else
		{

			ThreadInputs[Ind] = player_array[Loop].player_socket;
			ThreadHandles[Ind] = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)ServiceThread,
				&player_array[Loop],
				0,
				NULL
			);
		}
	} 

server_cleanup_3:

	CleanupWorkerThreads();

server_cleanup_2:
	if (closesocket(MainSocket) == SOCKET_ERROR)
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError());

server_cleanup_1:
	if (WSACleanup() == SOCKET_ERROR)
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static int FindFirstUnusedThreadSlot()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] == NULL)
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], 0);

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static void CleanupWorkerThreads()
{
	int Ind;

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
	{
		if (ThreadHandles[Ind] != NULL)
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject(ThreadHandles[Ind], INFINITE);

			if (Res == WAIT_OBJECT_0)
			{
				closesocket(ThreadInputs[Ind]);
				CloseHandle(ThreadHandles[Ind]);
				ThreadHandles[Ind] = NULL;
				break;
			}
			else
			{
				printf("Waiting for thread failed. Ending program\n");
				return;
			}
		}
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Service thread is the thread that opens for each successful client connection and "talks" to the client.
static DWORD ServiceThread(thread_service_arg* thread_argv)
{
	char SendStr[SEND_STR_SIZE], *recv =NULL;
	int  state = 0;
	BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	char* AcceptedStr = NULL;
	while (1) {
		switch (state)
		{
		case 0:
			RecvRes = ReceiveString(&recv, thread_argv->player_socket);
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
				printf("thie messafe from client is:%s\n", recv);
			}
			if (strstr(recv, CLIENT_REQUEST)) {
				strcpy(thread_argv->player_name, recv);//parser
				free(recv);
				SendRes = SendString(SERVER_APPROVED, thread_argv->player_socket);
				if (SendRes == TRNS_FAILED)
				{
					printf("Service socket error while writing, closing thread.\n");
					closesocket(thread_argv->player_socket);
					return 1;
				}
				state = 1;
			
				break;
			}
			free(recv);
			break;
		case 1:
			SendRes = SendString(SERVER_MAIN_MENU, thread_argv->player_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(thread_argv->player_socket);
				return 1;
			}
			recv = NULL;
			RecvRes = ReceiveString(&recv, thread_argv->player_socket);
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
				printf("thie messafe from client is:%s\n", recv);
			}
			if (strstr(recv, CLIENT_VERSUS)) {
				free(recv);
				number_of_player++;
				if (number_of_player == 1)
					thread_argv->player_index = 1;
				if (number_of_player == 2)
					thread_argv->player_index = 2;
				while (number_of_player < 2);
				state = 2;

				break;
			}
			break;
		case 2:
			printf("game start\n");
			SendRes = SendString(GAME_STARTED, thread_argv->player_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(thread_argv->player_socket);
				return 1;
			}
			if (thread_argv->player_index == 1)
				state = 3;
			if (thread_argv->player_index == 2)
				state = 3;
			break;
		case 3:

			while (!Done)
			{
				if (thread_argv->player_index == 1)
					WaitForSingleObject(semaphore_client_1_turn, INFINITE);
				if (thread_argv->player_index == 2)
					WaitForSingleObject(semaphore_client_2_turn, INFINITE);
				SendRes = SendString(SERVER_MOVE_REQUEST, thread_argv->player_socket);
				if (SendRes == TRNS_FAILED)
				{
					printf("Service socket error while writing, closing thread.\n");
					closesocket(thread_argv->player_socket);
					return 1;
				}
				char* AcceptedStr = NULL;

				RecvRes = ReceiveString(&AcceptedStr, thread_argv->player_socket);
				if (RecvRes == TRNS_FAILED)
				{
					printf("Service socket error while reading, closing thread.\n");
					closesocket(thread_argv->player_socket);
					return 1;
				}
				else if (RecvRes == TRNS_DISCONNECTED)
				{
					printf("Connection closed while reading, closing thread.\n");
					closesocket(thread_argv->player_socket);
					return 1;
				}
				else
				{
					printf("Got move : %s from %s \n", AcceptedStr, thread_argv->player_name);
				}

				free(AcceptedStr);
				//RecvRes = ReceiveString(&AcceptedStr, *t_socket);
			//	game_on = game_step();
				if(thread_argv->player_index == 1)
				ReleaseSemaphore(semaphore_client_2_turn, 1, NULL);
				if (thread_argv->player_index ==2)
					ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
				

			}
			break;
		/*case 4:
			if==
			WaitForSingleObject(semaphore_clinet_connect, INFINITE);
			state = 3;
			break;*/
		default:
			break;

		}


	}

	

	printf("Conversation ended.\n");
	closesocket(thread_argv->player_socket);
	return 0;
}









int main(int argc, char* argv[]) {
	int exitcode = -1;
	MainServer(atoi(argv[1]));
	return exitcode; // Returns 0 if returned sucsessfuly, 0x555 else
}



