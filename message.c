#include <stdio.h>

#include "net.h"
#include "message.h"
#include "local.h"
#include "interface.h"

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
	case MSG_PEER_INF:
		peer_inlist(msg);
		break;
	case MSG_FILE_RQST:
	case MSG_CHAT_RQST:
		respond_rqst(msg);
		break;
	case MSG_FILE_ACK:
	case MSG_CHAT_ACK:
		handle_ack(msg);
		break;
	case MSG_CHAT:
		chat_get_comment(msg);
	default:
		break;
	}
	return NULL;
}

