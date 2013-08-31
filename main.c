


int main()
{
	pthread_t tid1;
	pthread_t tid2;
	
	global_init();
	bcast_online();

	pthread_create(&tid1, NULL, recv_msg, NULL);
	pthread_create(&tid2, NULL, get_input, NULL);

	while(1) {
		pause();
	}
}
