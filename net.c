#include <sys/sock.h>
#include <netinet/in.h>
#include <stdlib.h>

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
		bind (fd, (struct sockaddr *)&addr, sizeof(addr));
		return fd;
		
	case FD_BROADCAST:
	case FD_SENDMSG:
		addr.sin_port = htons(MSG_PORT);
		
		if (type == FD_BROADCAST) {
			addr.sin_addr.s_addr = INADDR_BROADCAST;
			int broadcast = 1; 
			setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
		} else {
			addr.sin_addr.s_addr = pr->id.ip;
		}
		fd = socket(AF_INET, SOCK_DGRAM, 0);
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
	send(fd, &self->id, sizeof(struct base_inf), 0);

	shutdown(fd, SHUT_RDWR);
}
struct peer *peer_inlist(struct message *msg)
{
	if (msg->type != MSG_PEER_INF)
		return;

	struct peer *pr = (struct peer *)malloc(sizeof(struct peer));
	memset(pr, 0, sizeof(struct peer));
	
	pr->id = (struct base_inf *)malloc(sizeof(struct base_inf));
	memcpy(pr->id, &msg->data, sizeof(struct base_inf));
	
	list_insert_tail(peer_list, pr);

	free(msg);
	return pr;
}

in_addr_t get_local_ip()
{
	int fd;

	in_addr_t ip;
	in_addr_t mask = inet_addr("192.168.0.0");
	struct sockaddr_in *addr;
	
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
	send(fd, &self->id, sizeof(struct base_inf), 0);
	shutdown(fd, SHUT_RDWR)
}
void recv_msg()
{
	struct message msg;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_setdetachstate(&addr, PTHREAD_CREATE_DETACHED);
	
	int fd = getsockfd(FD_GETMSG, NULL);	
	while (1) {
		
		int size = recv(fd, &msg, sizeof(struct message), 0);
		
		if (msg->p_id.ip == self->s_id.ip)
			continue;
		if (size != sizeof(struct message))
			continue;

		struct message *dupmsg = (struct message *)malloc(sizeof(struct message));
		memcpy(dupmsg, &msg, sizeof(message));
		
		pthread_t tid;
		pthread_create(&tid, &attr, msg_handler, msg);
	}
}
