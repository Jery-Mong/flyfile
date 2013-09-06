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
	struct base_inf id;

	union {
		int m_answer;
		struct file_inf m_file;
		char m_news[64];

	} mdata;
};
#define answer mdata.m_answer
#define ms_file mdata.m_file
#define ms_news mdata.m_news

void *msg_handler(void *);

#endif
