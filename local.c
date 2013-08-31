

list_t *peer_list;		/* hosts alive in LAN */
struct host *self;


void global_init()
{
	self = (struct host *)malloc(sizeof(struct host));
	memset(self, 0, sizeof(struct host));
	int name[32];
	gethostname()
	

	peer_list = (list_t *)malloc(sizeof(list_t));
	list_init(peer_list);

	interface_init();
}
