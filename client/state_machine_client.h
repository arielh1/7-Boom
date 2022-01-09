#ifndef STATE_MACHINE_CLIENT
#define STATE_MACHINE_CLIENT
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "../shared/helper_function.h"

/// <summary>
/// 
/// </summary>
/// <param name="argv"></param>
/// <returns></returns>
int state0(char* argv[]);
/// <summary>
/// 
/// </summary>
/// <param name="argv"></param>
/// <returns></returns>
int state1(char* argv[]);
/// <summary>
/// 
/// </summary>
/// <param name="argv"></param>
/// <returns></returns>
int state2(char* argv[]);
/// <summary>
/// 
/// </summary>
/// <param name="argv"></param>
/// <returns></returns>
int state3(char* argv[]);
/// <summary>
/// 
/// </summary>
/// <returns></returns>
int state4();
/// <summary>
/// 
/// </summary>
/// <param name="clientService"></param>
/// <param name="argv"></param>
/// <returns></returns>
int state5(SOCKADDR_IN clientService, char* argv[]);
/// <summary>
/// 
/// </summary>
/// <param name="clientService"></param>
/// <param name="ip"></param>
/// <param name="port"></param>
/// <param name="argv"></param>
/// <param name="file_name"></param>
/// <returns></returns>
int state_machine(SOCKADDR_IN clientService, char* ip, int port, char* argv[], char file_name[MAX_LINE]);

#endif