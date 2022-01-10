
/*
ariel hacohen 313325938
ofir atiya 311142541
ex4
client is get a arg in this format <client.exe> <ip> >< port>< name>
*/
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
/// set timeout to socket
/// </summary>
/// <param name="sock">socket</param>
/// <param name="timeout"></param>
/// <returns> return 0 if no error code </returns>
int set_timeout(SOCKET sock, DWORD timeout);

/// <summary>
/// check if recvres reecived an error
/// </summary>
/// <param name="RecvRes">enum which old states of transacrions</param>
/// <returns> return 0 if no error code </returns>
int check_failed_disconnected(TransferResult_t RecvRes);
/// <summary>
/// checks if the user inserted to the program apositive integer
/// </summary>
/// <param name="str">recieve string from user</param>
/// <returns>return 1 if input is valid 0 else</returns>
int is_digit(char* str);

/// <summary>
/// main function to manage the client
/// </summary>
/// <param name="port">port number</param>
/// <param name="argv">arguments</param>
/// <returns>return code_exit if no errors</returns>
int MainClient(int port, char* argv[]);
/// <summary>
/// 
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[]);


#endif
