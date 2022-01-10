#include "state_machine_server.h"
#include "game.h
int seven_boom1(thread_service_arg* thread_argv, int number, Message* message) {
	char snum[50];
	char* AcceptedStr = NULL;
	TransferResult_t RecvRes;
	int state = 3;
	RecvRes = ReceiveString(&AcceptedStr, thread_argv->player_socket);
	if (RecvRes == TRNS_FAILED) {
		printf("Service socket error while reading, closing thread.\n");
		closesocket(thread_argv->player_socket);
		return ERROR_CODE;
	}
	else if (RecvRes == TRNS_DISCONNECTED) {
		printf("Connection closed while reading, closing thread.\n");
		closesocket(thread_argv->player_socket);
		return ERROR_CODE;
	}
	decode_message(AcceptedStr, message, "revice");
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