#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "state_machine_client.h"
#include "main_client.h"

int state0(char* argv[]) {
	char* recv = NULL;
	char SendStr[MAX_LINE];
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	Message message;
	sprintf(SendStr, "%s:%s", CLIENT_REQUEST, argv[3]);
	sprintf(name_file, "client_log_%s.txt", argv[3]);
	SendRes = SendString(SendStr, m_socket);
	if (SendRes == TRNS_FAILED) {
		printf("Server disconnected. Exiting.\n");
		if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		return ERROR_CODE;
	}
	decode_message(SendStr, &message, SENT_TO_SERVER);
	free_message(&message);
	if (strstr(message.message_type, CLIENT_REQUEST)) {
		sprintf(name_file, "client_log_%s.txt", argv[3]);
		if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
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
		decode_message(recv, &message, RECIVED_FROM_SERVER);
		free(recv);
		free_message(&message);
		if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}


		return 1;

	}
	if (strstr(recv, SERVER_DENIED)) {
		decode_message(recv, &message, "received from");
		printf("Server on %s:%s  denied the connection request.\n", argv[1], argv[2]);
		if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
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
int state1(char* argv[]) {
	char* recv = NULL;
	char SendStr[MAX_LINE];
	TransferResult_t SendRes;
	TransferResult_t RecvRes;
	Message message;
	RecvRes = ReceiveString(&recv, m_socket);
	if (check_failed_disconnected(RecvRes) == ERROR_CODE)
		return ERROR_CODE;
	if (strstr(recv, SERVER_MAIN_MENU)) {
		decode_message(recv, &message, RECIVED_FROM_SERVER);
		if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			free_message(&message);
			free(recv);
			return ERROR_CODE;
		}
		free_message(&message);
		free(recv);
		printf("%s", CLIENT_CHOOSE_P_Q);
		do {
			gets_s(SendStr, sizeof(SendStr)); 
			if (STRINGS_ARE_EQUAL(SendStr, "1")) {
				sprintf(SendStr, "%s:%s", CLIENT_VERSUS, argv[3]);
				SendRes = SendString(SendStr, m_socket);
				if (SendRes == TRNS_FAILED) {
					if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
					printf("Server disconnected. Exiting.\n");
					return ERROR_CODE;
				}
				decode_message(SendStr, &message, SENT_TO_SERVER);
				free_message(&message);
				if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				return 2;
			}
			if (STRINGS_ARE_EQUAL(SendStr, "2")) {
				return 4;
			}
			printf("Error: illegal command:\n");
			if (write_to_file(name_file, "Error: illegal command:\n") != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		} while (1);
	}
	return ERROR_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state2(char* argv[]) {
	char* recv = NULL;
	TransferResult_t RecvRes;
	Message message;
	set_timeout(m_socket, RESPOND_TIME*2);
	RecvRes = ReceiveString(&recv, m_socket);
	if (check_failed_disconnected(RecvRes) == 0)
	{
		if (strstr(recv, SERVER_NO_OPPONENTS)) {
			decode_message(recv, &message, "received from");
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				free_message(&message);
				free(recv);
				return ERROR_CODE;
			}
			free_message(&message);
			free(recv);
			set_timeout(m_socket, RESPOND_TIME );
			return 1;

		}
		if (strstr(recv, GAME_STARTED)) {
			printf("Game is on!\n");
			decode_message(recv, &message, "received from");
			free_message(&message);
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			free(recv);
			set_timeout(m_socket, RESPOND_TIME);
			return 3;
		}
	}
	return ERROR_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state3(char* argv[]) {
	char* recv = NULL;
	char  input_client[MAX_LINE], SendStr[MAX_LINE];
	TransferResult_t RecvRes;
	Message message;
	while (1) {
		RecvRes = ReceiveString(&recv, m_socket);
		if (check_failed_disconnected(RecvRes) == ERROR_CODE) {
			free(recv);
			return ERROR_CODE;
		}
		if (recv && strstr(recv, SERVER_MOVE_REQUEST) && recv) {
			decode_message(recv, &message, "received from");
			free(recv);
			recv = NULL;
			free_message(&message);
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			do {
				printf("Enter the next number or boom: !\n");
				gets_s(input_client, sizeof(input_client));

				if ((strcmp(input_client, "boom") != 0) && is_digit(input_client) == 0) {
					printf("Error: illegal command:\n");
					if (write_to_file(name_file, "Error: illegal command:\n") != SUCCESS_CODE) {
						printf(WRITE_TO_FILE_ERROR_MESSAGE);
						return ERROR_CODE;
					}
				}
				else
					break;
			} while (1);
			sprintf(SendStr, "%s:%s\n", CLIENT_PLAYER_MOVE, input_client);
			if (SendString(SendStr, m_socket) == TRNS_FAILED) {
				if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
					printf(WRITE_TO_FILE_ERROR_MESSAGE);
					return ERROR_CODE;
				}
				printf("Server disconnected. Exiting.\n");
				return ERROR_CODE;
			}
			decode_message(SendStr, &message, "sent to");
			free_message(&message);
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}

		}
		if (recv && strstr(recv, GAME_VIEW)) {
			decode_message(recv, &message, "received from");
			printf("%s move was %s\n%s\n", message.param[0], message.param[1], message.param[2]);
			free_message(&message);
			free(recv);
			recv = NULL;
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
		}
		if (recv && strstr(recv, TURN_SWITCH)) {
			decode_message(recv, &message, "received from");
			free(recv);
			recv = NULL;
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			(STRINGS_ARE_EQUAL(message.param[0], argv[3]) == TRUE) ? printf("your turn !\n") : printf("%s turn!\n", message.param[0]);
			free_message(&message);
		}
		if (recv && strstr(recv, GAME_ENDED)) {
			decode_message(recv, &message, "received from");
			printf("%s won!\n", message.param[0]);
			free_message(&message);
			free(recv);
			recv = NULL;
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return 1;
		}
		if (recv && strstr(recv, SERVER_OPPONENT_QUIT)) {
			printf("Opponent quit .\n");
			decode_message(recv, &message, "received from");
			free(recv);
			recv = NULL;
			free_message(&message);
			if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
				printf(WRITE_TO_FILE_ERROR_MESSAGE);
				return ERROR_CODE;
			}
			return 1;
		}
	}
	return ERROR_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state4() {
	char SendStr[MAX_LINE];
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
	decode_message(SendStr, &message, "sent to");
	if (write_to_file(name_file, message.log_file_format) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	free_message(&message);

	return 0;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int state5(SOCKADDR_IN clientService, char* argv[]) {

	while ((connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)) {
		{
			char SendStr[MAX_LINE];
			printf(CLIENT_CHOOSE_T_E, argv[1], argv[2]);
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
				clientService.sin_addr.s_addr = inet_addr(argv[1]); //Setting the IP address to connect to
				clientService.sin_port = htons(atoi(argv[2])); //Setting the port to connect to.
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
	char* recv = NULL;
	int state = 5;
	int i = 1;
	while (i) {
		switch (state) {
		case 0:
			state = state0(argv);
			break;
		case 1:
			state = state1(argv);
			break;
		case 2:
			state = state2(argv);
			break;
		case 3:
			state = state3(argv);
			break;
		case 4:
			state = state4();
			i = 0;
			break;
		case 5:
			state = state5(clientService, argv);
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