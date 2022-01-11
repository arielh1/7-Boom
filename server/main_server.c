
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4013)
#pragma warning(disable: 6258)
#include "main_server.h"
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "game.h"
#include "state_machine_server.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
static DWORD Exit_Thread(void) {

	while (1) {
		char from_board[256];
		int i;
		gets_s(from_board, sizeof(from_board));
		DWORD dwExitCode;
		if (STRINGS_ARE_EQUAL(from_board, "exit")) {
			printf("Server disconnected.Exiting.\n");
			server_run = 0;
			for (i = 0; i < 3; i++) {
				dwExitCode = TerminateThread(ThreadHandles[i], 0x555);

			}
			_CrtDumpMemoryLeaks();
			exit(0);

		}

	}
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void MainServer(int port) {
	thread_service_arg player_array[3];
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	int  ListenRes, bindRes, Ind, Loop = 0, index_player = 0;
	WSADATA wsaData;
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (StartupRes != NO_ERROR) {
		printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
		return;
	}
	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (MainSocket == INVALID_SOCKET) {
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
	if (bindRes == SOCKET_ERROR) {
		printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
		goto server_cleanup_2;
	}
	ListenRes = listen(MainSocket, SOMAXCONN);
	if (ListenRes == SOCKET_ERROR) {
		printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
		goto server_cleanup_2;
	}

	for (Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++)
		ThreadHandles[Ind] = NULL;
	Ind = 0;
	game_start();
	semaphore_clinet_connect = CreateSemaphore(0, 0, 1, NULL);
	semaphore_write = CreateSemaphore(0, 1, 1, NULL);

	while (server_run) {
		player_array[index_player].player_socket = accept(MainSocket, NULL, NULL);
		set_timeout(player_array[index_player].player_socket, (DWORD)RESPOND_TIME);

		player_array[index_player].player_number = index_player;
		if (player_array[index_player].player_socket == INVALID_SOCKET)
		{
			printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
			goto server_cleanup_3;
		}
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
int seven_boom(thread_service_arg* thread_argv, int number, Message* message) {
	char snum[50];
	char* AcceptedStr = NULL;
	TransferResult_t RecvRes;
	int state = 3;
	RecvRes = ReceiveString(&AcceptedStr, thread_argv->player_socket);
	if (rec_failed_disconnected(RecvRes) != 0) {
		return 5;
	}
	decode_message(AcceptedStr, message, "reviced from");
	strcpy(player_move[thread_argv->player_index - 1], message->param[0]);
	if (write_to_file(thread_argv->file_name, message->log_file_format) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	sprintf(snum, "%d", number);
	if ((is_seven_boom(number) == 1) || (seven_appeared(snum) == 1)) {
		char snum[50];
		sprintf(snum, "%d", number);
		if (strcmp("boom", message->param[0]) != 0) {
			win = (thread_argv->player_index == 1) ? 2 : 1;
			number_of_player--;
			game_on = 0;
			state = 4;
		}
	}
	else {
		if (atoi(message->param[0]) != number) {
			number_of_player--;
			game_on = 0;
			win = (thread_argv->player_index == 1) ? 2 : 1;
			state = 4;
		}
	}
	free(AcceptedStr);
	return state;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int game_view(thread_service_arg* thread_argv) {
	Message message;
	char message_to_client[100];
	if (strlen(player_move[(thread_argv->player_index) % 2])) {
		(game_on) ? sprintf(message_to_client, "%s:%s;%s;CONT\n", GAME_VIEW, name_player[(thread_argv->player_index) % 2], player_move[(thread_argv->player_index) % 2]) : sprintf(message_to_client, "%s:%s;%s;END\n", GAME_VIEW, name_player[(thread_argv->player_index) % 2], player_move[(thread_argv->player_index) % 2]);

		if (SendString(message_to_client, thread_argv->player_socket) == TRNS_FAILED)
		{

			closesocket(thread_argv->player_socket);
			//	ReleaseSemaphore(semaphore_clinet_connect, 1, NULL);
			return 1;
		}
		decode_message(message_to_client, &message, "sent to");
		free_message(&message);
		if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
	}
	return 0;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int server_opponent_quit(thread_service_arg* thread_argv) {
	game_on = 0;
	number_of_player--;
	if (write_to_file(thread_argv->file_name, SERVER_OPPONENT_QUIT) != SUCCESS_CODE)
	{
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	//free_message(&message);
	if (thread_argv->player_index == 1) {
		ReleaseSemaphore(semaphore_client_2_turn, 1, NULL);
	}
	if (thread_argv->player_index == 2) {
		ReleaseSemaphore(semaphore_client_1_turn, 1, NULL);
	}
	return 5;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int game_run_one_turn(thread_service_arg* thread_argv, int* number) {
	Message  message;
	int state = 3;
	char SendStr[SEND_STR_SIZE];
	if (game_view(thread_argv) == ERROR_CODE)
		return ERROR_CODE;
	if (SendString(SERVER_MOVE_REQUEST, thread_argv->player_socket) == TRNS_FAILED) {
		closesocket(thread_argv->player_socket);
		return server_opponent_quit(thread_argv);
	}
	sprintf(SendStr, "%s", SERVER_MOVE_REQUEST);
	decode_message(SendStr, &message, "sent to");
	free_message(&message);

	if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	state = seven_boom(thread_argv, *number, &message);

	if (state == 3) {
		sprintf(SendStr, "GAME_VIEW:%s;%s;CONT\n", name_player[player_played], message.param[0]);
		if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {

			closesocket(thread_argv->player_socket);
			return server_opponent_quit(thread_argv);
		}
		free_message(&message);

		player_played = (player_played + 1) % 2;
		sprintf(SendStr, "TURN_SWITCH:%s\n", name_player[player_played]);
		if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
			closesocket(thread_argv->player_socket);
			return server_opponent_quit(thread_argv);
		}
		decode_message(SendStr, &message, "sent to");
		free_message(&message);
		if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
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
		free_message(&message);

		decode_message(SendStr, &message, "sent to");
		free_message(&message);

		if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
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
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int game_on_state(thread_service_arg* thread_argv, int* number) {
	Message  message;
	char SendStr[SEND_STR_SIZE];
	int state = 3;
	while (game_on) {
		if (*number == 2 || *number == 1) {
			sprintf(SendStr, "TURN_SWITCH:%s\n", name_player[player_played]);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			decode_message(SendStr, &message, "sent to");
			if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			free_message(&message);
		}
		if (thread_argv->player_index == 1) {
			WaitForSingleObject(semaphore_client_1_turn, INFINITE);

		}
		if (thread_argv->player_index == 2) {
			WaitForSingleObject(semaphore_client_2_turn, INFINITE);

		}
		if (game_on == 1) {
			state = game_run_one_turn(thread_argv, number);
		}
		else {
			if (number_of_player != 2) {
				sprintf(SendStr, "GAME_VIEW:%s;%s;END\n", name_player[player_played], player_move[player_played]);
				if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
					closesocket(thread_argv->player_socket);
					return server_opponent_quit(thread_argv);
				}
				decode_message(SendStr, &message, "sent");
				free_message(&message);
				if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
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
				decode_message("GAME_ENDED", &message, "sent to");
				free_message(&message);
				if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
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
				decode_message(SendStr, &message, "sent to");
				free_message(&message);
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
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int server_main_menu(thread_service_arg* thread_argv, int* number) {
	int state = 1;
	char SendStr[SEND_STR_SIZE], * recv = NULL;
	TransferResult_t RecvRes;
	Message  message;
	set_timeout(thread_argv->player_socket, INFINITE);
	strcpy(SendStr, "SERVER_MAIN_MENU\n");
	if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
		printf("Service socket error while writing, closing thread.\n");
		closesocket(thread_argv->player_socket);
		return ERROR_CODE;
	}
	decode_message(SendStr, &message, "sent to");
	if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	free_message(&message);
	free(recv);
	recv = NULL;
	RecvRes = ReceiveString(&recv, thread_argv->player_socket);
	if (rec_failed_disconnected(RecvRes) != 0) {
		return ERROR_CODE;
	}
	if (strstr(recv, CLIENT_VERSUS)) {
		decode_message(recv, &message, "received from");
		free_message(&message);
		free(recv);
		if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
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
			WaitForSingleObject(semaphore_wait, RESPOND_TIME);
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
				decode_message(SendStr, &message, "sent to");
				free_message(&message);
				if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
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
		decode_message(recv, &message, "received from");
		free_message(&message);
		free(recv);
		if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		return 5;
	}
	return state;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int client_req_server_state(thread_service_arg* thread_argv) {
	Message  message;
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	int state = 0;
	char SendStr[SEND_STR_SIZE], * recv = NULL;
	RecvRes = ReceiveString(&recv, thread_argv->player_socket);
	if (rec_failed_disconnected(RecvRes) != 0) {
		return ERROR_CODE;
	}
	decode_message(recv, &message, "received from");
	if (strstr(message.message_type, CLIENT_REQUEST)) {
		strcpy(thread_argv->player_name, message.param[0]);
		sprintf(thread_argv->file_name, "thread_log_%s.txt", thread_argv->player_name);

		if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		free(recv);
		free_message(&message);
		if (thread_argv->player_number > 1) {
			SendRes = SendString(SERVER_DENIED, thread_argv->player_socket);
			if (SendRes == TRNS_FAILED) {
				printf("Service socket error while writing, closing thread.\n");
				closesocket(thread_argv->player_socket);
				return ERROR_CODE;
			}
			sprintf(SendStr, "%s", SERVER_DENIED);
			decode_message(SendStr, &message, "sent");

			if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			closesocket(thread_argv->player_socket);
			return 5;
		}
		sprintf(SendStr, "%s", SERVER_APPROVED);
		decode_message(SendStr, &message, "sent to");
		if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
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
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int server_state(thread_service_arg* thread_argv) {
	char SendStr[SEND_STR_SIZE], * recv = NULL, * input_file_str = NULL, file_name[SEND_STR_SIZE] = { 0 };
	int  state = 0, number = 0;
	Message message;
	char* AcceptedStr = NULL;
	while (1) {
		switch (state) {
		case 0:
			if ((state = client_req_server_state(thread_argv)) == ERROR_CODE)
				return ERROR_CODE;
			break;
		case 1: {
			if ((state = server_main_menu(thread_argv, &number)) == ERROR_CODE)
				return ERROR_CODE;
		}
			  break;
		case 2:

			if (SendString("GAME_STARTED\n", thread_argv->player_socket) == TRNS_FAILED) {
				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			strcpy(player_move[thread_argv->player_index - 1], "");
			decode_message("GAME_STARTED", &message, "sent");
			free_message(&message);
			if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			state = 3;
			break;
		case 3:
			if ((state = game_on_state(thread_argv, &number)) == ERROR_CODE)
				return ERROR_CODE;
			break;
		case 4:
			sprintf(SendStr, "%s:%s\n", GAME_ENDED, name_player[win - 1]);
			if (SendString(SendStr, thread_argv->player_socket) == TRNS_FAILED) {
				closesocket(thread_argv->player_socket);
				return server_opponent_quit(thread_argv);
			}
			decode_message(SendStr, &message, "sent to");
			free_message(&message);
			if (write_to_file(thread_argv->file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			state = 1;
			break;
		case 5:
			sprintf(SendStr, "thread_log_%s.txt", thread_argv->player_name);
			printf("Player disconnected.Exiting.\n");
			if (write_to_file(thread_argv->file_name, "Player disconnected.Exiting.\n") != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return 5;
		default:
			break;
		}
	}
	return 0;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
static DWORD ServiceThread(thread_service_arg* thread_argv)
{
	char* recv = NULL, * input_file_str = NULL, file_name[1000] = { 0 };
	int  state = 0, number = 0, error_code = 0;
	BOOL Done = FALSE;
	char* AcceptedStr = NULL;
	while ((error_code == 0)) {
		error_code = server_state(thread_argv);
		if ((error_code == ERROR_CODE))
			return ERROR_CODE;
	}
	DeleteFileA(thread_argv->file_name);
	closesocket(thread_argv->player_socket);
	//	_CrtDumpMemoryLeaks();
	return 0;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int main(int argc, char* argv[]) {
	int exitcode = -1;
	if (argc != 2)
		exit(exitcode);
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


