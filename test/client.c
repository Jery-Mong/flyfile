#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>

#define DO_UDP
#define PORT 3333

int main()
{
	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	
#ifdef DO_UDP
	//server_addr.sin_addr.s_addr = INADDR_BROADCAST;
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.255");
#else
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.2");
#endif


#ifdef DO_UDP
	int fd = socket(server_addr.sin_family, SOCK_DGRAM, 0);
	int broadcast = 1;
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
#else
	int fd = socket(server_addr.sin_family, SOCK_STREAM, 0);
#endif	

	if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
		printf("connected\n");
	
	char *p = "hello";

	while(1) {
		send(fd, p, strlen(p)+1, 0);
		
		printf("send %s\n", p);
		sleep(1);
	}
}
