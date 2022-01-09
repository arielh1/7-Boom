#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "../server/main_server.h"
#include "game.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
 ///to do list
 /// 
//QUIT_OPPONENT_SERV
//SWITCH_TURN
//OPPONENTS_NO_SERVE
///mem leak

/// 
 /*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <crtdbg.h>
#include <time.h>

int server_run = 1;
int number_of_player=0 ;
int game_on = 1;
int win;
char *name_player[2];
char* player_move[2] = {NULL};
int samp1;
int samp2;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define NUM_OF_WORKER_THREADS 3

#define MAX_LOOPS 3

#define SEND_STR_SIZE 350

HANDLE semaphore_write;
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];



static int FindFirstUnusedThreadSlot();

static void CleanupWorkerThreads();

static DWORD ServiceThread(thread_service_arg *thread_argv);


static DWORD Exit_Thread(void) {
	
while (1) {
	char from_board[256];
	gets_s(from_board, sizeof(from_board));

	if (STRINGS_ARE_EQUAL(from_board, "exit")) {
		server_run = 0;
CleanupWorkerThreads();
	/*	TerminateThread(ThreadHandles[0], NULL);
		TerminateThread(ThreadHandles[1], NULL);
		TerminateThread(ThreadHandles[2], NULL);*/
_CrtDumpMemoryLeaks();
		exit( 0);
	}
}
}


void MainServer(int port){
	thread_service_arg player_array[3];
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	int  ListenRes, bindRes, Ind, Loop = 0, index_player = 0;
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (StartupRes != NO_ERROR){
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());                               
		return;
	}
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (MainSocket == INVALID_SOCKET){
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
	if (bindRes == SOCKET_ERROR){
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR){
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}
	
	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;
	Ind = 0;
	game_start();
	printf("Waiting for a client to connect...\n");
	semaphore_clinet_connect = CreateSemaphore(0, 0, 1, NULL);
	semaphore_write= CreateSemaphore(0, 1, 1, NULL);
	
	while (server_run) {
			player_array[index_player].player_socket = accept(MainSocket, NULL, NULL);
			if(0!=set_timeout(player_array[index_player].player_socket, (DWORD)RESPOND_TIME))
				goto server_cleanup_3;
			player_array[index_player].player_number = index_player;
			if (player_array[index_player].player_socket == INVALID_SOCKET)
			{
				printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
				goto server_cleanup_3;
			}
			printf("new connect witn index:%d\n", index_player);
				ThreadInputs[index_player] = player_array[index_player].player_socket;
				ThreadHandles[index_player] = CreateThread(
					NULL,
					0,
					(LPTHREAD_START_ROUTINE)ServiceThread,
					&player_array[index_player],
					0,
					NULL
				);
				if (index_player == 2) {
					WaitForSingleObject(ThreadHandles[index_player], INFINITE);
				}
		
				index_player = FindFirstUnusedThreadSlot();
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



int rec_failed_disconnected(TransferResult_t RecvRes, thread_service_arg* thread_argv){
	
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
		return 0;

}



int send_failed(TransferResult_t SendRes, thread_service_arg* thread_argv) {
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(thread_argv->player_socket);


		return 1;
	}
	return 0;

}



int set_timeout(SOCKET sock, DWORD timeout) {

	if (SUCCESS_CODE != setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(DWORD))) {
		printf("failed to set sockopt\n");
		return 1;
	}
	if (SUCCESS_CODE != setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(DWORD))) {
		printf("failed to set sockopt\n");
		return 1;
	}
	return 0;
}

int seven_boom( thread_service_arg* thread_argv,int number, Message *message,char * file_name) {

	char snum[50];
	char* AcceptedStr = NULL;
	TransferResult_t RecvRes;
	int state=0 ;
	RecvRes = ReceiveString(&AcceptedStr, thread_argv->player_socket);
	if (RecvRes == TRNS_FAILED)
	{
		printf("Service socket error while reading, closing thread.\n");

	
		closesocket(thread_argv->player_socket);
		return -1;
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("Connection closed while reading, closing thread.\n");
		closesocket(thread_argv->player_socket);
		return -1;
	}
	//to do game view
	printf("Got move : %s from %s \n", AcceptedStr, thread_argv->player_name);
	decode_message(AcceptedStr, message, "revice");
player_move[thread_argv->player_index-1]=message->param[0];
	if (write_to_file(file_name, message->log_file_format) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	sprintf(snum, "%d", number);
	if ((is_seven_boom(number) == 1) || (seven_appeared(snum) == 1))
	{
		char snum[50];
		sprintf(snum, "%d", number);

		if (strcmp("boom", message->param[0]) != 0)
		{
	
			win = (thread_argv->player_index == 1) ? 2 : 1;
	
			game_on = 0;
			state= 4;
		}

	}
	else {
		if (atoi(message->param[0]) != number) {
	
			game_on = 0;
			win = (thread_argv->player_index == 1) ? 2 : 1;
			
			state= 4;
		}
	}
	
	
	
	free(AcceptedStr);
	return state;
}


int game_view(thread_service_arg* thread_argv, char* file_name) {
	Message message;
	char message_to_client[100]; 
		if (player_move[(thread_argv->player_index ) % 2]!=NULL){
			(game_on) ? sprintf(message_to_client, "%s:%s;%s;CONT\n", GAME_VIEW, name_player[(thread_argv->player_index) % 2], player_move[(thread_argv->player_index) % 2]) : sprintf(message_to_client, "%s:%s;%s;END\n", GAME_VIEW, name_player[(thread_argv->player_index) % 2], player_move[(thread_argv->player_index) % 2]);

		if (SendString(message_to_client, thread_argv->player_socket) == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(thread_argv->player_socket);
			//	ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
			return 1;
		}
		decode_message(message_to_client, &message, "sent");
		if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
	}
	return 0;
}

int game_on_state(thread_service_arg* thread_argv, char* file_name,int *number) {
	Message  message;
	char SendStr[SEND_STR_SIZE];
	int state=3;
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
			game_view(thread_argv, file_name);
			if (SendString(SERVER_MOVE_REQUEST, thread_argv->player_socket) == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(thread_argv->player_socket);
				return ERROR_CODE;
			}
			decode_message("SERVER_MOVE_REQUEST", &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			state = seven_boom(thread_argv, *number,  &message, file_name);
			if (thread_argv->player_index == 1) {
				ReleaseSemaphore(semaphore_client_2_turn, 1, NULL);
			}
			if (thread_argv->player_index == 2) {
				ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
				samp1++;
				if (state == 4)
					return ERROR_CODE;
			}
			*number += 2;
		}
		else {
			sprintf(SendStr, "%s:%s\n", GAME_ENDED, thread_argv->player_name);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(thread_argv->player_socket);
				return ERROR_CODE;
			}
			decode_message("GAME_ENDED", &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
			number_of_player--;
			return 1;

		}



	}
	return state;
}

int client_req_server_state(thread_service_arg* thread_argv,char *file_name) {
	Message  message;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	char  *recv=NULL;
	int state = 0;

		RecvRes = ReceiveString(&recv, thread_argv->player_socket);
		if (rec_failed_disconnected(RecvRes, thread_argv) != 0)
		{
			return ERROR_CODE;
		}
		
		decode_message(recv, &message, "received");
		if (strstr(message.message_type, CLIENT_REQUEST)) {
			strcpy(thread_argv->player_name, message.param[0]);
			sprintf(file_name, "thread_log_%s.txt", thread_argv->player_name);
			strcpy(thread_argv->file_name, file_name);
		
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			free(recv);
			if (thread_argv->player_number > 1){
				SendRes = SendString(SERVER_DENIED, thread_argv->player_socket);
				if (SendRes == TRNS_FAILED){
					printf("Service socket error while writing, closing thread.\n");
					closesocket(thread_argv->player_socket);
					return ERROR_CODE;
				}
				decode_message("SERVER_DENIED", &message, "sent");
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				closesocket(thread_argv->player_socket);
				return 5;
			}
			
			decode_message("SERVER_APPROVED", &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			free_message(&message);
			SendRes = SendString(SERVER_APPROVED, thread_argv->player_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Service socket error while writing, closing thread.\n");
				closesocket(thread_argv->player_socket);
				return ERROR_CODE;
			}
			return 1;
		}
		return 0;
}

int server_state(thread_service_arg* thread_argv) {
	char SendStr[SEND_STR_SIZE], * recv = NULL, * input_file_str = NULL, file_name[1000] = { 0 };
	int  state = 0, number=0;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	Message message;
	char* AcceptedStr = NULL;

	while (1) {
		switch (state){
		case 0: 
			if ((state = client_req_server_state(thread_argv, file_name)) == ERROR_CODE)
				return ERROR_CODE;
			_CrtDumpMemoryLeaks();
			  break;
		case 1:{
			if (SendString(SERVER_MAIN_MENU, thread_argv->player_socket) == TRNS_FAILED){
				printf("Service socket error while writing, closing thread.\n");
				closesocket(thread_argv->player_socket);
				return ERROR_CODE;
			}
			decode_message("SERVER_MAIN_MENU", &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			recv = NULL;
			RecvRes = ReceiveString(&recv, thread_argv->player_socket);
			if (rec_failed_disconnected(RecvRes, thread_argv) != 0) {
				return ERROR_CODE;
			}
			if (strstr(recv, CLIENT_VERSUS)) {
				decode_message(recv, &message, "received");
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				free(recv);
				while (number_of_player == 2);
				WaitForSingleObject(semaphore_write, INFINITE);
				number_of_player++;
				if (number_of_player == 1)
					thread_argv->player_index = 1;
				if (number_of_player == 2) {

					thread_argv->player_index = 2;
				}
				number = thread_argv->player_index;
				ReleaseSemaphore(semaphore_write, 1, NULL);
				while (number_of_player < 2);
				WaitForSingleObject(semaphore_write, INFINITE);
				game_on = 1;
				name_player[number - 1] = thread_argv->player_name;
				ReleaseSemaphore(semaphore_write, 1, NULL);
				state = 2;
				break;
			}
			if (strstr(recv, CLIENT_DISCONNECT)) {
				closesocket(thread_argv->player_socket);
				decode_message(recv, &message, "received");
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				
				}
				
				return 1;
			}
		}
		break;
		case 2:
			SendRes = SendString(GAME_STARTED, thread_argv->player_socket);
			send_failed(SendRes, thread_argv);
			player_move[thread_argv->player_index - 1] = 0;
			decode_message("GAME_STARTED", &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			state = 3;
			break;
		case 3:
			if( (state=game_on_state(thread_argv, file_name, &number)) == ERROR_CODE)
				return ERROR_CODE;
			break;
		case 4:
			number_of_player--;
			sprintf(SendStr, "%s:%s\n", GAME_ENDED, name_player[win - 1]);
			SendRes = SendString(SendStr, thread_argv->player_socket);
			send_failed(SendRes, thread_argv);
			printf("game end \n");
			game_on = 0;
			state = 1;
			break;
		case 5:
			return 5;
		default:
			break;
		}
	}
	free(file_name);
	_CrtDumpMemoryLeaks();
	return 0;
}

static DWORD ServiceThread(thread_service_arg* thread_argv)
{
	char  * recv = NULL, * input_file_str = NULL, file_name[1000] = { 0 };
	int  state = 0, number=0,error_code=0;
	BOOL Done = FALSE;



	char* AcceptedStr = NULL;
	while ((error_code == 0) ) {
error_code = server_state(thread_argv);
if ((error_code == ERROR_CODE))
return ERROR_CODE;
	}
	
	DeleteFileA(thread_argv->file_name);
	closesocket(thread_argv->player_socket);
	_CrtDumpMemoryLeaks();
	return 0;
}

int main(int argc, char* argv[]) {
	int exitcode = -1;
	
	CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)Exit_Thread,
		NULL,
		0,
		NULL
	);
	MainServer(atoi(argv[1]));
	return exitcode; // Returns 0 if returned sucsessfuly, 0x555 else
}


