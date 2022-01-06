
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "game.h"
int is_seven_boom(int number) {
	return (number % 7 == 0);
}
int game_start() {//TODO ERROR
	semaphore_clinet_connect = CreateSemaphore(0, 0,1, NULL);

	 semaphore_client_1_turn=CreateSemaphore(0, 1, 1, NULL);
	 semaphore_client_2_turn= CreateSemaphore(0, 0, 1, NULL);
	return 0;
	
}
int game_step() {
	
	return 0;
}