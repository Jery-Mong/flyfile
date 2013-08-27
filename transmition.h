#ifndef __TRANSMITION_H
#define __TRANSMITION_H

#define NAME_LEN 128

struct host_raw {
	char name[NAME_LEN];
};

class host {
public:
	static char[32] peer_map; 
private:
	string name;
	string ip;	
};
#endif
