/*
ariel hacohen 313325938
ofir atiya 311142541
ex4
server gets an arg in this format <server.exe> <port>
*/
#ifndef MAIN_SERVER_H
#define MAIN_SERVER_H
#include "../Shared/helper_function.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#include "game.h"
#include "../Shared/SocketSendRecvTools.h"
#define NUM_OF_WORKER_THREADS 3
#define SEND_STR_SIZE 350
SOCKET array_socket[4];
typedef struct thread_service_arg {
	char player_name[MAX_LEN_NAME];
	int player_index;
	int player_number;
	SOCKET player_socket;
	char file_name[MAX_LEN_NAME];
}thread_service_arg;

int server_run = 1;
int number_of_player = 0;
int game_on = 1;
int win;
char name_player[2][MAX_LEN_NAME] = { "","" };
char player_move[2][MAX_LEN_NAME] = { 0,0 };
int player_played;
int  flag_opponent_quit = 0;



/// <summary>
/// check if recvres reecived an error
/// </summary>
/// <param name="RecvRes">enum which hold states of transacrions</param>
/// <returns></returns>
int set_timeout(SOCKET sock, DWORD timeout);
/// <summary>
///  exit thread if recieve exit from user
/// </summary>
/// <returns></returns>
static DWORD Exit_Thread(void);
/// <summary>
/// create sockets, threads, semaphore and perform cleanup in the end
/// </summary>
/// <param name="port">port num</param>
void MainServer(int port);
/// <summary>
/// find the first unused thread slot
/// </summary>
/// <returns></returns>
static int FindFirstUnusedThreadSlot();
/// <summary>
/// clean handels and sockets
/// </summary>
static void CleanupWorkerThreads();
/// <summary>
///  recieve mesaage from client and decieds what will be next state according to game rules
/// </summary>
/// <param name="thread_argv"> the thread in charge of client</param>
/// <param name="number"> number from client</param>
/// <param name="message"> message from client</param>
/// <returns> retuns next state </returns>
int seven_boom(thread_service_arg* thread_argv, int number, Message* message);

/// <summary>
/// return next state according to the client input
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <returns></returns>
int game_view(thread_service_arg* thread_argv);

/// <summary>
/// send a message to client if opponent has quited
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <returns></returns>
int server_opponent_quit(thread_service_arg* thread_argv);

/// <summary>
/// recieve next state from seven_boom and send messages to client accordingly
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <param name="number"> number from client</param>
/// <returns>returns next state</returns>
int game_run_one_turn(thread_service_arg* thread_argv, int* number);

/// <summary>
/// start and end game accordibng to the client
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <param name="number"> number from client</param>
/// <returns> returns next state </returns>
int game_on_state(thread_service_arg* thread_argv, int* number);

/// <summary>
/// send to client request to start the game and recieve client choise
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <param name="number">number from client</param>
/// <returns> returns next state </returns>
int server_main_menu(thread_service_arg* thread_argv, int* number);

/// <summary>
/// 
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <returns></returns>
int client_req_server_state(thread_service_arg* thread_argv);

/// <summary>
/// manage connections with client
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <returns> return 0 if no errors</returns>
int server_state(thread_service_arg* thread_argv);

/// <summary>
/// manage thread
/// </summary>
/// <param name="thread_argv">the thread in charge of client</param>
/// <returns>returns 0 if no errors</returns>
static DWORD ServiceThread(thread_service_arg* thread_argv);

/// <summary>
/// 
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[]);
#endif


