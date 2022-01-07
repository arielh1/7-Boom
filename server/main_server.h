
#ifndef MAIN_SERVER_H
#define MAIN_SERVER_H


void MainServer(int port);
typedef struct thread_service_arg {
	char player_name[MAX_LEN_NAME];
	int player_index;
	SOCKET player_socket;
}thread_service_arg;
#endif