#ifndef __CMD_H
#define __CMD_H

struct cmd {
	char *name;
	char *help_info;
	int (*cmd_func)(char **);
};

extern char **cmdstr_to_list(char *);
extern int cmd_handler(char **);


#endif
