//user.c
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#define PORT 58000
#define NG 9
#define BUFFER_SIZE 128

int main(int argc, char **argv){
	int fd, n, nbytes, nwritten, nread;
	struct sockaddr_in addr;
	struct hostent *hostptr;
	char req[BUFFER_SIZE], buffer[BUFFER_SIZE];
	
	char *username = argv[1];
	char *SMBname;
	int SMBport;
	
	if (argc == 1) {
		printf("\nUsername missing...\nCorrect sintaxe: user username [-n SMBname] [-p SMBport]\n");
		return 0;

	} else if (argc == 4) {
	
		if (!strcmp(argv[2], "-p")) {
			SMBport = atoi(argv[3]);
			SMBname = "localhost";
		}
		else if (!strcmp(argv[2], "-n")) {
			SMBname = argv[3];
			SMBport = PORT + NG;
		} else {
			printf("\nArgument not known: %s\n", argv[2]);
			return 0;
		}
	
	} else if(argc == 6) {
		
		if (!strcmp(argv[2], "-n") && !strcmp(argv[4], "-p")) {
			SMBname = argv[3];
			SMBport = atoi(argv[5]);
		}
		else if (!strcmp(argv[2], "-p") && !strcmp(argv[4], "-n")) {
			SMBport = atoi(argv[3]);
			SMBname = argv[5];
		} else {
			printf("\nArgument not known: %s %s\n", argv[2], argv[4]);
			return 0;
		}
	
	} else if (argc == 2) {
		SMBname = "localhost";
		SMBport = PORT + NG;
	}

	else {
		printf("\nCorrect sintaxe: user username [-n SMBname] [-p SMBport]\n");
		return 0;
	}

	printf("name: %s \t port: %d\n", SMBname, SMBport);
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1)
		exit(1);
		
	hostptr = gethostbyname(SMBname);
		
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	addr.sin_port = htons(SMBport);
	
	n = connect(fd,(struct sockaddr*)&addr, sizeof(addr));
	if(n == -1)
		exit(1);
	
	sprintf(req, "REQ %s\n", username);
	nwritten = write(fd, req, strlen(req));
	if(nwritten == -1) {
		fprintf(stderr, "ERROR: Couldn't receive message from SMB...\n");
		exit(1);
	}
	
	memset(buffer, '\0', sizeof(buffer));
	nread = read(fd, buffer, BUFFER_SIZE);
	
	puts(buffer);
	
	close(fd);
	exit(0);
}


/* vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */
