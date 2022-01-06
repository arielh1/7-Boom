#ifndef GAME_H
#define GAME_H
HANDLE semaphore_clinet_connect;
HANDLE semaphore_client_1_turn;
HANDLE semaphore_client_2_turn;
int game_start();
int game_step();
#endif