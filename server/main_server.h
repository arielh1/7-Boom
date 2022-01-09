
#ifndef MAIN_SERVER_H
#define MAIN_SERVER_H

typedef struct thread_service_arg {
	char player_name[MAX_LEN_NAME];
	int player_index;
	int player_number;
	SOCKET player_socket;
	char file_name[MAX_LEN_NAME];
}thread_service_arg;

void MainServer(int port);
/// <summary>
/// 
/// </summary>
/// <returns></returns>
static int FindFirstUnusedThreadSlot();
/// <summary>
/// 
/// </summary>
static void CleanupWorkerThreads();
/// <summary>
/// 
/// </summary>
/// <param name="thread_argv"></param>
/// <returns></returns>
static DWORD ServiceThread(thread_service_arg* thread_argv);

#endif


