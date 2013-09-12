#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "common.h"

enum {
	MSG_ONLINE,
	MSG_OFFLINE,
	MSG_FILE_RQST,
	MSG_FILE_ACK,
	MSG_CHAT_RQST,
	MSG_CHAT_ACK,
	MSG_PEER_INF,
	MSG_CHAT,
};
enum {
	RSP_NO = 0,
	RSP_YES,
};
struct message {
	int type;
	struct base_inf id;

	union {
		int m_answer;
		struct file_inf m_file;
		char m_news[64];
		char m_chat[64];

	} mdata;
};
#define answer mdata.m_answer
#define msfile mdata.m_file
#define msnews mdata.m_news
#define mschat mdata.m_chat

void *msg_handler(void *);

#endif
