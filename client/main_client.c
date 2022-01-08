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
		printf("Socket error while trying to write data to socket\n");
		exit( ERROR_CODE);
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("Server closed connection. Bye!\n");
		exit(ERROR_CODE);
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

int MainClient(char *ip,int port,char *argv[])
{
	char SendStr[2560], * recv = NULL,  input_client[500], file_name[1000] = { 0 };
//	char client_name[100];
//	int input_is_valid=0;
	SOCKADDR_IN clientService;
//	HANDLE hThread[2];
	int state = 5;
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
		return ERROR_CODE;
	}

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip); //Setting the IP address to connect to
	clientService.sin_port = htons(port); //Setting the port to connect to.

	

	
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	Message message;
	int i = 1;
	while (i) {
		switch (state)
		{
		case 0: 
		{
			sprintf(SendStr, "%s:%s", CLIENT_REQUEST, argv[3]);
			SendRes = SendString(SendStr, m_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return ERROR_CODE;
			}
			sprintf(file_name, "client_log_%s.txt", argv[3]);
			decode_message(CLIENT_REQUEST, &message, "sent");
			if (strstr(message.message_type, CLIENT_REQUEST)) {
		//		strcpy(client_name, message.param[0]);
				sprintf(file_name, "client_log_%s.txt", argv[3]);
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
			}
			RecvRes = ReceiveString(&recv, m_socket);
			if (RecvRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return ERROR_CODE;
			}
			else if (RecvRes == TRNS_DISCONNECTED)
			{
				printf("Server closed connection. Bye!\n");
				return ERROR_CODE;
			}
			else
			{
				printf("this message from server is:%s", recv);
			}
			if (strstr(recv, SERVER_APPROVED)) {
				decode_message("SERVER_APPROVED", &message, "received");
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				state = 1;
				free(recv);
				break;
			}

			if (strstr(recv, SERVER_DENIED)) {
				decode_message(SERVER_DENIED, &message, "received");
				if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				state = 5;
				free(recv);
				break;
			}
		}
			break;
		case 1:
		{
			recv = NULL;

			RecvRes = ReceiveString(&recv, m_socket);
			if (RecvRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return 0x555;
			}
			else if (RecvRes == TRNS_DISCONNECTED)
			{
				printf("Server closed connection. Bye!\n");
			
				return ERROR_CODE;
			}
			else
			{
				printf("this message from server is:%s", recv);
				if (strstr(recv, SERVER_MAIN_MENU)) {
					decode_message("SERVER_MAIN_MENU", &message, "received");
					if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
					printf("%s",CLIENT_CHOOSE_P_Q);
				do{
					gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard
					if (STRINGS_ARE_EQUAL(SendStr, "1"))
					{
						sprintf(SendStr, "%s:%s", CLIENT_VERSUS, argv[3]);
						SendRes = SendString(SendStr, m_socket);
						if (SendRes == TRNS_FAILED)
						{
							printf("Socket error while trying to write data to socket\n");
							return 0x555;
						}
						decode_message(CLIENT_VERSUS, &message, "sent");
						if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
							printf(WRITE_TO_FILE_ERROR_MESSAGE);
							return ERROR_CODE;
						}
						state = 2;
						free(recv);
						break;
					}
					if (STRINGS_ARE_EQUAL(SendStr, "2"))
					{
						state = 4;
						free(recv);
						break;
					}
					printf("Error: illegal command:\n");
					if (write_to_file(file_name, "Error: illegal command:\n") != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
					printf("please type again\n");

				} while (1);
				
					break;
				}
			}
		}
			break;
		case 2: {
			recv = NULL;
			RecvRes = ReceiveString(&recv, m_socket);
			if (timeout(30) == 1) {
				printf("Time Out\n");
			//	return 0;
			}

			if(check_failed_disconnected(RecvRes)==0)
			{
				printf("this message from server is:%s\n", recv);

				if (strstr(recv, GAME_STARTED)) {
					decode_message("GAME_STARTED", &message, "received");
					if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}

					state = 3;
					free(recv);
					break;
				}
			}
		}
			break;
		case 3:
			
			while (1)
			{
				recv = NULL;
				RecvRes = ReceiveString(&recv, m_socket);

				if (check_failed_disconnected(RecvRes) != 0)
				{
					return 0x555;
				}
				printf("this message from server is:%s\n", recv);
				if (strstr(recv, SERVER_MOVE_REQUEST)) {
					decode_message("SERVER_MOVE_REQUEST", &message, "received");
					if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
						do{
						printf("your turn\n");
						gets_s(input_client, sizeof(input_client)); //Reading a string from the keyboard
	
						if ((strcmp(input_client, "boom") != 0) && is_digit(input_client) == 0) {
							printf("Error: illegal command:\n");
							if (write_to_file(file_name,"Error: illegal command:\n") != SUCCESS_CODE) {
								printf(WRITE_TO_FILE_ERROR_MESSAGE);
								return ERROR_CODE;
							}
						}
						else
							break;
						} while (1);
					
						sprintf(SendStr,"%s:%s\n", CLIENT_PLAYER_MOVE, input_client);
						

						if (SendString(SendStr, m_socket) == TRNS_FAILED)
						{
							printf("Socket error while trying to write data to socket\n");
							return 0x555;
						}
						decode_message(CLIENT_PLAYER_MOVE, &message, "sent");
						if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
							printf(WRITE_TO_FILE_ERROR_MESSAGE);
							return ERROR_CODE;
						}
						//break;
					}
				if (strstr(recv, GAME_ENDED)) {
					decode_message("GAME_ENDED", &message, "received");
					if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
						state = 1;

						break;
					}
				
				}

			
			break;
		case 4:
			sprintf(SendStr, "%s:%s", CLIENT_DISCONNECT, argv[3]);
			SendRes = SendString(SendStr, m_socket);
			if (SendRes == TRNS_FAILED)
			{
				printf("Socket error while trying to write data to socket\n");
				return ERROR_CODE;
			}
			decode_message(CLIENT_DISCONNECT, &message, "sent");
			if (write_to_file(file_name, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			i = 0;
			break;
		case 5:
		{
			while ((connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)) {
				{
					printf(CLIENT_CHOOSE_T_E, ip, argv[2]);
					gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard
					if (STRINGS_ARE_EQUAL(SendStr, "1"))
					{
						continue;
					}
					if (STRINGS_ARE_EQUAL(SendStr, "2"))
					{
						return 0;
					}
					printf("this messase not good pls type again\n");
					//WSACleanup();
				}

			}
			state = 0;
			printf("connect.\n");
			break;
		}
		default:
			break;
		}
	}
	//WaitForMultipleObjects(2, hThread, FALSE, INFINITE);

	//TerminateThread(hThread[0], 0x555);
	//TerminateThread(hThread[1], 0x555);

	//CloseHandle(hThread[0]);
	//CloseHandle(hThread[1]);

	closesocket(m_socket);
	DeleteFileA(file_name);
	WSACleanup();

	return 0;
}






int main(int argc, char* argv[]) {
	int exitcode = -1;
	
	return MainClient(argv[1], atoi(argv[2]), argv);
	
}



