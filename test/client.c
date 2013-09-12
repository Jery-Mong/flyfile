#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>


#define PORT 3333

int main()
{
	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.19");
	
	int fd = socket(server_addr.sin_family, SOCK_DGRAM, 0);

	if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
		printf("connected\n");
	
	char *p = "hello";

//	while(1) {
		send(fd, p, strlen(p)+1, 0);
		printf("send %s\n", p);
//	}
	return 0;
}
