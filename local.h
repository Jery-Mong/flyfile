#ifndef __LOCAL_H
#define __LOCAL_H

#include <pthread.h>

#include "message.h"

struct host {
	struct base_inf id;
	in_addr_t bcastaddr;
	int file_status;
	
};

struct peer {
	int idnum;
	struct base_inf id;
	volatile int file_rsq_stat;
	volatile int chat_rsq_stat;
};

void global_init();
struct peer *getpeerbyid(struct base_inf *);
struct peer *getpeerbyidnum(char*);
struct peer *peer_inlist(struct message *);
void peer_outlist(struct message *);
void respond_rqst(struct message *);
void handle_ack(struct message *);
void chat_get_comment(struct message *); 

#endif
