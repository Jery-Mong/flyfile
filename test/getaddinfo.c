#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main()
{
        int inet_sock;
	
        struct ifreq *ifr;
	struct ifconf ifc;

	
	ifc.ifc_len = sizeof(struct ifreq) * 10;
	ifc.ifc_buf = (struct freq *)malloc(ifc.ifc_len);
	
        inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
        //strcpy(ifr.ifr_name, "eth0");
	
        if (ioctl(inet_sock, SIOCGIFCONF, &ifc) < 0)
                perror("ioctl");

	int i = ifc.ifc_len / sizeof(struct ifreq);
	for (ifr = ifc.ifc_req; i > 0; i-- ) {
		printf("name:%s\n", ifr->ifr_name);
		printf("ip:%s\n", inet_ntoa(((struct sockaddr_in*)&(ifr->ifr_addr))->sin_addr));
		printf("\n");
		ifr++;
	}
        return 0;
}
