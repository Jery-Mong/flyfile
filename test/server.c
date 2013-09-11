#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define PORT 3333
int main()
{
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
//	addr.sin_addr.s_addr = inet_addr("192.168.1.2");
	addr.sin_addr.s_addr = INADDR_ANY;
	       

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
		       
	bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	char buf[128];
	       
//	while (1) {
		recv(fd, buf, 128, 0);
		printf("%s\n", buf);
//		memset(buf, 0, 128);
//	}
}
