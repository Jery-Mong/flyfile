#ifndef __CMD_H
#define __CMD_H

struct cmd {
	char name[8];
	int (*cmd_func)(void *);
};

extern char **cmdstr_to_list(char *);
extern int cmd_handler(char **);


#endif
