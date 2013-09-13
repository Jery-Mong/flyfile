#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#include "list.h"
#include "local.h"
#include "common.h"
#include "net.h"
#include "interface.h"
#include "transmit.h"

static int history_online = 0;

void global_init()
{
	self = (struct host *)malloc(sizeof(struct host));
	memset(self, 0, sizeof(struct host));
	self->file_status = FILE_AVAL;
	
	gethostname(self->id.name, 32);

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
		if (id->ip == ((struct peer *)(iter->data))->id.ip)
			return iter->data;
	}
	return NULL;
}

static int __pow_10(int i)
{
	int po = 1;
	while(i--)
		po *= 10;
	return po;
}
static int __atoui(char *pi)
{
	int i;
	int num = 0;
	
	for (i = strlen(pi); i > 0; i--, pi++) {
		if (*pi > ('0' + 9) || *pi < 0) /* not a valid number */
			return -1;
		
		num += (*pi - '0') * __pow_10(i - 1);
	}
	return num;
}

struct peer *getpeerbyidnum(char *cid)
{
	int idnum = __atoui(cid);
	
	if (idnum < 0)
		return NULL;
	
	node_t *iter;	
	for_each_node(iter, peer_list) {
		if (((struct peer *)(iter->data))->idnum == idnum)
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
	pr->idnum = history_online++;
	
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
	struct message r_msg;

	if ((pr = getpeerbyid(&msg->id)) == NULL)
		return;

	if (self->file_status == FILE_BUSY)
		return;
	else
		rsp = popwin_getrsp(msg);
	
	/* send respond msg */
	if (msg->type == MSG_FILE_RQST)
		r_msg.type = MSG_FILE_ACK;
	else
		r_msg.type = MSG_CHAT_ACK;
	memcpy(&r_msg.id, &self->id, sizeof(struct base_inf));
	r_msg.answer = rsp;

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &r_msg, sizeof(struct message), 0);
	close(fd);

	if (rsp == RSP_NO)
		m_printf("You Answer Is [No]\n");
	else
		m_printf("You Answer Is [Yes]\n");
	
	if (msg->type == MSG_CHAT_RQST) {
		pr->chat_rsq_stat = rsp;
		goto out;
	}
	
	fd = getsockfd(FD_DATA_RECV, pr);

	if (fd < 0)
		goto out;
	
	self->file_status = FILE_BUSY;	
	recv_file(fd, &msg->msfile);
	self->file_status = FILE_AVAL;
	close(fd);
out:
	free(msg);
}

void handle_ack(struct message *msg)
{
	struct peer *pr;

	if ((pr = getpeerbyid(&msg->id)) == NULL)
		return;
	
	if (msg->type == MSG_FILE_ACK) {
		pr->file_rsq_stat = msg->answer;
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
	
	print_chat_comment(msg->id.name, pr->idnum, msg->mschat);
out:
	free(msg);
}
