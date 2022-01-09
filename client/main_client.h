#ifndef MAIN_CLIENT_H
#define MAIN_CLIENT_H
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "../shared/helper_function.h"
#include "state_machine_client.h"


//global paramter
SOCKET m_socket;
char name_file[FILE_NAME_SIZE];


/// <summary>
/// 
/// </summary>
/// <param name="sock"></param>
/// <param name="timeout"></param>
/// <returns></returns>
int set_timeout(SOCKET sock, DWORD timeout);
/// <summary>
/// 
/// </summary>
/// <param name="SendStr"></param>
/// <param name="SendRes"></param>
/// <param name="argv"></param>
/// <param name="message"></param>
/// <param name="file_name"></param>
/// <returns></returns>
int play_or_quit(char* SendStr, TransferResult_t SendRes, char* argv[], Message message, char* file_name);
/// <summary>
/// 
/// </summary>
/// <param name="RecvRes"></param>
/// <returns></returns>
int check_failed_disconnected(TransferResult_t RecvRes);
/// <summary>
/// 
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
int is_digit(char* str);
/// <summary>
/// 
/// </summary>
/// <param name="seconds"></param>
/// <returns></returns>
int timeout(int seconds);
/// <summary>
/// 
/// </summary>
/// <param name="port"></param>
/// <param name="argv"></param>
/// <returns></returns>
int MainClient(int port, char* argv[]);
/// <summary>
/// 
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[]);


#endif
