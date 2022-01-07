#ifndef HELPER_FUNCTION
#define HELPER_FUNCTION


/// <summary>
/// 
/// </summary>
/// <param name="mutex_handle"></param>
/// <returns></returns>
//int wait_for_mutex(HANDLE mutex_handle);
/// <summary>
/// 
/// </summary>
/// <param name="mutex_handle"></param>
/// <returns></returns>
//int release_mutex(HANDLE mutex_handle);
/// <summary>
/// 
/// </summary>
/// <param name="input"></param>
/// <returns></returns>
int decode_message( char* input);
/// <summary>
/// 
/// </summary>
/// <param name="write_file_name"></param>
/// <param name="message"></param>
/// <param name="offset"></param>
/// <param name="size_of_file_to_write"></param>
/// <returns></returns>
int write_to_file(char* write_file_name, char* message, int offset, int size_of_file_to_write);
/// <summary>
/// 
/// </summary>
/// <param name="start"></param>
/// <param name="log_file"></param>
/// <param name="input"></param>
/// <returns></returns>
int write_to_log_file(char* start, char* log_file, char* input);



#endif




