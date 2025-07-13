#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

void print_listener_address(int lfd)
{
	struct sockaddr_in sin;
	assert(getsockname(lfd, (struct sockaddr *)&sin, &(socklen_t){sizeof(sin)}) == 0);

	const size_t buf_size = INET_ADDRSTRLEN;
	char buffer[INET_ADDRSTRLEN];
	assert(inet_ntop(AF_INET, &sin.sin_addr, buffer, buf_size) != NULL);

	fprintf(stderr, "Listening on %s:%d\n", buffer, ntohs(sin.sin_port));
}

int create_listener(char *address)
{
	const char *ip = strtok(address, ":");
	const char *port = strtok(NULL, "\0");
	int sockfd = create_socket();
	struct sockaddr_in sin;
	struct in_addr addr;

	assert(inet_pton(AF_INET, ip, &addr) == 1);

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = addr.s_addr;
	sin.sin_port = htons(atoi(port));
	assert(bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) == 0);
	assert(listen(sockfd, 8) == 0);
	free(address);
	return sockfd;
}

int create_socket()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);
	assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == 0);
	return sockfd;
}