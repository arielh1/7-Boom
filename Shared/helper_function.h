#ifndef HELPER_FUNCTION
#define HELPER_FUNCTION

#include "../Shared/SocketExampleShared.h"


/// <summary>
/// 
/// </summary>
/// <param name="write_file_name"></param>
/// <param name="message"></param>
/// <returns></returns>
int write_to_file(char* write_file_name, char* message);
/// <summary>
/// 
/// </summary>
/// <param name="start"></param>
/// <param name="log_file"></param>
/// <param name="input"></param>
/// <returns></returns>
int write_to_log_file(char* log_file, char* input);
/// <summary>
/// 
/// </summary>
/// <param name="message"></param>
void free_message(Message* message);
/// <summary>
/// 
/// </summary>
/// <param name="input"></param>
/// <param name="message"></param>
/// <param name="send_or_recv"></param>
/// <returns></returns>
int decode_message(char* input,Message * message,char * send_or_recv);
#endif




