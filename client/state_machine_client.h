#ifndef STATE_MACHINE_CLIENT
#define STATE_MACHINE_CLIENT
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include "../shared/helper_function.h"

/// <summary>
/// connecting to server
/// </summary>
/// <param name="argv"><arguments/param>
/// <returns> returns next state </returns>
int state0(char* argv[]);
/// <summary>
/// let the client chhose if to play or quit
/// </summary>
/// <param name="argv">arguments</param>
/// <returns> returns next state </returns>
int state1(char* argv[]);
/// <summary>
///  starting game
/// </summary>
/// <param name="argv">arguments</param>
/// <returns> returns next state </returns>
int state2(char* argv[]);
/// <summary>
/// switch turns between opponents and lets client know whos the winner
/// </summary>
/// <param name="argv">arguments</param>
/// <returns> returns next state </returns>
int state3(char* argv[]);
/// <summary>
///  disconnection
/// </summary>
/// <returns></returns>
int state4();
/// <summary>
/// when connection failes lets the client chhose if to reconnect
/// </summary>
/// <param name="clientService">client socket</param>
/// <param name="argv">arguments</param>
/// <returns> returns 0 if no errors </returns>
int state5(SOCKADDR_IN clientService, char* argv[]);
/// <summary>
/// manage client set of actions and messages
/// </summary>
/// <param name="clientService">client socket</param>
/// <param name="ip">client ip</param>
/// <param name="port">client port</param>
/// <param name="argv">arguments</param>
/// <param name="file_name">name of log file</param>
/// <returns></returns>
int state_machine(SOCKADDR_IN clientService, char* ip, int port, char* argv[], char file_name[MAX_LINE]);

#endif