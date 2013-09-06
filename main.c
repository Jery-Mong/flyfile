/**
 * Global Note:
 *
 * 1.the localhost'name is "self"
 * 2.There can be two missions at most at time same time, and they can not are the same.No or only one mission are allowed. 
 * 3.there are two ports, one is for receiving message based on UDP, the other  is for file or chat data transmission based on TCP. 
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
