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

int getsockfd(int type, struct peer *pr)
{
	int fd;
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	
	switch (type) {
	case FD_GETMSG:
		addr.sin_port = htons(MSG_PORT);
		addr.sin_addr.s_addr = INADDR_ANY;
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		bind(fd, (struct sockaddr *)&addr, sizeof(addr));
		
		return fd;
	case FD_BROADCAST:
	case FD_SENDMSG:
		addr.sin_port = htons(MSG_PORT);
		
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (type == FD_BROADCAST) {
			addr.sin_addr.s_addr = INADDR_BROADCAST;
			int broadcast = 1; 
			setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
		} else {
			addr.sin_addr.s_addr = pr->id.ip;
		}
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
			return -1;
		else
			return fd;
		
	case FD_DATACLIENT:
		
		if (pr == NULL)
			return -1;
		addr.sin_port = htons(DATA_PORT);
		addr.sin_addr.s_addr = pr->id.ip;
		fd = socket(AF_INET, SOCK_STREAM, 0);
		
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
			return -1;
		else
			return fd;
		
	case FD_DATASERVER:
		break;
	default:
		break;	
	}
	return -1;
}
void bcast_online()
{
	int fd = getsockfd(FD_BROADCAST, NULL);

	struct message bmsg;
	bmsg.type = MSG_ONLINE;
	memcpy(&bmsg.m_id, &self->id, sizeof(struct base_inf));
	
	send(fd, &bmsg, sizeof(struct message), 0);

	shutdown(fd, SHUT_RDWR);
}
in_addr_t get_local_ip()
{

	int fd;
	in_addr_t ip;
	struct sockaddr_in *addr;
	in_addr_t mask = inet_addr("192.168.0.0");
	
	struct ifreq *ifr;
	struct ifconf ifc;
	
	ifc.ifc_len = sizeof(struct ifreq) * 4;
	ifc.ifc_buf = (struct freq *)malloc(ifc.ifc_len);
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0)
		perror("ioctl");

	int i = ifc.ifc_len / sizeof(struct ifreq);
	
	for (ifr = ifc.ifc_req; i > 0; i-- ) {
		addr = (struct sockaddr_in *)&ifr->ifr_addr;
		ip = addr->sin_addr.s_addr;;
		
		if ((ip & 0x0000ffff) == mask)
			return ip;
		else
			ifr++;
	}

	shutdown(fd, SHUT_RDWR);
	return 0;
}
void peer_online(struct message *msg)
{
	if (msg->type != MSG_ONLINE)
		return;
	int fd;
	struct peer *pr;
	msg->type = MSG_PEER_INF;
	pr = peer_inlist(msg);	

	fd = getsockfd(FD_SENDMSG, pr);

	msg->type = MSG_PEER_INF;
	memcpy(&msg->m_id, &self->id, sizeof(struct base_inf));
	
	send(fd, msg, sizeof(struct message), 0);

	shutdown(fd, SHUT_RDWR);
	free(msg);
}
void *recv_msg(void *data)
{
	struct message msg;
	pthread_t tid;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	int fd = getsockfd(FD_GETMSG, NULL);	
	while (1) {
		
		recv(fd, &msg, sizeof(struct message), 0);

		printf("get a message from: %s\n", msg.m_id.name);
		
		if (msg.m_id.ip == self->id.ip)
			continue;

		struct message *dupmsg = (struct message *)malloc(sizeof(struct message));
		memcpy(dupmsg, &msg, sizeof(struct message));
		
		pthread_create(&tid, &attr, msg_handler, dupmsg);
	}
}
