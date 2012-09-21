//user.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int main(void){
	int fd, n, nbytes, nwritten, nread;
	struct sockaddr_in addr;
	struct hostent *hostptr;
	char *ptr, buffer[128];
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1)
		exit(1);
		
	hostptr = gethostbyname("localhost");
		
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	addr.sin_port = htons(59000);
	
	n = connect(fd,(struct sockaddr*)&addr, sizeof(addr));
	if(n == -1)
		exit(1);
	
	ptr = strcpy(buffer, "ping");
	nbytes = 4;
	
	nwritten = write(fd, ptr, nbytes);
	if(nwritten == -1)
		exit(1);
		
	nread = read(fd, buffer, 128);
	
	puts(buffer);
	
	close(fd);
	exit(0);
}
