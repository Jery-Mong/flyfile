#ifndef __LOCAL_H
#define __LOCAL_H

struct base_inf {
	char name[32];
	in_addr_t ip;
};

struct rqst_inf {
	struct peer *begger;
	int rqst_type;
};
struct host {
	struct base_inf s_id;
	struct rqst_inf last_rqst;
	
};

struct peer {
	struct base_inf p_id;
	int rqst_file_stat;
	int rqst_chat_stat;
	
	int fd;
};

#endif
