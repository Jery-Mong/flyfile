#ifndef __MESSAGE_H
#define __MESSAGE_H

enum message_type {
	MSG_FILE_REQ,
	MSG_FILE_CFM,
	MSG_FILE_START,		/* file data is this message's data */
	MSG_ONLINE,
	MSG_OFFLINE,
	MSG_CHAT,
};

struct msg_raw {
	int msg_type;
	int data_size;
};

	
class message {
public:
	int send();
	int receive();
	message(int mtype);
	
	struct msg_raw *netpack;
	int msg_type;
	void *msg_data;
private:
};
#endif
