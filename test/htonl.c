#include <arpa/inet.h>
#include <stdio.h>

int main()
{
	unsigned long i = 0x12345678;
	unsigned long j = htonl(i);

	printf("%x\n", i);
	printf("%x\n", j);
	
	return 0;
}


