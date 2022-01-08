#ifndef HELPER_FUNCTION
#define HELPER_FUNCTION
typedef struct Message {
	char* message_type;
	char* param[3];
	char log_file_format[100];
}Message;


int write_to_file(char* write_file_name, char* message);
/// <summary>
/// 
/// </summary>
/// <param name="start"></param>
/// <param name="log_file"></param>
/// <param name="input"></param>
/// <returns></returns>
int write_to_log_file(char* log_file, char* input);



int decode_message(char* input,Message * message,char * send_or_recv);
#endif




