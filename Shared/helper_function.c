#include "../shared/helper_function.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../Shared/SocketExampleShared.h"
#include "../Shared/SocketSendRecvTools.h"
#include <stdlib.h>
/// <summary>
/// this function calls WaitForSingleObject and checks if error occured
/// </summary>
/// <param name="mutex_handle">mutex var</param>
/// <returns>0 if function succeed, (-1) else</returns>
int wait_for_mutex(HANDLE mutex_handle) {
	//Wait for the mutex to become available, then take ownership
	DWORD wait_code;
	wait_code = WaitForSingleObject(mutex_handle, INFINITE);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting for mutex\n");
		return ERROR_CODE;
	}
	return SUCCESS_CODE;
}

/// <summary>
/// this function calls ReleaseMutex and checks if error occured
/// </summary>
/// <param name="mutex_handle">mutex var</param>
/// <returns>0 if function succeed, (-1) else</returns>
int release_mutex(HANDLE mutex_handle) {
	BOOL ret_val;
	ret_val = ReleaseMutex(mutex_handle);
	if (FALSE == ret_val)
	{
		printf("Error when releasing\n");
		return ERROR_CODE;
	}
	return SUCCESS_CODE;
}


int write_to_file(char* write_file_name, char* message, int offset, int size_of_file_to_write) {
	HANDLE file_name = NULL;
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;
	//create file
	file_name = CreateFileA(write_file_name, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_name == INVALID_HANDLE_VALUE) {
		printf(CREATE_FILE_FALIURE_MESSAGE);
		return ERROR_CODE;
	}
	//set pointer
	DWORD dwPtr = SetEndOfFile(file_name);
	if (dwPtr == INVALID_SET_FILE_POINTER) // Test for failure
	{
		printf(OFFSET_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	//write to file
	bErrorFlag = WriteFile(file_name, message, size_of_file_to_write, &dwBytesWritten, NULL);
	if (bErrorFlag == FALSE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	if (dwBytesWritten != size_of_file_to_write) {
		printf(WRRITEN_BYTES_ERROR_MESSAGE);
	}
	CloseHandle(file_name);
	return SUCCESS_CODE;
}

int write_to_log_file(char* start, char* log_file,char *input ) {
	int start_len = strlen(start);
	int str_len = strlen(input);
	int message_len = start_len + str_len;
	char* message_str = NULL;
		message_str = (char*)calloc(message_len + 1, sizeof(char));
	if(message_str==NULL)
		return ERROR_CODE;
	strcpy(message_str, start);
	strcat(message_str, input);
	if (write_to_file(log_file, message_str, 0, message_len) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		free(message_str);
		return ERROR_CODE;
	}
	free(message_str);
	return SUCCESS_CODE;
}

int decode_message( char* input)
{
//	char *token, *next_token;
	//char seps1[] = ":,\n";
	//char seps2[] = ";,\n";

	/*token = strtok_s(input, seps1, &next_token);
	strcpy(message->message_type, token);
	token = strtok_s(NULL, seps2, &next_token);
	for (int i = 0; i < MAX_PARAM_NUM; i++) {
		if (token == NULL)
			break;
		message->param[i] = malloc(sizeof(char) * MAX_SIZE_OF_PARAM);
		if (!message->param[i]) {
			printf(MEMORY_ALLOCATION_FALIURE_MEESAGE);
			return ERROR_CODE;
		}
		strcpy(message->param[i], token);
		token = strtok_s(NULL, seps2, &next_token);
	}*/
	return SUCCESS_CODE;
}
