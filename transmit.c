#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "interface.h"
#include "transmit.h"

#define PIECE_SIZE (PACK_SIZE + sizeof(unsigned int))

void send_file(int sockfd, char *path)
{
	int pack_cnt, i;
	unsigned int fsize;
	int fd;
	struct stat st;
	struct file_piece piece;
	int percent = 0;
	pthread_t tid;
	int rnbytes;
	
	if ((fd  = open(path, O_RDONLY)) < 0)
		return;

	stat(path, &st);
	fsize = st.st_size;

	if (fsize % PACK_SIZE)
		pack_cnt = fsize / PACK_SIZE;
	else
		pack_cnt = fsize / PACK_SIZE + 1;

	pthread_create(&tid, NULL, show_progress, &percent);
	
	for (i = 0; i < pack_cnt; i++) {
		piece.pi_idx = i;
		rnbytes = read(fd, &piece.pi_data, PACK_SIZE);
		write(sockfd, &piece, rnbytes + sizeof(unsigned int));
		
		percent = (int)(((float)i / pack_cnt) * 100);
	}
}
void recv_file(int sockfd, void *data)
{
	pthread_t tid;
	struct file_inf *finf = (struct file_inf *)data;
	int fd;
	int pack_cnt, i;
	struct file_piece piece;
	int rnbytes;
	int percent = 0;
	
	chdir(getenv("HOME"));
		
	fd = open(finf->fname, O_CREAT | O_WRONLY, 0644);
	
	if (finf->fsize % PACK_SIZE)
		pack_cnt = finf->fsize / PACK_SIZE;
	else
		pack_cnt = finf->fsize / PACK_SIZE + 1;

	pthread_create(&tid, NULL, show_progress, &percent);
	
	for (i = 0; i < pack_cnt; i++) {
		rnbytes = read(sockfd, &piece, PIECE_SIZE);
		lseek(fd, piece.pi_idx * PACK_SIZE, SEEK_SET);
		write(fd, &piece.pi_data, rnbytes - sizeof(unsigned int));
		
		percent = (int)(((float)i / pack_cnt) * 100);
	}
}
