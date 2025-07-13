#include <sys/socket.h>
#include <stdio.h>

void handle_connection(int connected_sock)
{
	char buffer[512];
	recv(connected_sock, buffer, sizeof(buffer), 0);
	printf("%s\n", buffer);
}