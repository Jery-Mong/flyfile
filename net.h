#ifndef __NET_H
#define __NET_H

#define MSG_PORT 3333
#define DATA_PORT 3334


enum sockfd_type {
	FD_BROADCAST,
	FD_DATACLIENT,
	FD_DATASERVER,
	FD_GETMSG,
	FD_SENDMSG,
};




#endif
