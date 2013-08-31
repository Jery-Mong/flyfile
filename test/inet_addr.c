#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *cip = "192.168.1.2";
	char *tt;
	
	in_addr_t nip ;

	nip = inet_addr(cip);
	printf("%x\n", nip);

	tt = (char *)malloc(32);
	memset(tt, 0, 32);
	
	tt = inet_ntoa(*(struct in_addr*)&nip);
	
	printf("%s\n", tt);
}
