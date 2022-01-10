#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "main_client.h"
#include "state_machine_client.h"
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int check_failed_disconnected(TransferResult_t RecvRes) {
	if (RecvRes == TRNS_FAILED)
	{
		if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		printf("Server disconnected. Exiting.\n");
		return ( ERROR_CODE);
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		if (write_to_file(name_file, "Server disconnected. Exiting.\n") != SUCCESS_CODE) {
			printf(WRITE_TO_FILE_ERROR_MESSAGE);
			return ERROR_CODE;
		}
		printf("Server disconnected. Exiting.\n");
		return ERROR_CODE;
	}
	else
		return 0;

}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int is_digit(char* str) {

	for (unsigned int index = 0; index < strlen(str); index++) {
		if (!isdigit(str[index]))
		{
			return 0;
		}
	}
	return 1;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int MainClient(int port,char *argv[])
{
	int code_exit;
	char file_name[FILE_NAME_SIZE] = { 0 };
	SOCKADDR_IN clientService;
	WSADATA wsaData; //Create a WSADATA object called wsaData.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return ERROR_CODE;
	}
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(argv[1]); //Setting the IP address to connect to
	clientService.sin_port = htons(port); //Setting the port to connect to.
	set_timeout(m_socket, (DWORD)RESPOND_TIME);
	code_exit = state_machine(clientService, argv[1], port, argv, file_name) == ERROR_CODE;
	WSACleanup();
	closesocket(m_socket);

	return code_exit;
}
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int main(int argc, char* argv[]) {
	int exitcode = -1;
	sprintf(name_file, "client_log_%s.txt", argv[3]);
	exitcode= MainClient( atoi(argv[2]), argv);
	DeleteFileA(name_file);
	_CrtDumpMemoryLeaks();
	return exitcode;
	
}



