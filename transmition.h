#ifndef __TRANSMITION_H
#define __TRANSMITION_H

#define NAME_LEN 128

enum message_type {
	MSG_FILE_REQ,
	MSG_FILE_CFM,
	MSG_FILE_START,		/* file data is this message's data */
	MSG_ONLINE,
	MSG_OFFLINE,
	MSG_CHAT,
};
struct host_raw {
	char name[NAME_LEN];
};

class host {
public:
	static char[32] peer_map; 
private:
	string name;
	string ip;	
};

struct msg_raw {
	int msg_type;
	int data_size;
}

	
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
