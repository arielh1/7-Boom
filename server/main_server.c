#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../server/main_server.h"



/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
 ///to do list
 /// 
//
//
//OPPONENTS_NO_SERVE
///mem leak
/// time out in samporeim
/// 
 /*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <crtdbg.h>


int server_run = 1;
int number_of_player=0 ;
int game_on = 1;
int win;
char name_player[2][MAX_LEN_NAME] = {"",""};
char player_move[2][MAX_LEN_NAME] = { 0,0 };
int samp1;
int samp2;
int player_played;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define NUM_OF_WORKER_THREADS 3

#define SEND_STR_SIZE 350

HANDLE semaphore_write ;
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];

HANDLE semaphore_wait;

static int FindFirstUnusedThreadSlot();

static void CleanupWorkerThreads();

static DWORD ServiceThread(thread_service_arg *thread_argv);

static DWORD Exit_Thread(void) {
	
while (1) {
	char from_board[256];
	gets_s(from_board, sizeof(from_board));

	if (STRINGS_ARE_EQUAL(from_board, "exit")) {
		server_run = 0;

	TerminateThread(ThreadHandles[0], 0);
		TerminateThread(ThreadHandles[1], 0);
		TerminateThread(ThreadHandles[2], 0);
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
			set_timeout(player_array[index_player].player_socket, (DWORD)RESPOND_TIME);
				
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
	int state=3 ;
	RecvRes = ReceiveString(&AcceptedStr, thread_argv->player_socket);
	if (RecvRes == TRNS_FAILED){
		printf("Service socket error while reading, closing thread.\n");
		closesocket(thread_argv->player_socket);
		return ERROR_CODE;
	}
	else if (RecvRes == TRNS_DISCONNECTED){
		printf("Connection closed while reading, closing thread.\n");
		closesocket(thread_argv->player_socket);
		return ERROR_CODE;
	}
	printf("Got move : %s from %s \n", AcceptedStr, thread_argv->player_name);
	decode_message(AcceptedStr, message, "revice");
	strcpy(player_move[thread_argv->player_index - 1] , message->param[0]);
	if (write_to_file(file_name, message->log_file_format) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	sprintf(snum, "%d", number);
	if ((is_seven_boom(number) == 1) || (seven_appeared(snum) == 1)){
		char snum[50];
		sprintf(snum, "%d", number);
		if (strcmp("boom", message->param[0]) != 0){
			win = (thread_argv->player_index == 1) ? 2 : 1;
			number_of_player--;
			game_on = 0;
			state= 4;
		}
	}
	else {
		if (atoi(message->param[0]) != number) {
			number_of_player--;
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
		if (strlen(player_move[(thread_argv->player_index ) % 2])){
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

int server_opponent_quit(thread_service_arg* thread_argv) {
	Message  message;
	char SendStr[SEND_STR_SIZE];
	game_on = 0;
	number_of_player--;
	sprintf(SendStr,"%s", SERVER_OPPONENT_QUIT);
	decode_message(SendStr, &message, "sent");
	if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE)
	{
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	
	if (thread_argv->player_index == 1) {
		ReleaseSemaphore(semaphore_client_2_turn, 1, NULL);
	}
	if (thread_argv->player_index == 2) {
		ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
		samp1++;
		
	}
	return 5;
}

int game_run_one_turn(thread_service_arg* thread_argv,char file_name[SEND_STR_SIZE],int *number ) {
	Message  message;
	int state = 3;
	char SendStr[SEND_STR_SIZE];
		if (game_view(thread_argv, file_name) == ERROR_CODE)
			return ERROR_CODE;
		if (SendString(SERVER_MOVE_REQUEST, thread_argv->player_socket) == TRNS_FAILED) {
			closesocket(thread_argv->player_socket);
			return server_opponent_quit(thread_argv);
		}
		sprintf(SendStr, "%s", SERVER_MOVE_REQUEST);
		decode_message(SendStr, &message, "sent");
		if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		state = seven_boom(thread_argv, *number, &message, file_name);
		if (state == 3) {
			sprintf(SendStr, "GAME_VIEW:%s;%s;CONT\n", name_player[player_played], message.param[0]);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {

				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			decode_message(SendStr, &message, "sent");
			player_played = (player_played + 1) % 2;
			sprintf(SendStr, "TURN_SWITCH:%s\n", name_player[player_played]);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {

				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			decode_message(SendStr, &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		}
		else {
			sprintf(SendStr, "GAME_VIEW:%s;%s;END\n", name_player[player_played], message.param[0]);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {

				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			decode_message(SendStr, &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		}
		*number += 2;
		if (thread_argv->player_index == 1) {
			ReleaseSemaphore(semaphore_client_2_turn, 1, NULL);
		}
		if (thread_argv->player_index == 2) {
			ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
		}
		if (state == 4) {
			*number = player_played;
			return state;
		}
	
	return state;
}

int game_on_state(thread_service_arg* thread_argv, char file_name[SEND_STR_SIZE],int *number) {
	Message  message;
	char SendStr[SEND_STR_SIZE];
	int state=3;
	while (game_on)	{
		if (*number == 2 || *number == 1) {
			sprintf(SendStr, "TURN_SWITCH:%s\n", name_player[player_played]);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {

				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			decode_message(SendStr, &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		}
		if (thread_argv->player_index == 1) {
			WaitForSingleObject(semaphore_client_1_turn, INFINITE);
			samp1--;
		}
		if (thread_argv->player_index == 2) {
			WaitForSingleObject(semaphore_client_2_turn, INFINITE);
			samp2--;
		}
		if (game_on == 1) {
			state = game_run_one_turn(thread_argv, thread_argv->file_name, number);
		}
		else {
			if (number_of_player != 2) {
				sprintf(SendStr, "GAME_VIEW:%s;%s;END\n", name_player[player_played],player_move[player_played]);
				if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
					closesocket(thread_argv->player_socket);
					return server_opponent_quit(thread_argv);
				}
				decode_message(SendStr, &message, "sent");
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
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
				number_of_player--;
				ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
				return 1;
			}
			else {
				number_of_player--;
				if (SendString("SERVER_OPPONENT_QUIT\n", thread_argv->player_socket) == TRNS_FAILED) {
					printf("Service socket error while writing, closing thread.\n");
					closesocket(thread_argv->player_socket);
					return ERROR_CODE;
				}
				sprintf(SendStr, "%s", SERVER_OPPONENT_QUIT);
				decode_message(SendStr, &message, "sent");
				if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE)
				{
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				return 1;
			}
		}
	}
	return state;
}

int server_main_menu(thread_service_arg* thread_argv,char file_name[SEND_STR_SIZE],int *number) {
	int state = 1;
	char SendStr[SEND_STR_SIZE],*recv=NULL;

	TransferResult_t RecvRes;
	Message  message;
		strcpy(SendStr, "SERVER_MAIN_MENU\n");
		if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
			printf("Service socket error while writing, closing thread.\n");
			closesocket(thread_argv->player_socket);
			return ERROR_CODE;
		}
		decode_message(SendStr, &message, "sent");
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
			free_message(&message);
			free(recv);
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			free(recv);
			while (number_of_player == 2);
			WaitForSingleObject(semaphore_write, INFINITE);
			number_of_player++;
			if (number_of_player == 1) {
				player_played = 0;
				thread_argv->player_index = 1;
			}
			if (number_of_player == 2) {
				thread_argv->player_index = 2;
				ReleaseSemaphore(semaphore_wait, 1, NULL);
			}
			*number = thread_argv->player_index;
			strcpy(name_player[*number - 1], thread_argv->player_name);
			ReleaseSemaphore(semaphore_write, 1, NULL);
			if (number_of_player < 2) {
				WaitForSingleObject(semaphore_wait, RESPOND_TIME * 2);
				if (number_of_player < 2) {
					WaitForSingleObject(semaphore_write, INFINITE);
					number_of_player--;
					ReleaseSemaphore(semaphore_write, 1, NULL);
					strcpy(SendStr, "SERVER_NO_OPPONENTS\n");
					if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
						printf("Service socket error while writing, closing thread.\n");
						closesocket(thread_argv->player_socket);
						return ERROR_CODE;
					}
					decode_message(SendStr, &message, "sent");
					free_message(&message);
					if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
					return state;
				}
			}
			WaitForSingleObject(semaphore_write, INFINITE);
			game_on = 1;

			ReleaseSemaphore(semaphore_write, 1, NULL);
			state = 2;
			return state;
		}
		if (strstr(recv, CLIENT_DISCONNECT)) {
			closesocket(thread_argv->player_socket);
			decode_message(recv, &message, "received");
			free_message(&message);
			free(recv);
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return 5;
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
		if (rec_failed_disconnected(RecvRes, thread_argv) != 0){
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
	char SendStr[SEND_STR_SIZE], * recv = NULL, * input_file_str = NULL, file_name[SEND_STR_SIZE] = { 0 };
	int  state = 0, number=0;

	Message message;
	char* AcceptedStr = NULL;
	while (1) {
		switch (state){
		case 0: 
			if ((state = client_req_server_state(thread_argv, file_name)) == ERROR_CODE)
				return ERROR_CODE;
			  break;
		case 1: {
			if((state=server_main_menu(thread_argv, thread_argv->file_name,&number))== ERROR_CODE)
				return ERROR_CODE;
		}
		break;
		case 2:
			if (SendString(GAME_STARTED, thread_argv->player_socket) == TRNS_FAILED) {
				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
		strcpy(player_move[thread_argv->player_index - 1] , "");
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
			
			sprintf(SendStr, "%s:%s\n", GAME_ENDED, name_player[win - 1]);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			state = 1;
			break;
		case 5:
			return 5;
		default:
			break;
		}
	}
	
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
//	_CrtDumpMemoryLeaks();
	return 0;
}

int main(int argc, char* argv[]) {
	int exitcode = -1;
	semaphore_wait = CreateSemaphore(0, 0, 1, NULL);
	CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)Exit_Thread,
		NULL,
		0,
		NULL
	);
	MainServer(atoi(argv[1]));
	_CrtDumpMemoryLeaks();
	return exitcode; // Returns 0 if returned sucsessfuly, 0x555 else
}


