//stat.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(void) {
	int fd, addrlen, ret, newfd;
	int nread, nwritten, nbytes;
	struct sockaddr_in clientaddr;
	struct sockaddr_in addr;
	char *ptrBuffer, bufferReceived[128], bufferSend[128];
	
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		exit(1);
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(58000);
	
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret == -1)
		exit(1);
	
	while(1) {
		addrlen = sizeof(addr);

		nread = recvfrom(fd, bufferReceived, 128, 0, (struct sockaddr*)&clientaddr, &addrlen);
		
		if(nread == -1)
			exit(1);
		
		puts(bufferReceived);
		
		ptrBuffer = strcpy(bufferSend, "OK");
		nbytes = 2;
		
		nwritten = sendto(fd, ptrBuffer, nbytes, 0, (struct sockaddr*)&clientaddr, addrlen);
		
		if(nwritten == -1)
			exit(1);
	}
	
	close(fd);
	exit(0);
}