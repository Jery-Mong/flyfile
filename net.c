#include <sys/sock.h>
#include <netinet/in.h>


void msg_handler(void *data)
{
	struct *msg = (struct message *)data;

	switch (msg->type) {
	case MSG_ONLINE:
		peer_outlist(msg) ;
	case MSG_ONLINE:
		peer_online(msg);
		break;
	case MSG_FILE_RQST:
	case MSG_CHAT_RQST:
		respond_rqst(msg);
		break;
	case MSG_FILE_START:
		receive_file(msg);
		break;
	case MSG_FILE_ACK:
		handle_ack();
		break;
	case MSG_CHAT:
		chat_with_peer(msg);
		break;
	case MSG_PEER_INF:
		peer_inlist(msg);
		break;
	default:
		break;
	}
}

int get_sockfd(int type, struct peer *pr)
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
	int fd = get_sockfd(FD_BROADCAST, NULL);
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

void get_local_ip()
{
	int inet_sock;
	
        struct ifreq *ifr;
	struct ifconf ifc;

	
	ifc.ifc_len = sizeof(struct ifreq) * 5;
	ifc.ifc_buf = (struct freq *)malloc(ifc.ifc_len);
	
        inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	
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
}
void peer_online(struct message *msg)
{
	if (msg->type != MSG_ONLINE)
		return;
	int fd;
	struct peer *pr;
	msg->type = MSG_PEER_INF;
	pr = peer_inlist(msg);
	
	fd = get_sockfd(FD_SENDMSG, pr);
	send(fd, &self->id, sizeof(struct base_inf), 0);
	shutdown(fd, SHUT_RDWR)
}
void recv_msg()
{
	struct message msg;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_setdetachstate(&addr, PTHREAD_CREATE_DETACHED);
	
	int fd = get_sockfd(FD_GETMSG, NULL);	
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
