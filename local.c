#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "list.h"
#include "local.h"
#include "common.h"
#include "net.h"
#include "interface.h"
#include "transmit.h"

static int his_online = 0;

void global_init()
{
	self = (struct host *)malloc(sizeof(struct host));
	memset(self, 0, sizeof(struct host));
	
	gethostname(self->id.name, 32);
//	self->id.ip = get_local_ip();
	/* get local ip and broadcast ip */
	get_local_ipinf(&(self->id.ip), &self->bcastaddr);
	
	if (!self->id.ip) {
		printf("error: you don't seem to have connected to LAN, please check your network\n");
		exit(1);	
	}

	peer_list = (list_t *)malloc(sizeof(list_t));
	list_init(peer_list);

	winds_init();
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

struct peer *getpeerbyidnum(char *idnum)
{
	node_t *iter;
	
	for_each_node(iter, peer_list) {
		if (((struct peer *)(iter->data))->idnum == atoi(idnum))
			return iter->data;
	}
	return NULL;
}

struct peer *peer_inlist(struct message *msg)
{	
	struct peer *pr;

	if ((pr = getpeerbyid(&msg->id)) != NULL) /* the peer is already in the peer_list */
		return pr;

	
	pr = (struct peer *)malloc(sizeof(struct peer));
	memset(pr, 0, sizeof(struct peer));
	memcpy(&pr->id, &msg->id, sizeof(struct base_inf));
	pr->idnum = his_online++;
	
	list_insert_tail(peer_list, pr);

	free(msg);
	return pr;
}

void peer_outlist(struct message *msg)
{
	struct peer *pr = getpeerbyid(&msg->id);

	if (pr == NULL)
		return;
	list_del_node(peer_list, pr);
	
	free(pr);
}

void respond_rqst(struct message *msg)
{
	struct peer *pr;
	int rsp;

	if ((pr = getpeerbyid(&msg->id)) == NULL)
		return;

	rsp = popwin_getrsp(msg);	

	memset(msg, 0, sizeof(struct message));
	
	if (msg->type == MSG_FILE_RQST)
		msg->type = MSG_FILE_ACK;
	else
		msg->type = MSG_CHAT_ACK;
	
	msg->answer = rsp;

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &msg, sizeof(struct message), 0);
	close(fd);
	free(msg);

	if (msg->type == MSG_CHAT_RQST) {
		pr->chat_rsq_stat = rsp;
		return;
	}
	if (rsp == RSP_NO) {
		m_printf("\nreject\n");
		return;
	}


	self->file_status = FILE_BUSY;
	
	fd = getsockfd(FD_DATA_RECV, pr);
	recv_file(fd, &msg->msfile);
	
	self->file_status = FILE_AVAL;	
}

void handle_ack(struct message *msg)
{
	struct peer *pr;

	if ((pr = getpeerbyid(&msg->id)) == NULL)
		return;
	
	if (msg->type == MSG_FILE_ACK) {
		if (msg->answer == RSP_YES)
			pr->file_rsq_stat = 3; /* 11 */
		else
			pr->file_rsq_stat = 2; /* 10 */
	} else {
		pr->chat_rsq_stat = msg->answer;
	}
}


void chat_get_comment(struct message *msg)
{
	struct peer *pr;

	if ((pr = getpeerbyid(&msg->id)) == NULL)
		goto out;

	if (pr->chat_rsq_stat == RSP_NO)
		goto out;
	
	print_chat_comment(msg->mschat);

out:
	free(msg);
}
