#ifndef __TRANSMIT_H
#define __TRANSMIT_H

#define PACK_SIZE 512

struct file_piece {
	int pi_idx;
	char pi_data[PACK_SIZE];
};

extern void send_file(int, char *);
extern void recv_file(int, void *);

#endif
