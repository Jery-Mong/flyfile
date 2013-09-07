#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define DO_UDP 1

#define PORT 3333
int main()
{
	struct sockaddr_in addr;
	       
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	       
#ifdef DO_UDP
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
#else
	int fd = socket(AF_INET, SOCK_STREAM, 0);
#endif
		       
	bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	       
#ifndef DO_UDP
	listen(fd, 10);
	int nfd = accept(fd, NULL, NULL);
	if (nfd < 0)
		printf("error\n");
	else
		printf("get a client\n");
	       
#endif
	char buf[128];
	       
	while (1) {
#ifdef DO_UDP
		read(fd, buf, 128);
#else
		read(nfd, buf, 128);
#endif
		sleep(1);
		printf("%s\n", buf);
	}
}
