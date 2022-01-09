



#include "game.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int is_seven_boom(int number) {
	return (number % 7 == 0);
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int game_start() {//TODO ERROR
	//semaphore_clinet_connect = CreateSemaphore(0, 2,2, NULL);

	semaphore_client_1_turn = CreateSemaphore(0, 1, 1, NULL); 
	 semaphore_client_2_turn= CreateSemaphore(0, 0, 1, NULL);
	return 0;
	
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int seven_appeared(char* snum) {


	return (strstr(snum, "7") != NULL);

}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
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
