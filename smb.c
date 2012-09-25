//smb.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int main(void) {
	int fd, addrlen, ret, newfd;
	int fdStat, nStat, nreadStat;
	int addrStatlen;
	struct sockaddr_in addrStat;
	int nread, nwritten, nbytes;
	struct sockaddr_in addr;
	char *ptr, buffer[128];
	struct hostent *hostptr;
	
	fdStat = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(fdStat == -1) {
		perror("Couldn't create de UDP socket.");
		exit(1);
	}
	
	hostptr = gethostbyname("localhost");
	
	memset((void*)&addrStat, (int)'\0', sizeof(addrStat));
	addrStat.sin_family = AF_INET;
	addrStat.sin_addr.s_addr = ((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	addrStat.sin_port = htons(58000);
	
	addrStatlen = sizeof(addrStat);
	nStat = sendto(fdStat, "Hello!", 6, 0, (struct sockaddr*)&addrStat, addrStatlen);
	
	if(nStat == -1) {
		perror("Couldn't send message to STAT");
		exit(1);
	}
	
	nreadStat = recvfrom(fdStat, buffer, 128, 0, (struct sockaddr*)&addrStat, &addrStatlen);
	
	if(nreadStat == -1) {
		perror("Couldn't receive message from STAT");
		exit(1);
	}
	
	puts(buffer);
	
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Couldn't create de TCP socket.");
		exit(1);
	}
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(59000);
	
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret == -1)
		exit(1);
	
	if(listen(fd,5) == -1)
		exit(1);
	
	while(1) {
		addrlen = sizeof(addr);
		
		if((newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) == -1)
			exit(1);

		nread = read(newfd, buffer, 128);
		
		if(nread == -1)
			exit(1);
		
		puts(buffer);
		
		ptr = strcpy(buffer, "pong");
		nbytes = 4;
		
		nwritten = write(newfd, ptr, nbytes);
		
		if(nwritten == -1)
			exit(1);
		
		close(newfd);
	}
	
	close(fd);
	exit(0);
}