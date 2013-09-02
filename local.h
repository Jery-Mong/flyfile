#ifndef __LOCAL_H
#define __LOCAL_H

#include <pthread.h>

#include "message.h"

enum mission_type{
	FILE_TRANS,
	CHATTING,
};
enum mission_status {
	DEAD,
	ALIVE,
	
};
struct mission {
	int type;
	pthread_t tid;
	int status;
};


struct host {
	struct base_inf id;
	pthread_mutex_t lock;
	struct mission *cur_misn[2];
	
};
#define fimisn cur_misn[0]
#define chmisn cur_misn[1]

struct peer {
	struct base_inf id;
	int rqst_file_stat;
	int rqst_chat_stat;
	int fd;
};

void global_init();
struct peer *getpeerbyid(struct base_inf *);
struct peer *peer_inlist(struct message *);
void peer_outlist(struct message *);
void respond_rqst(struct message *);
void handle_answer(struct message *);
void create_mission(int);

#endif
