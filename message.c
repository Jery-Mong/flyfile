#include <stdio.h>
#include "net.h"
#include "message.h"
#include "local.h"


void *msg_handler(void *data)
{
	struct message *msg = (struct message *)data;

	switch (msg->type) {
	case MSG_OFFLINE:
		peer_outlist(msg) ;
		break;
	case MSG_ONLINE:
		peer_online(msg);
		break;
	case MSG_FILE_RQST:
	case MSG_CHAT_RQST:
//		respond_rqst(msg);
		break;
	case MSG_FILE_START:
		//	receive_file(msg);
		break;
	case MSG_FILE_ACK:
	case MSG_CHAT_ACK:
		handle_answer(msg);
		break;
	case MSG_PEER_INF:
		peer_inlist(msg);
		break;
	default:
		break;
	}
	return NULL;
}

