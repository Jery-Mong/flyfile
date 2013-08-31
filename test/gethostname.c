#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>


int main()
{
	char tt[32];

	gethostname((char *)&tt, 32);
	printf("%s\n", (char *)&tt);
		
	return 0;
	
}


