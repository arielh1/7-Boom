#ifndef GAME_H
#define GAME_H

//#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#define NUM_OF_WORKER_THREADS 3
#define SEND_STR_SIZE 350
HANDLE semaphore_clinet_connect;
HANDLE semaphore_client_1_turn;
HANDLE semaphore_client_2_turn;
HANDLE semaphore_wait;
HANDLE semaphore_write;
HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
SOCKET ThreadInputs[NUM_OF_WORKER_THREADS];
HANDLE semaphore_wait;



/// <summary>
/// 
/// </summary>
/// <returns></returns>
int game_start();
/// <summary>
/// check if number is a double of 7
/// </summary>
/// <param name="number">input from clien</param>
/// <returns></returns>
int is_seven_boom(int number);
/// <summary>
/// check if number containes of 7
/// </summary>
/// <param name="snum">input from client</param>
/// <returns></returns>
int seven_appeared(char* snum);
#endif