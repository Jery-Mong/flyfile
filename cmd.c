#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "interface.h"
#include "common.h"
#include "list.h"
#include "net.h"
#include "cmd.h"

int cmd_quit(void **data)
{
	bcast_offline();
	list_destroy(peer_list);
	winds_quit();
	exit(0);
}

int wait_file_ack(struct peer *pr)
{
	m_printf("\n");
	for (i = 5; i > 0; i--) {
		m_printf("%d\t", i);
		if (pr->rsq_stat == 3) {
			pr->rsq_stat = 0;
			return 1;
		} else if (pr->rsq_stat == 2) {
			pr->rsq_stat = 0;
			return 0;
		}
		sleep(1);
	}
	return 0;
}

int cmd_send(void **data)
{
	char **cmd = (char **)data;

	if (cmd[1] == NULL)
		goto err1;
		
	struct *pr = getpeerbyidnum(*cmd[1]);
	if (pr == NULL)
		goto err1;

	struct stat st;
	if (!stat(cmd[2], &st))
		goto err1;

	struct message msg;
	msg.type = MSG_FILE_RQST;
	memcpy(&msg.id, &self->id, sizeof(struct base_inf));

	strcpy(msg.ms_file.fname, cmd[2]);
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
err:
	m_printf("Invalid");
	return 0;
out:
	m_printf("rejected");
	return -1;
}

int cmd_printpr(void **data)
{
	node_t *iter;
	int i = 0;
	
	for_each_node(iter, peer_list) {
		struct peer *pr = (struct peer *)(iter->data);	
		m_printf("%d. %s : %s\n", i, pr->id.name, inet_ntoa(*((struct in_addr *)&(pr->id.ip))));
		i++;
	}
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
