#ifndef HELPER_FUNCTION
#define HELPER_FUNCTION

#include "../Shared/SocketExampleShared.h"


/// <summary>
/// create file and write into it
/// </summary>
/// <param name="write_file_name">log file name</param>
/// <param name="message">message from/to server</param>
/// <returns></returns>
int write_to_file(char* write_file_name, char* message);
/// <summary>
/// write input to log file
/// </summary>
/// <param name="log_file">log file name</param>
/// <param name="input">input to write to log</param>
/// <returns></returns>
int write_to_log_file(char* log_file, char* input);
/// <summary>
/// free nessage struct
/// </summary>
/// <param name="message">message from/to server</param>
void free_message(Message* message);
/// <summary>
/// recieve input and decode it and store into message struct
/// </summary>
/// <param name="input">input to decode</param>
/// <param name="message">message from/to server</param>
/// <param name="send_or_recv">mode of decode for print</param>
/// <returns>return SUCCESS_CODE if no errors</returns>
int decode_message(char* input,Message * message,char * send_or_recv);


#endif




