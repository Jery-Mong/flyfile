#ifndef __MESSAGE_H
#define __MESSAGE_H

enum {
	MSG_ONLINE,
	MSG_OFFLINE,
	MSG_FILE_RQST,
	MSG_FILE_START,
	MSG_FILE_ACK,
	MSG_CHAT_RQST,
	MSG_CHAT_RQST,
	MSG_PEER_INF,
};

struct message {
	int type;
	struct base_inf p_id;
	
	char data[128];
};

#endif
