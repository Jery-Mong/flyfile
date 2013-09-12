#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h> 


#include "common.h"
#include "net.h"
#include "message.h"
#include "local.h"
#include "interface.h"

int getsockfd(int type, void *data)
{
	struct peer *pr = (struct peer *)data;
	int fd;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	
	switch (type) {
	case FD_GETMSG:
		addr.sin_port = htons(MSG_PORT);
		//addr.sin_addr.s_addr = self->id.ip;
		addr.sin_addr.s_addr = INADDR_ANY;
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		bind(fd, (struct sockaddr *)&addr, sizeof(addr));
		
		return fd;
	case FD_BROADCAST:
	case FD_SENDMSG:
		addr.sin_port = htons(MSG_PORT);
		
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (type == FD_BROADCAST) {
			addr.sin_addr.s_addr = self->bcastaddr;
			int broadcast = 1; 
			setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
		} else {
			addr.sin_addr.s_addr = pr->id.ip;
		}
		
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
			return -1;
		
		return fd;
		
	case FD_DATA_SEND:
		if (pr == NULL)
			return -1;
		
		addr.sin_port = htons(DATA_PORT);
		addr.sin_addr.s_addr = pr->id.ip;
		fd = socket(AF_INET, SOCK_STREAM, 0);
		
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
			return -1;
		
		return fd;
		
	case FD_DATA_RECV:
		if (pr == NULL)
			return -1;
		
		addr.sin_port = htons(DATA_PORT);
		addr.sin_addr.s_addr = self->id.ip;
		fd = socket(AF_INET, SOCK_STREAM, 0);
		bind(fd, (struct sockaddr *)&addr, sizeof(addr));
		listen(fd, 1);
		return accept(fd, NULL, NULL);
		
	default:
		break;	
	}
	return -1;
}

void bcast_status(int status)
{
	int fd = getsockfd(FD_BROADCAST, NULL);

	struct message bmsg;
	bmsg.type = status;
	memcpy(&bmsg.id, &self->id, sizeof(struct base_inf));
	
	send(fd, &bmsg, sizeof(struct message), 0);

	close(fd);
}
void bcast_online()
{
	bcast_status(MSG_ONLINE);
}
void bcast_offline()
{
	bcast_status(MSG_OFFLINE);
}

void get_local_ipinf(void *lip, void *bip)
{
	int fd;
	in_addr_t ip;
	struct sockaddr_in *addr;
	in_addr_t mask = inet_addr("192.168.0.0");

	struct ifconf ifc;
	struct ifreq *ifr;

	ifc.ifc_len = sizeof(struct ifreq) * 4;
	ifc.ifc_req = (struct ifreq *)malloc(ifc.ifc_len);	

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0)
		perror("ioctl");

	int i = ifc.ifc_len / sizeof(struct ifreq);
	
	for (ifr = ifc.ifc_req; i > 0; i--) {
		addr = (struct sockaddr_in *)&ifr->ifr_addr;
		ip = addr->sin_addr.s_addr;
		
		if ((ip & mask) == mask) {
			*(in_addr_t *)lip = ip;

			ioctl(fd, SIOCGIFBRDADDR, ifr);
			*(in_addr_t *)bip = (((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr).s_addr;
			return;
		}
		
		ifr++;
	}
	close(fd);
}
void peer_online(void *data)
{
	struct peer *pr = peer_inlist((struct message *)data);

	if (pr == NULL)
		return;
		
	struct message msg;
	msg.type = MSG_PEER_INF;
	memcpy(&msg.id, &self->id, sizeof(struct base_inf));
		
	int fd = getsockfd(FD_SENDMSG, pr);
	if (fd < 0)
		m_printf("send msg error\n");

	int i;
	for (i = 0; i < 3; i++) {
		sleep(1);
		send(fd, &msg, sizeof(struct message), 0);
		
		close(fd);
	}
}
void *recv_msg(void *data)
{
	struct message msg;
	struct message *dupmsg;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	int fd = getsockfd(FD_GETMSG, NULL);
	
	while (1) {
		
		recv(fd, &msg, sizeof(struct message), 0);

		if (msg.id.ip == self->id.ip)
			continue;

		dupmsg = (struct message *)malloc(sizeof(struct message));
		memcpy(dupmsg, &msg, sizeof(struct message));
		
		pthread_create(&self->msg_handler_thr, &attr, msg_handler, dupmsg);
	}
}
