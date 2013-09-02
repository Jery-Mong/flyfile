#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "list.h"
#include "local.h"
#include "common.h"
#include "net.h"
//#include "interface.h"

void global_init()
{
	self = (struct host *)malloc(sizeof(struct host));
	memset(self, 0, sizeof(struct host));
	
	gethostname(self->id.name, 32);
	self->id.ip = get_local_ip();
	

	if (!self->id.ip) {
		printf("error: you seem to have not connected to LAN please check you network\n");
		exit(-1);	
	}
	self->chmisn = NULL;
	self->fimisn = NULL;

	peer_list = (list_t *)malloc(sizeof(list_t));
	list_init(peer_list);

	//interface_init();
}

struct peer *getpeerbyid(struct base_inf *id)
{
	node_t *iter;
	for_each_node(iter, peer_list) {
		if (!memcmp(id, iter->data, sizeof(struct base_inf)))
			return iter->data;
	}
	return NULL;
}


struct peer *peer_inlist(struct message *msg)
{
	if (msg->type != MSG_PEER_INF)
		return NULL;
	
	struct peer *pr;

	if ((pr = getpeerbyid(&msg->m_id)) != NULL) /* the peer is already in the peer_list */
		return pr;
	
	pr = (struct peer *)malloc(sizeof(struct peer));
	memset(pr, 0, sizeof(struct peer));
	
	list_insert_tail(peer_list, pr);

	free(msg);
	return pr;
}

void peer_outlist(struct message *msg)
{
	struct peer *pr = getpeerbyid(&msg->m_id);

	if (pr == NULL)
		return;
	
	list_del_node(peer_list, pr);
}
#if 0
void respond_rqst(struct message *msg)
{
	struct peer *pr;
	int rsp;

	if ((pr = getpeerbyid(&msg->m_id)) == NULL)
		return;

	/* if there is already the same as request type mission,
	 * auto say no to request peer
	*/
	if ((msg->type == MSG_FILE_RQST && self->fimisn != NULL) ||
	    (msg->type == MSG_CHAT_RQST && self->chmisn != NULL)) {
		rsp = RSP_NO;
	} else {
		rsp = popwind_getrsp(msg);
		if (msg->type == MSG_FILE_RQST)
			pr->rqst_file_stat = rsp;
		else
			pr->rqst_chat_stat = rsp;
	}
	

	memset(msg, 0, sizeof(struct message));
	
	if (msg->type == MSG_FILE_RQST)
		msg->type = MSG_FILE_ACK;
	else
		msg->type = MSG_CHAT_ACK;
	
	msg->answer = rsp;

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &msg, sizeof(struct message), 0);

	shutdown(fd, SHUT_RDWR);
	free(msg);
}
#endif

void handle_answer(struct message *msg)
{
	struct mission *misn;
	
	if (msg->type == MSG_CHAT_ACK)
		misn = self->chmisn;
	else
		misn = self->fimisn;
	
	if (msg->answer == RSP_NO)
		misn->status = DEAD;
	else
		misn->status = ALIVE;
}

void create_mission(int type)
{
	struct mission *misn = (struct mission *)malloc(sizeof(struct mission));
	memset(misn, 0, sizeof(struct mission));

	misn->type = type;
	if (type == FILE_TRANS) {
		self->fimisn = misn; 
	} else
		self->chmisn = misn;
}
