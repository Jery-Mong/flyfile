


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
		handle_ack(msg);
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
