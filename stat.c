//stat.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 59000

int main(int argc, char **argv) {
	int fd, addrlen, ret, newfd;
	int nread, nwritten, nbytes;
	struct sockaddr_in clientaddr;
	struct sockaddr_in addr;
	char *ptrBuffer, bufferReceived[128], bufferSend[128];

	// obter argumento se houver
	int STATport;

	if (argc == 1) {
		STATport = PORT;
	}
	else if (argc == 3) {
		if (!strcmp(argv[1], "-t")) {
			STATport = atoi(argv[2]);
		}
		else {
			printf("\nCorrect sintaxe: STAT [-t STATport]\n");
		}
	}
	else {
		printf("\nCorrect sintaxe: STAT [-t STATport]\n");
		return 0;
	}
	
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		exit(1);
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(STATport);
	
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

/* vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */
