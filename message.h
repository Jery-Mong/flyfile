#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "common.h"

enum {
	MSG_ONLINE,
	MSG_OFFLINE,
	MSG_FILE_START,
	MSG_FILE_RQST,
	MSG_FILE_ACK,
	MSG_CHAT_RQST,
	MSG_CHAT_ACK,
	MSG_PEER_INF,
};
enum {
	RSP_YES,
	RSP_NO,
};
struct message {
	int type;

	union {
		struct base_inf id;
		int m_answer;

	} mdata;
};
#define m_id mdata.id
#define answer mdata.m_answer

void *msg_handler(void *);



#endif
