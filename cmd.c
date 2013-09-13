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
int cmd_help(char **);

int cmd_quit(char **cmd)
{
	bcast_offline();
	list_destroy(peer_list);
	winds_quit();
	exit(0);
}

void wait_file_ack(struct peer *pr)
{	
	int i;
	for (i = 10; i > 0; i--) {
		sleep(1);
		m_printf("%d ", i);
		if (pr->file_rsq_stat == RSP_YES)
			return;
	}
	pr->file_rsq_stat = RSP_NO;
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
	struct peer *pr = getpeerbyidnum(cmd[1]);
	if (pr == NULL)
		goto err;

	struct stat st;
	if (stat(cmd[2], &st))
		goto err;

	struct message msg;
	msg.type = MSG_FILE_RQST;
	memcpy(&msg.id, &self->id, sizeof(struct base_inf));

	get_filename(msg.msfile.fname, cmd[2]);
	msg.msfile.fsize = st.st_size;

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &msg, sizeof(msg), 0);
	close(fd);

	wait_file_ack(pr);	/* wait here */
	
	if (pr->file_rsq_stat == RSP_NO)
		goto out;

	fd = getsockfd(FD_DATA_SEND, pr);
	if (fd < 0)
		goto out;

	self->file_status = FILE_BUSY;
	send_file(fd, cmd[2]);
	self->file_status = FILE_AVAL;
	close(fd);

	
	return 1;
err:
	m_printf("Invalid\n");
	return 0;
out:
	m_printf("rejected\n");
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
	memset(&msg, 0, sizeof(struct message));
	msg.type = MSG_CHAT;
	memcpy(&msg.id, &self->id, sizeof(struct base_inf));

	int i;
	for (i = 2; cmd[i] != NULL; i++) {
		strcat(msg.mschat, " ");
		strcat(msg.mschat, cmd[i]);
	}

	int fd = getsockfd(FD_SENDMSG, pr);
	send(fd, &msg, sizeof(msg), 0);
	close(fd);
	return 0;
}
static void wait_chat_ack(struct peer *pr)
{
	int i;
	for (i = 10; i > 0; i--) {
		sleep(1);
		m_printf("%d ", i);
		if (pr->chat_rsq_stat == RSP_YES)
			return;
	}
	pr->chat_rsq_stat = RSP_NO;

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
	
	wait_chat_ack(pr);
	if (pr->chat_rsq_stat == RSP_NO)
		m_printf ("Rejected\n");
	else
		m_printf ("Ok\n");
	
	return 0;
}
struct cmd cmd_list[] = {
	[0] = {
		.name = "send",
		.help_info = "<send idnum file>: sends a file to a peer",
		.cmd_func = cmd_send,
	},[1] = {
		.name = "pp",
		.help_info = "<pp>: prints all peers with which localhost has connected",
		.cmd_func = cmd_printpr,
	},[2] = {
		.name = "ch",
		.help_info = "<ch idnum comment>: send coment to a peer",
		.cmd_func = cmd_chat,
	},[3] = {
		.name = "chrq",
		.help_info = "<chrsq idnum>: ask for chatting with a peer",
		.cmd_func = cmd_chrsq,
	},[4] = {
		.name = "h",
		.help_info = "<help>",
		.cmd_func = cmd_help,
	},[5] = {
		.name = "quit",
		.help_info = "<quit>",
		.cmd_func = cmd_quit,
	}
};

int cmd_help(char **cmd)
{
	int cmd_cnt = sizeof(cmd_list) / sizeof(cmd_list[0]);

	int i;
	for (i = 0; i < cmd_cnt; i++) {
		m_printf("%d. %s%c %s\n", i, cmd_list[i].name, 9, cmd_list[i].help_info);
	}
	return 0;
}
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
	
	char **args_list = (char **)malloc(sizeof(char **) * 16);
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
	if (cmd == NULL)
		return;
	
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
