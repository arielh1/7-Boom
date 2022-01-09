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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

SOCKET m_socket;
char name_file[FILE_NAME_SIZE];
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Reading data coming from the server
int set_timeout(SOCKET sock, DWORD timeout) {
	// set sock options
	if (SUCCESS_CODE != setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(DWORD))) {
		printf("failed to set sockopt\n");
		return ERROR_CODE;
	}
	if (SUCCESS_CODE != setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(DWORD))) {
		printf("failed to set sockopt\n");
		return ERROR_CODE;
	}
	return 0;
}


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int play_or_quit(char* SendStr, TransferResult_t SendRes, char* argv[],Message message,char *file_name) {
	do {
		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard
		if (STRINGS_ARE_EQUAL(SendStr, "1"))
		{
			sprintf(SendStr, "%s:%s", CLIENT_VERSUS,argv[3]);
			SendRes = SendString(SendStr, m_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				free(recv);
				return 0x555;
			}
			decode_message(CLIENT_VERSUS, &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return 2;
			
		}
		else if (STRINGS_ARE_EQUAL(SendStr, "2"))
		{
			
			free(recv);
			return 4;
		}
		printf("this messase is not good please type again\n");
	} while (1);
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int check_failed_disconnected(TransferResult_t RecvRes) {
	if (RecvRes == TRNS_FAILED)
	{
		if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		printf("Server disconnected. Exiting.\n");
		return ( ERROR_CODE);
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		printf("Server disconnected. Exiting.\n");
		return ERROR_CODE;
	}
	else
		return 0;

}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int is_digit(char* str) {

	for (unsigned int index = 0; index < strlen(str); index++) {
		if (!isdigit(str[index]))
		{
			return 0;
		}
	}
	return 1;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int timeout(int seconds) {

	clock_t endwait;
	endwait = clock() + seconds * CLOCKS_PER_SEC;
	while (clock() < endwait) {}

	return  1;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state0( char* argv[], char file_name[MAX_LINE]) {
	char* recv;
	char SendStr[MAX_LINE];
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	Message message;
	recv = NULL;
	
		sprintf(SendStr, "%s:%s", CLIENT_REQUEST, argv[3]);
		sprintf(file_name, "client_log_%s.txt", argv[3]);
		SendRes = SendString(SendStr, m_socket);
		if (SendRes == TRNS_FAILED)
		{
			printf("Server disconnected. Exiting.\n");
			if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return ERROR_CODE;
		}
		
		decode_message(SendStr, &message, "sent");
		free_message(&message);
	
		if (strstr(message.message_type, CLIENT_REQUEST)) {
			sprintf(file_name, "client_log_%s.txt", argv[3]);
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				free_message(&message);
				free(recv);
				return ERROR_CODE;
			}
		}
		RecvRes = ReceiveString(&recv, m_socket);
		if (check_failed_disconnected(RecvRes) == ERROR_CODE) {
			free(recv);
			return ERROR_CODE;
		}
	

		if (strstr(recv, SERVER_APPROVED)) {
			decode_message(recv, &message, "received");
			free(recv);
			free_message(&message);
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		
			
			return 1;
			
		}
	
		if (strstr(recv, SERVER_DENIED)) {
			decode_message(recv, &message, "received");
			printf("Server on %s:%s  denied the connection request.\n", argv[1], argv[2]);
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				free_message(&message);
				free(recv);
				return ERROR_CODE;
			}
			free_message(&message);
			free(recv);
			return 5;
		
		}
	
	return ERROR_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state1(char SendStr[MAX_LINE], char* argv[], char file_name[MAX_LINE]) {
	{
		char* recv;
		char SendStr[MAX_LINE];
		TransferResult_t SendRes;
		TransferResult_t RecvRes;
		Message message;
		recv = NULL;
		
		RecvRes = ReceiveString(&recv, m_socket);
		if (check_failed_disconnected(RecvRes) == ERROR_CODE)
			return ERROR_CODE;

			if (strstr(recv, SERVER_MAIN_MENU)) {
				decode_message(recv, &message, "received");
				
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					free_message(&message);
					free(recv);
					return ERROR_CODE;
				}
				free_message(&message);
				free(recv);
				printf("%s", CLIENT_CHOOSE_P_Q);
				do {
					gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard
					if (STRINGS_ARE_EQUAL(SendStr, "1"))
					{
						sprintf(SendStr, "%s:%s", CLIENT_VERSUS, argv[3]);
						SendRes = SendString(SendStr, m_socket);
						if (SendRes == TRNS_FAILED)
						{
						
							if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
								printf(WRITE_TO_FILE_ERROR_MESSAGE);
								
								return ERROR_CODE;
							}

							printf("Server disconnected. Exiting.\n");
							return ERROR_CODE;
						}
						decode_message(SendStr, &message, "sent");
						free_message(&message);
						if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
							printf(WRITE_TO_FILE_ERROR_MESSAGE);
							return ERROR_CODE;
						}
		
						
						return 2;
					}
					if (STRINGS_ARE_EQUAL(SendStr, "2"))
					{
		
				
						return 4;
					}
					printf("Error: illegal command:\n");
					if (write_to_file(file_name, "Error: illegal command:\n") != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
				} while (1);
			}
			
	}
	return ERROR_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state2(char SendStr[MAX_LINE], char* argv[], char file_name[MAX_LINE]) {
	char* recv;
	TransferResult_t RecvRes;
	Message message;
	
	recv = NULL;
	RecvRes = ReceiveString(&recv, m_socket);
	if (check_failed_disconnected(RecvRes) == 0)
	{
		if (strstr(recv, SERVER_NO_OPPONENTS)) {
			//Sleep(RESPOND_TIME);
			decode_message(recv, &message, "received");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				free_message(&message);
				free(recv);
				return ERROR_CODE;
			}
			free_message(&message);
			free(recv);
			return 1;

		}
		if (strstr(recv, GAME_STARTED)) {
			printf("Game is on!\n");
			decode_message(recv, &message, "received");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}

	
			free(recv);

			return 3;
		}
	}
	
	return ERROR_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state3(char SendStr[MAX_LINE], char* argv[], char file_name[MAX_LINE]) {

	char* recv=NULL;
	char  input_client[MAX_LINE];
	TransferResult_t RecvRes;
	Message message;
	recv = NULL;
	while (1)
	{
		recv = NULL;
		RecvRes = ReceiveString(&recv, m_socket);
		if (check_failed_disconnected(RecvRes) == ERROR_CODE){
			return ERROR_CODE;
		}
		if (strstr(recv, SERVER_MOVE_REQUEST)) {
			decode_message(recv, &message, "received");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			do {
				printf("Enter the next number or boom: !\n");
				gets_s(input_client, sizeof(input_client)); //Reading a string from the keyboard

				if ((strcmp(input_client, "boom") != 0) && is_digit(input_client) == 0) {
					printf("Error: illegal command:\n");
					if (write_to_file(file_name, "Error: illegal command:\n") != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
				}
				else
					break;
			} while (1);

			sprintf(SendStr, "%s:%s\n", CLIENT_PLAYER_MOVE, input_client);


			if (SendString(SendStr, m_socket) == TRNS_FAILED)
			{
				if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				printf("Server disconnected. Exiting.\n");
				return ERROR_CODE;
			}
			decode_message(SendStr, &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		
		}
		if (strstr(recv, GAME_VIEW)) {

			decode_message(recv, &message, "received");
			printf("%s move was %s\n%s\n", message.param[0], message.param[1], message.param[2]);
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		}
		if (strstr(recv, TURN_SWITCH)) {
			decode_message(recv, &message, "received");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			(STRINGS_ARE_EQUAL(message.param[0], argv[3]) == TRUE) ? printf("your turn !\n") : printf("%s turn!\n", message.param[0]);
		}
		if (strstr(recv, GAME_ENDED)) {

			decode_message(recv, &message, "received");
			printf("%s won!\n", message.param[0]);
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return 1;

		//	break;
		}
		if (strstr(recv, SERVER_OPPONENT_QUIT)) {
			printf("Opponent quit .\n");
			decode_message(recv, &message, "received");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return 1;
		}
	}
	return ERROR_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state4(char SendStr[MAX_LINE], char* argv[], char file_name[MAX_LINE]) {

	TransferResult_t SendRes;
	Message message;
	sprintf(SendStr, "%s\n", CLIENT_DISCONNECT);
	SendRes = SendString(SendStr, m_socket);
	if (SendRes == TRNS_FAILED)
	{
		if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		printf("Server disconnected. Exiting.\n");
		return ERROR_CODE;
	}
	decode_message(SendStr, &message, "sent");
	if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	return 0;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state5(SOCKADDR_IN clientService, char *ip, int port,char SendStr[MAX_LINE], char* argv[]) {

	while ((connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)) {
		{
			printf(CLIENT_CHOOSE_T_E, ip, argv[2]);
			gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard
			if (STRINGS_ARE_EQUAL(SendStr, "1"))
			{
				m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (m_socket == INVALID_SOCKET) {
					printf("Error at socket(): %ld\n", WSAGetLastError());
					WSACleanup();
					return ERROR_CODE;
				}
				clientService.sin_family = AF_INET;
				clientService.sin_addr.s_addr = inet_addr(ip); //Setting the IP address to connect to
				clientService.sin_port = htons(port); //Setting the port to connect to.
				set_timeout(m_socket, (DWORD)RESPOND_TIME);
				continue;
			}
			if (STRINGS_ARE_EQUAL(SendStr, "2"))
			{
				return 4;
			}
			printf("Error: illegal command:\n");
			//WSACleanup();
		}

	}
	
	return 0;


}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state_machine(SOCKADDR_IN clientService, char* ip, int port, char* argv[], char file_name[MAX_LINE]) {
	char SendStr[2560], * recv = NULL;
	int state = 5;
	int i = 1;

	while (i) {
		switch (state)
		{
		case 0:
			
			state = state0( argv, file_name);
			
			break;
		case 1:
		
			state = state1(SendStr, argv, file_name);
		
			break;
		case 2:
			state = state2(SendStr, argv, file_name);
			
			break;
		case 3:
			state = state3(SendStr, argv, file_name);
			
			break;
		case 4:
			state = state4(SendStr, argv, file_name);
			i = 0;
			break;
		case 5:
			state = state5(clientService, ip, port, SendStr, argv);
			if (state == 0)
				printf("Connected to server on %s:%s\n", argv[1], argv[2]);
			break;
		case ERROR_CODE:
			return ERROR_CODE;

		default:

			break;
		}
	}
	return 0;
		
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int MainClient(char *ip,int port,char *argv[])
{
	int code_exit;
	char file_name[FILE_NAME_SIZE] = { 0 };
	SOCKADDR_IN clientService;
	WSADATA wsaData; //Create a WSADATA object called wsaData.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return ERROR_CODE;
	}
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip); //Setting the IP address to connect to
	clientService.sin_port = htons(port); //Setting the port to connect to.
	set_timeout(m_socket, (DWORD)RESPOND_TIME);
	code_exit = state_machine(clientService, ip, port, argv, file_name) == ERROR_CODE;
	DeleteFileA(file_name);
	WSACleanup();
	closesocket(m_socket);
	_CrtDumpMemoryLeaks();
	return code_exit;
}


int main(int argc, char* argv[]) {
	int exitcode = -1;
	sprintf(name_file, "client_log_%s.txt", argv[3]);
	return MainClient(argv[1], atoi(argv[2]), argv);
	
}



