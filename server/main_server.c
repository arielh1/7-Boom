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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

int number_of_player=0 ;
int game_on = 1;
int win;
int samp1;
int samp2;
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

void MainServer(int port)
{
	thread_service_arg player_array[2];
	
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	
	int server_run = 1, ListenRes, bindRes, Ind, Loop = 0, index_player = 0;
	// Initialize Winsock.
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());                               
		return;
	}


 
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (MainSocket == INVALID_SOCKET)
	{
		printf("Error at socket( ): %ld\n", WSAGetLastError());
		goto server_cleanup_1;
	}

	

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

	
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR)
	{
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;
	Ind = 0;
	printf("Waiting for a client to connect...\n");
	semaphore_clinet_connect = CreateSemaphore(0, 2, 2, NULL);
	while (server_run) {
		
			WaitForSingleObject(semaphore_clinet_connect, INFINITE);
			player_array[Ind].player_socket = accept(MainSocket, NULL, NULL);
		
	
			if (player_array[Ind].player_socket == INVALID_SOCKET)
			{
				printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
				goto server_cleanup_3;
			}
			
				ThreadInputs[Ind] = player_array[Ind].player_socket;
				ThreadHandles[Ind] = CreateThread(
					NULL,
					0,
					(LPTHREAD_START_ROUTINE)ServiceThread,
					&player_array[Ind],
					0,
					NULL
				);
	
			Ind = FindFirstUnusedThreadSlot();
			if (ThreadHandles[0]==NULL|| ThreadHandles[1]==NULL)
				continue;
			printf(" 2 Clients Connected.\n");
			game_start();
			
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

int rec_failed_disconnected(TransferResult_t RecvRes, thread_service_arg* thread_argv){
	RecvRes = ReceiveString(&recv, thread_argv->player_socket);
	if (RecvRes == TRNS_FAILED)
	{
		printf("Socket error while trying to write data to socket\n");

		ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
		return 0x555;
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("Server closed connection. Bye!\n");

		ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
		return 0x555;
	}
	else
		return 0;

}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int send_failed(TransferResult_t SendRes, thread_service_arg* thread_argv) {
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(thread_argv->player_socket);

		ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
		return 1;
	}
	return 0;

}


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Service thread is the thread that opens for each successful client connection and "talks" to the client.
static DWORD ServiceThread(thread_service_arg* thread_argv)
{
	char SendStr[SEND_STR_SIZE], *recv =NULL;
	int  state = 0, number;
	BOOL Done = FALSE;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	char* AcceptedStr = NULL;
	while (1) {
		switch (state)
		{
		case 0:
			RecvRes = ReceiveString(&recv, thread_argv->player_socket);

			if(rec_failed_disconnected(RecvRes, thread_argv)==0)
			{
				printf("thie message from client is:%s\n", recv);
			}
			if (strstr(recv, CLIENT_REQUEST)) {
				strcpy(thread_argv->player_name, recv);
				free(recv);
				SendRes = SendString(SERVER_APPROVED, thread_argv->player_socket);
				send_failed(SendRes, thread_argv);
				state = 1;

				break;
			}
			free(recv);
			break;
		case 1:
			SendRes = SendString(SERVER_MAIN_MENU, thread_argv->player_socket);
			send_failed(SendRes, thread_argv);

			recv = NULL;
			RecvRes = ReceiveString(&recv, thread_argv->player_socket);

			rec_failed_disconnected(RecvRes, thread_argv);

			if (strstr(recv, CLIENT_VERSUS)) {
				free(recv);
				while (number_of_player == 2);
				number_of_player++;

				if (number_of_player == 1)
					thread_argv->player_index = 1;
				if (number_of_player == 2) {
				
					thread_argv->player_index = 2;
				}
				number = thread_argv->player_index;
				while (number_of_player < 2);
				
				game_on = 1;
			
				state = 2;

				break;
			}
			if (strstr(recv, CLIENT_DISCONNECT)) {
				printf("this message from client is:%s\n", recv);
				closesocket(thread_argv->player_socket);

				ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
				return 0;
			}
			break;
		case 2:
			printf("game start\n");
			SendRes = SendString(GAME_STARTED, thread_argv->player_socket);
			send_failed(SendRes, thread_argv);
			state = 3;
			break;
		case 3:

			while (game_on)
			{
				if (thread_argv->player_index == 1) {
					WaitForSingleObject(semaphore_client_1_turn, INFINITE);
					samp1--;
				}
				if (thread_argv->player_index == 2) {
					WaitForSingleObject(semaphore_client_2_turn, INFINITE);
					samp2--;
				}

				if (game_on == 1) {
					SendRes = SendString(SERVER_MOVE_REQUEST, thread_argv->player_socket);
					if (SendRes == TRNS_FAILED)
					{
						printf("Service socket error while writing, closing thread.\n");
						closesocket(thread_argv->player_socket);

						ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
						return 1;
					}

					char* AcceptedStr = NULL;
					char snum[50];
			//		char* AcceptedStr_demo = NULL;
					RecvRes = ReceiveString(&AcceptedStr, thread_argv->player_socket);
					if (RecvRes == TRNS_FAILED)
					{
						printf("Service socket error while reading, closing thread.\n");

						ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
						closesocket(thread_argv->player_socket);
						return 1;
					}
					else if (RecvRes == TRNS_DISCONNECTED)
					{
						printf("Connection closed while reading, closing thread.\n");

						ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
						closesocket(thread_argv->player_socket);
						return 1;
					}
					sprintf(snum, "%d", number);
					printf("Got move : %s from %s \n", AcceptedStr, thread_argv->player_name);
		//			strcpy(AcceptedStr_demo, AcceptedStr);
					if ((is_seven_boom(number) == 1) || (seven_appeared(snum)==1))
					{
						if (strcmp("boom", AcceptedStr) != 0)
						{
							state = 4;
							win = (thread_argv->player_index == 1) ? 2 : 1;
							Done = 1;
							game_on = 0;
							break;
						}

					}
					else {
						if (atoi(AcceptedStr) != number) {
							state = 4;
							game_on = 0;
							win = (thread_argv->player_index == 1) ? 2 : 1;
							Done = 1;
							break;
						}
					}
					free(AcceptedStr);
			
					if (thread_argv->player_index == 1) {
						ReleaseSemaphore(semaphore_client_2_turn, 1, NULL); 
					}

					if (thread_argv->player_index == 2) {
						ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
						samp1++;
					}
					number += 2;

				}
				else {
					sprintf(SendStr, "%s you won\n", GAME_ENDED);
					SendRes = SendString(SendStr, thread_argv->player_socket);
					if (SendRes == TRNS_FAILED)
					{
						printf("Service socket error while writing, closing thread.\n");
						closesocket(thread_argv->player_socket);

						ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
						return 1;
					}
					printf("game end \n");
					ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
					number_of_player--;
					state = 1;
				}
			}
			break;
		case 4:
			number_of_player--;
			if ((thread_argv->player_index == 1)) {
				ReleaseSemaphore(semaphore_client_2_turn, 1, NULL);
				samp2++;
			}
			if (thread_argv->player_index == 2 )
			{
				ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
				samp1++;
			}
			(thread_argv->player_index == win) ? sprintf(SendStr, "%s you won\n", GAME_ENDED) : sprintf(SendStr, "%s you lost\n", GAME_ENDED);
	
			SendRes = SendString(SendStr, thread_argv->player_socket);
			send_failed(SendRes, thread_argv);
		 printf("game end \n");
		 game_on = 0;
		
		 state = 1;
			break;
		default:
			break;

		}


	}


	ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
	printf("Conversation ended.\n");
	closesocket(thread_argv->player_socket);
	return 0;
}



int main(int argc, char* argv[]) {
	int exitcode = -1;
	MainServer(atoi(argv[1]));
	return exitcode; // Returns 0 if returned sucsessfuly, 0x555 else
}



