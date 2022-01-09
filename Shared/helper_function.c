#include "helper_function.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "../Shared/SocketSendRecvTools.h"
#include <stdlib.h>


/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int write_to_file(char* write_file_name, char* message) {
	HANDLE hFile = NULL;
	DWORD dwBytesWritten=0 ;
	DWORD dwBytesToWrite = (DWORD)strlen(message);
	BOOL bErrorFlag = FALSE;
	//create file
	hFile = CreateFileA(write_file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		printf(CREATE_FILE_FALIURE_MESSAGE);
		return ERROR_CODE;
	}
	//set pointer

	
	if (SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) 
		
	{
		printf(OFFSET_ERROR_MESSAGE);
		return ERROR_CODE;
	}

	bErrorFlag = WriteFile(hFile, message, dwBytesToWrite, &dwBytesWritten, NULL);
	if (bErrorFlag == FALSE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		return ERROR_CODE;
	}
	
	CloseHandle(hFile);
	return SUCCESS_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int write_to_log_file( char* log_file,char *input ) {
	if (write_to_file(log_file, input) != SUCCESS_CODE) {
		printf(WRITE_TO_FILE_ERROR_MESSAGE);
		
		return ERROR_CODE;
	}

	return SUCCESS_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void free_message(Message* message)
{
	free(message->message_type);
	int i = 0;
	for (i = 0; i < 3; i++) {
		if (message->param[i] != NULL) {
		free(message->param[i]);

		}
			
	}
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int decode_message(char* input,  Message * message,char *send_or_recv)
{
	char* token, * next_token=NULL,*type=NULL;
	char seps1[] = ":,\n";
	char seps2[] = ";,\n";
	int i;
	message->message_type = NULL;
	message->message_type = malloc(sizeof(char) * MAX_SIZE_OF_PARAM);
	if (!message->message_type) {
		printf(MEMORY_ALLOCATION_FALIURE_MEESAGE);
		return ERROR_CODE;
	}
	message->param[0] = NULL ;
	message->param[1] = NULL;
	message->param[2] = NULL;

	type = strtok(input, seps1);
	strcpy(message->message_type,type);
	token = strtok(NULL, seps2);
	for ( i = 0; i < MAX_PARAM_NUM; i++) {
		if (token == NULL)
			break;
		message->param[i] = malloc(sizeof(char) * MAX_SIZE_OF_PARAM);
		if (!message->param[i]) {
			printf(MEMORY_ALLOCATION_FALIURE_MEESAGE);
			return ERROR_CODE;
		}
		
		strcpy(message->param[i], token);
		token = strtok(NULL, seps2);
	}
	switch (i)
	{
	case 0:
		sprintf(message->log_file_format, "%s from server-%s\n", send_or_recv, message->message_type);
		break;
	case 1:
		sprintf(message->log_file_format, "%s from server-%s:%s\n", send_or_recv, message->message_type, message->param[0]);
		break;
	case 2:
		sprintf(message->log_file_format, "%s from server-%s:%s;%s\n", send_or_recv, message->message_type, message->param[0], message->param[1]);
		break;
	case 3:
		sprintf(message->log_file_format, "%s from server-%s:%s;%s;%s\n", send_or_recv, message->message_type, message->param[0], message->param[1], message->param[2]);
		break;

	default:
		break;
	}

	
	return SUCCESS_CODE;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/