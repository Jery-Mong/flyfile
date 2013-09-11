/**
 * Global Note:
 *
 * 1.the localhost'name is "self"
 * 2.There can be only one file mission, while chat mission doesn't have limitation.
 * 3.there are two ports, one is for receiving message based on UDP, the other  is for file transmission based on TCP. 
 */
#include <pthread.h>
#include <unistd.h>

#include "net.h"
#include "local.h"
#include "interface.h"

list_t *peer_list;
struct host *self;

int main()
{
	pthread_t tid1;
	
	global_init();
	bcast_online();

	pthread_create(&tid1, NULL, recv_msg, NULL);
	while(1) {
		main_wind();
	}
}
