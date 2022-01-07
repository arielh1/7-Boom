/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
 This file was written for instruction purposes for the 
 course "Introduction to Systems Programming" at Tel-Aviv
 University, School of Electrical Engineering, Winter 2011, 
 by Amnon Drory.
*/
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#ifndef SOCKET_EXAMPLE_SHARED_H
#define SOCKET_EXAMPLE_SHARED_H
#include "../shared/helper_function.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define SERVER_ADDRESS_STR "127.0.0.1"
#define MAX_LEN_NAME 50
#define _CRT_SECURE_NO_WARNINGS

#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )



#define ERROR_CHAR "-1"
#define SENT_TO_SERVER "sent to server-"
#define RECIVED_FROM_SERVER "received from server-"
#define SERVER_CONNECTION_FAILIURE "Failed Connecting to Server on "
#define SERVER_CONNECTION_APPROVED "Connected to server on"
#define CLIENT_REQUEST "CLIENT_REQUEST:"
#define CLIENT_VERSUS "CLIENT_VERSUS"
#define CLIENT_PLAYER_MOVE "CLIENT_PLAYER_MOVE:"
#define CLIENT_DISCONNECT "CLIENT_DISCONNECT"
#define CLIENT_ILLEGAL_COMMAND "Error: Illegal command\n"


#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define ERROR_CODE ((int)(-1))
#define SUCCESS_CODE 0
#define NUM_OF_COLUMNS 5
#define NUM_OF_FILES  5
#define MAX_THREADS  10
#define LENGTH  3
#define BRUTAL_TERMINATION_CODE 0x55
#define FILE_NAME_SIZE 100 
#define SON_LEN 10
#define NUM_OF_PARAM 5
#define WITHSPACE " "
#define BACKSLASH "\n"
#define HANDLE_FALIURE_MESSAGE "Error when closing handle\n"
#define ARGUMENTS_POINTER_NULL "Arguments Pointer to manage_calculations is NULL\n"
#define THRAED_FALIURE_MESSAGE "Thread Creation Failed!\n"
#define PROCESS_FALIURE_MESSAGE "Process Creation Failed!\n"
#define PROCESS_TIMEOUT_TERMINATION_MESSAGE "Process was not terminated before timeout!\nTerminating brutally!\n"
#define EXIT_CODE_ERROR_MESSAGE "failed getting exit code\n"
#define CREATE_FILE_FALIURE_MESSAGE "Terminal failure to create file\n"
#define GET_SIZE_FALIURE_MESSAGE "Terminal failure to get file size\n"
#define OFFSET_ERROR_MESSAGE "offset error was occurred\n"
#define WRITE_TO_FILE_ERROR_MESSAGE "Terminal failure to write to the file\n"
#define WRRITEN_BYTES_ERROR_MESSAGE "Error: dwBytesWritten != dwBytesToWrite\n"
#define SPARE_ARRAY_FOR_COMMAND_LINE 7
#define SPACE_FOR_BACKSLASH_0  1
#define MEMORY_ALLOCATION_FALIURE_MEESAGE "Memory allocation failed\n"
#define PROCESS_WAIT_ERROR_MESSAGE "Error code from the WaitForSingleObject function \n "
#define EMPY_FILE_MESSAGE "File is empty \n "
#define ERROR_IN_THE_NUMBER_OF_ARGUMENTS " wrong numbers of arguments.\n"
#define NULL_POINTER "Received null pointer \n"
#define ERROR_IN_PATH_LENGTH "path length smaller then zero \n "
#define ERROR_IN_THREAD_NUMBER_X "thread (%d) has failed \n"
#define ERROR_IN_WAITING_TIME "Error when waiting for thread\n"
#define MEMORY_ALLOCATION_FALIURE_MEESAGE "Memory allocation failed\n"
#define ARGUMENTS_POINTER_NULL "Arguments Pointer to manage_calculations is NULL\n"
#define CLIENT_CHOOSE_P_Q "Choose what to do next:\n1. Play against another client\n2. Quit\n"
#define CLIENT_CHOOSE_T_E "Failed connecting to server on %s:%s.\n1. Try to reconnect\n2. Exit\n"

#define WRITE_TO_LOG_MSG_TRN_FAIL "message transfer failed. Exiting.\n"
#define WRITE_TO_LOG_MSG_DISCCONECT "Player disconnected. Exiting.\n"
#define WRITE_TO_LOG_WRONG_MSG "Message error. Exiting.\n"
#define SENT_TO_CLIENT "sent to client-"
#define RECIEVED_FROM_CLIENT "recieved from client-"
#define MAX_LINE 100
#define TXT_TYPE ".txt"
#define OUTPUT_DIRECTORY "./Output"

#define SERVER_DENIED "SERVER_DENIED\n"
#define SERVER_APPROVED "SERVER_APPROVED\n"
#define SERVER_MAIN_MENU "SERVER_MAIN_MENU\n"
#define GAME_STARTED "GAME_STARTED\n"
#define GAME_ENDED "GAME_ENDED\n"
#define SERVER_NO_OPPONENTS "SERVER_NO_OPPONENTS\n"
#define SERVER_OPPONENT_QUIT "SERVER_OPPONENT_QUIT\n"
#define SERVER_MOVE_REQUEST "SERVER_MOVE_REQUEST\n"



#define SERVER_ADDRESS_STR "127.0.0.1"

#define RESPOND_TIME 15000
#define SIZE_OF_LOG_FILE 40
#define MAX_SIZE_OF_USERNAME 20
#define MAX_SIZE_OF_MESSAGE_TYPE 25
#define MAX_SIZE_OF_PARAM 20
#define MAX_PARAM_NUM 3
#define CLIENT_DISSCONECT 2
#define READ 1
#define WRITE 2
#define TIMEOUT -2
#define TIMEOUT_15_SEC 15
typedef struct Message {
	char* message_type;
	char* param[MAX_PARAM_NUM];
}Message;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#endif // SOCKET_EXAMPLE_SHARED_H