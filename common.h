#ifndef __COMMON_H
#define __COMMON_H

#include <netinet/in.h>
#include "list.h"

struct base_inf {
	char name[32];
	in_addr_t ip;
};

extern list_t *peer_list;
extern struct host *self;

#endif
