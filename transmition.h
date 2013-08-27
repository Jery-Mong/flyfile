#ifndef __TRANSMITION_H
#define __TRANSMITION_H

#define NAME_LEN

enum message_type {
	MSG_FILE_BEGIN,
	MSG_FILE_END,
	MSG_COMFIRM,
	MSG_ONLINE,
	MSG_OFFLINE,
	MSG_CHAT,
};
struct host_raw {
	char name[NAME_LEN];
};

class host {
public:
	
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
private:
	int msg_type;
	struct msg_raw *netpack;
	void *msg_data;
};

#endif
