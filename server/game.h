#ifndef GAME_H
#define GAME_H
HANDLE semaphore_clinet_connect;
HANDLE semaphore_client_1_turn;
HANDLE semaphore_client_2_turn;
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