
#ifndef MAIN_SERVER_H
#define MAIN_SERVER_H
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "../shared/helper_function.h"





#include "game.h"

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
static DWORD Exit_Thread(void);


int rec_failed_disconnected(TransferResult_t RecvRes, thread_service_arg* thread_argv);

#endif


