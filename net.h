#ifndef __NET_H
#define __NET_H

#define MSG_PORT 3333
#define DATA_PORT 3334

enum sockfd_type {
	FD_BROADCAST,
	FD_DATA_SEND,
	FD_DATA_RECV,
	FD_GETMSG,
	FD_SENDMSG,
};

enum {
	FILE_BUSY,
	FILE_AVAL,
};

void *recv_msg(void *);
void peer_online(void *);
//in_addr_t get_local_ip();
void get_local_ipinf(void *, void *);
void bcast_online();
void bcast_offline();
int getsockfd(int, void *);


#endif
