

list_t *peer_list;		/* hosts alive in LAN */
struct host *self;


void global_init()
{
	self = (struct host *)malloc(sizeof(struct host));
	memset(self, 0, sizeof(struct host));
	
	gethostname(self->id.name, 32);
	self->id.ip = get_local_ip();

	if (!self->id.ip) {
		printf("error: you seem to have not connected to LAN please check you network\n");
		exit(0);	
	}

	peer_list = (list_t *)malloc(sizeof(list_t));
	list_init(peer_list);

	interface_init();
}
