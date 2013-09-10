#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "interface.h"
#include "common.h"
#include "list.h"
#include "net.h"
#include "cmd.h"
#include "transmit.h"
#include "local.h"

int cmd_quit(char **cmd)
{
	bcast_offline();
	list_destroy(peer_list);
	winds_quit();
	exit(0);
}

int wait_file_ack(struct peer *pr)
{
	m_printf("\n");
	
	int i;
	for (i = 5; i > 0; i--) {
		sleep(1);
		m_printf("%d\t", i);
		if (pr->file_rsq_stat == 3) {
			pr->file_rsq_stat = 0;
			return RSP_YES;
		} else if (pr->file_rsq_stat == 2) {
			pr->file_rsq_stat = 0;
			return RSP_NO;
		}
	}
	return RSP_NO;
}


void get_filename(char *buf, char *path)
{
	char *p;
	p = strrchr(path, '/');
	
	if (p == NULL)
		strcpy(buf, path);
	else 
		strcpy(buf, p + 1);
}
int cmd_send(char **cmd)
{
	if (cmd[1] == NULL)
		goto err;
		
	struct peer *pr = getpeerbyidnum(cmd[1]);
	if (pr == NULL)
		goto err;

	struct stat st;
	if (!stat(cmd[2], &st))
		goto err;

	struct message msg;
	msg.type = MSG_FILE_RQST;
	memcpy(&msg.id, &self->id, sizeof(struct base_inf));


	get_filename(msg.msfile.fname, cmd[2]);
	//strcpy(msg.msfile.fname, cmd[2]);
	msg.msfile.fsize = st.st_size;

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &msg, sizeof(msg), 0);
	close(fd);

	if (wait_file_ack(pr) == RSP_NO)
		goto out;

	fd = getsockfd(FD_DATA_SEND, pr);

	if (fd < 0)
		goto out;

	self->file_status = FILE_BUSY;
	send_file(fd, cmd[2]);
	self->file_status = FILE_AVAL;

	return 1;
err:
	m_printf("Invalid");
	return 0;
out:
	m_printf("rejected");
	return 0;
}

int cmd_printpr(char **cmd)
{
	node_t *iter;
	
	for_each_node(iter, peer_list) {
		struct peer *pr = (struct peer *)(iter->data);	
		m_printf("%d. %s : %s\n", pr->idnum, pr->id.name, inet_ntoa(*((struct in_addr *)&(pr->id.ip))));
	}
	return 0;
}
int cmd_chat(char **cmd)
{

	struct peer *pr = getpeerbyidnum(cmd[1]);
	
	if (pr == NULL) {
		m_printf ("No Such Peer\n");
		return -1;
	}
	
	if (pr->chat_rsq_stat == RSP_NO) {
		m_printf ("chat with %s is not allowed\n", pr->id.name);
		return -1;
	}
	if (cmd[2] == NULL) {
		m_printf ("No comment\n");
		return -1;
	}

	struct message msg;
	msg.type = MSG_CHAT;
	memcpy(&msg.id, &self->id, sizeof(struct base_inf));
	strcpy(msg.mschat, cmd[2]);

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &msg, sizeof(msg), 0);
	close(fd);
	return 0;
}
int wait_chat_ack(struct peer *pr)
{
	int i;

	for (i = 3; i > 0; i--) {
		sleep(1);
		m_printf ("%d\t", i);
	}

	return pr->chat_rsq_stat;
}


int cmd_chrsq(char **cmd)
{
	struct peer *pr = getpeerbyidnum(cmd[1]);
	
	if (pr == NULL) {
		m_printf ("No Such Peer\n");
		return 0;
	}

	struct message msg;
	msg.type = MSG_CHAT_RQST;
	memcpy(&msg.id, &self->id, sizeof(struct base_inf));

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &msg, sizeof(msg), 0);
	close(fd);
	
	if (wait_chat_ack(pr) == RSP_NO)
		m_printf ("Rejected\n");
	else
		m_printf ("Ok\n");
	return 0;
}

struct cmd cmd_list[] = {
	[0] = {
		.name = "quit",
		.cmd_func = cmd_quit,
	},[1] = {
		.name = "send",
		.cmd_func = cmd_send,
	},[2] = {
		.name = "pp",
		.cmd_func = cmd_printpr,
	},[3] = {
		.name = "ch",
		.cmd_func = cmd_chat,
	},[4] = {
		.name = "chrsq",
		.cmd_func = cmd_chrsq,
	}
};

/* the command is blankspace-terminated */
static int is_cmd(const char *cmd)
{
	int i, j;
	for (i = 0; cmd[i] != ' ' && cmd[i] != '\0'; i++);

	for (j = 0; j < sizeof(cmd_list) / sizeof(cmd_list[0]); j++) {
		if (!strncmp(cmd, cmd_list[j].name, i))
			return 1; 
	}
	
	return 0;
}

char **cmdstr_to_list(char *raw_cmd)
{
	int i;
	for (i = 0; raw_cmd[i] == ' '; i++);
	raw_cmd += i;
	
	if (*raw_cmd == '\0')
		goto out2;
	
	char **args_list = (char **)malloc(sizeof(char **) * 8);
	memset(args_list, 0, sizeof(char **) * 8);
	
	if (!is_cmd(raw_cmd))	/* the command is blankspace-terminated */
		goto out1; 
	int k = 0;
	for (;;) {
		for (i = 0; raw_cmd[i] != ' ' && raw_cmd[i] != '\0'; i++);
		
		args_list[k] = (char *)malloc(i + 1);
		strncpy(args_list[k], raw_cmd, i);
		args_list[k][i] = '\0';
		
		raw_cmd += i;
		for (; *raw_cmd == ' '; raw_cmd++);
		
		if (*raw_cmd == '\0')
			return args_list;
		k++;
	}
out1:
	free(args_list);
out2:
	return NULL;
}

static void free_cmd(char **cmd)
{
	int i;
	for (i = 0; cmd[i] != NULL; i++)
		free(cmd[i]);
}
int cmd_handler(char **cmd)
{
	int i;
	for (i = 0; i < sizeof(cmd_list) / sizeof(cmd_list[0]); i++)
		if (!strcmp(cmd[0], cmd_list[i].name)) {
			cmd_list[i].cmd_func(cmd);
			free_cmd(cmd);
			return 0;
		}

	return -1;
}
