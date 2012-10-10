//smb.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define SMBPORT 58000
#define STATPORT 59000
#define NG 9
#define BUFFER_SIZE 128

void getArgs(int argc, char **argv, int *SMBport, char **STATname, int *STATport) {
	
	if (argc == 1) {
		*SMBport = SMBPORT + NG;
		*STATname = "localhost";
		*STATport = STATPORT;

	} else if (argc == 3) {
	
		if (!strcmp(argv[1], "-p")) {
			*SMBport = atoi(argv[2]);
			*STATname = "localhost";
			*STATport = STATPORT;
		}
		else if (!strcmp(argv[1], "-n")) {
			*STATname = argv[2];
			*SMBport = SMBPORT + NG;
			*STATport = STATPORT;
		}
		else if (!strcmp(argv[1], "-t")) {
			*STATport = atoi(argv[2]);
			*SMBport = SMBPORT + NG;
			*STATname = "localhost";
		} else {
			printf("\nArgument not known: %s\n", argv[1]);
			exit(1);
		}
	
	} else if(argc == 5) {
		
		if (!strcmp(argv[1], "-n") && !strcmp(argv[3], "-p")) {
			*STATname = argv[2];
			*SMBport = atoi(argv[4]);
			*STATport = STATPORT;
		}
		else if (!strcmp(argv[1], "-p") && !strcmp(argv[3], "-n")) {
			*SMBport = atoi(argv[2]);
			*STATname = argv[4];
			*STATport = STATPORT;
		}
		else if (!strcmp(argv[1], "-p") && !strcmp(argv[3], "-t")) {
			*SMBport = atoi(argv[2]);
			*STATport = atoi(argv[4]);
			*STATname = "localhost";
		}
		else if (!strcmp(argv[1], "-t") && !strcmp(argv[3], "-p")) {
			*STATport = atoi(argv[2]);
			*SMBport = atoi(argv[4]);
			*STATname = "localhost";
		}
		else if (!strcmp(argv[1], "-t") && !strcmp(argv[3], "-n")) {
			*STATport = atoi(argv[2]);
			*STATname = argv[4];
			*SMBport = SMBPORT + NG;
		}
		else if (!strcmp(argv[1], "-n") && !strcmp(argv[3], "-t")) {
			*STATname = argv[2];
			*STATport = atoi(argv[4]);
			*SMBport = SMBPORT + NG;
		} else {
			printf("\nArgument not known: %s %s\n", argv[2], argv[4]);
			exit(1);
		}
	
	} else if (argc == 7) {
		
		if (!strcmp(argv[1], "-p") && !strcmp(argv[3], "-n") && !strcmp(argv[5], "-t")) {
			*SMBport = atoi(argv[2]);
			*STATname = argv[4];
			*STATport = atoi(argv[6]);
		}
		else if (!strcmp(argv[1], "-n") && !strcmp(argv[3], "-p") && !strcmp(argv[5], "-t")) {
			*STATname = argv[2];
			*SMBport = atoi(argv[4]);
			*STATport = atoi(argv[6]);
		}
		else if (!strcmp(argv[1], "-p") && !strcmp(argv[3], "-t") && !strcmp(argv[5], "-n")) {
			*SMBport = atoi(argv[2]);
			*STATport = atoi(argv[4]);
			*STATname = argv[6];
		}
		else if (!strcmp(argv[1], "-t") && !strcmp(argv[3], "-p") && !strcmp(argv[5], "-n")) {
			*STATport = atoi(argv[2]);
			*SMBport = atoi(argv[4]);
			*STATname = argv[6];
		}
		else if (!strcmp(argv[1], "-t") && !strcmp(argv[3], "-n") && !strcmp(argv[5], "-p")) {
			*STATport = atoi(argv[6]);
			*STATname = argv[2];
			*SMBport = atoi(argv[4]);
		} else {
			printf("\nArgument not known: %s %s\n", argv[2], argv[4]);
			exit(1);
		}

	}

	else {
		printf("\nCorrect sintaxe: SMB [-p SMBport] [-n *STATname] [-t STATport]\n");
		exit(1);
	}

}

char *read_file(int line_num, char *user_file_message)
{
	static const char filename[] = "file";
    	FILE *file = fopen ( filename, "r" );
    	int rnd, temp;
    //	char user_file_message[BUFFER_SIZE];
  	if ( file != NULL ) {
  		/* initialize random seed: */
		srand ( time(NULL) );

		/* generate secret number: */
		rnd = rand() % line_num + 1;
		temp = 1;
    		char line [ BUFFER_SIZE ]; /* or other suitable maximum line size */
    		while ( fgets ( line, sizeof line, file ) != NULL ) { /* read a line */
    			if (temp == rnd) {
    				strcpy (user_file_message, line);
    				break;
    			} else
    				temp++;
    		}
    		fclose ( file );
    	} else {
    		perror ( filename ); /* why didn't the file open? */
    	}
    	return user_file_message;
}

/* Conta o numero de linhas do ficheiro de resposta */
int count_lines() 
{
	int lines = 0;
	static const char filename[] = "file";
    	FILE *file = fopen ( filename, "r" );
   	if ( file != NULL ) {
   		char line [ BUFFER_SIZE ];
		while ( fgets ( line, sizeof line, file ) != NULL )
			lines++;
	}
	close ( file );
	return lines;
}

int main(int argc, char **argv) {
	int fd, addrlen, ret, newfd;
	int fdStat, nStat, nreadStat;
	int addrStatlen;
	struct sockaddr_in addrStat;
	int nread, nwritten, nbytes;
	struct sockaddr_in addr;
	char *ptr, buffer[BUFFER_SIZE], buffer2[BUFFER_SIZE];
	struct hostent *hostptr;
	int SMBport, STATport;
	char *STATname;
	char *userTokens;
	char reg[BUFFER_SIZE], udp[BUFFER_SIZE];
	struct in_addr* ip;
	char hostname[BUFFER_SIZE];
	int line_num;
	char user_file_message[BUFFER_SIZE];

	line_num = count_lines();

	getArgs(argc, argv, &SMBport, &STATname, &STATport);
	
	fdStat = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(fdStat == -1) {
		fprintf(stderr, "ERROR: Couldn't create de UDP socket...\n");
		exit(1);
	}
	
	hostptr = gethostbyname(STATname);
	
	memset((void*)&addrStat, (int)'\0', sizeof(addrStat));
	addrStat.sin_family = AF_INET;
	addrStat.sin_addr.s_addr = ((struct in_addr*)(hostptr->h_addr_list[0]))->s_addr;
	addrStat.sin_port = htons(STATport);
	
	if(gethostname(hostname, BUFFER_SIZE)==-1) {
		fprintf(stderr, "ERROR: Couldn't send message to STAT...\n");
	    exit(1);
	}
	hostptr = gethostbyname(hostname);
	addrStatlen = sizeof(addrStat);
	ip = (struct in_addr*)(hostptr->h_addr_list[1]);
	if(ip == NULL) ip = (struct in_addr*)(hostptr->h_addr_list[0]);
	sprintf(reg, "REG %s %d\n", inet_ntoa(*ip), SMBport);
	nStat = sendto(fdStat, reg, strlen(reg), 0, (struct sockaddr*)&addrStat, addrStatlen);
	
	if(nStat == -1) {
		fprintf(stderr, "ERROR: Couldn't send message to STAT...\n");
		exit(1);
	}
	
	nreadStat = recvfrom(fdStat, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addrStat, &addrStatlen);
	
	if(nreadStat == -1) {
		fprintf(stderr, "ERROR: Couldn't receive message from STAT...\n");
		exit(1);
	}
	
	printf("%s", buffer);
	
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "ERROR: Couldn't create de TCP socket...\n");
		exit(1);
	}
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SMBport);
	
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret == -1) {
		fprintf(stderr, "ERROR: Couldn't bind socket...\n");
		exit(1);
	}
	
	if(listen(fd,5) == -1) {
		fprintf(stderr, "ERROR: Couldn't listen...\n");
		exit(1);
	}
	
	while(1) {
		addrlen = sizeof(addr);
		
		if((newfd = accept(fd, (struct sockaddr*)&addr, &addrlen)) == -1) {
			fprintf(stderr, "ERROR: Couldn't accept TCP connection...\n");
			exit(1);
		}

		nread = read(newfd, buffer, BUFFER_SIZE);
		
		if(nread == -1) {
			fprintf(stderr, "ERROR: Couldn't read message from User...\n");
			exit(1);
		}
		
		printf("%s", buffer);
		
		strcpy(user_file_message, read_file(line_num, user_file_message));
		nwritten = write(newfd, user_file_message, strlen(user_file_message));
		if(nwritten == -1) {
			fprintf(stderr, "ERROR: Couldn't send message to user...\n");
			exit(1);
		}
		
		userTokens = strtok(buffer, " ");
		if(strcmp(userTokens, "REQ") == 0) {
			userTokens = strtok(NULL, " ");
			if(userTokens != NULL) {
				sprintf(reg, "UDP %s %d %s\n", inet_ntoa(*ip), SMBport, userTokens);
				nStat = sendto(fdStat, reg, strlen(reg), 0, (struct sockaddr*)&addrStat, addrStatlen);
				
				if(nStat == -1) {
					fprintf(stderr, "ERROR: Couldn't send message to STAT...\n");
					exit(1);
				}
				printf("Sent msg to stat\n");
				
				memset(buffer2, '\0', sizeof(buffer2));
				nreadStat = recvfrom(fdStat, buffer2, BUFFER_SIZE, 0, (struct sockaddr*)&addrStat, &addrStatlen);

				if(nreadStat == -1) {
					fprintf(stderr, "ERROR: Couldn't receive message from STAT...\n");
					exit(1);
				}

				printf("%s", buffer2);
					
			} else {
				fprintf(stderr, "ERROR: message unknown...\n");
				ptr = strcpy(buffer, "KO\n");
				nwritten = write(newfd, ptr, strlen(ptr));
				if(nwritten == -1) {
					fprintf(stderr, "ERROR: Couldn't send message to user...\n");
					exit(1);
				}
			}
			
		} else {
			fprintf(stderr, "ERROR: message unknown...\n");
			ptr = strcpy(buffer, "KO\n");
			nwritten = write(newfd, ptr, strlen(ptr));
			if(nwritten == -1)
				exit(1);
		}
		
		close(newfd);
		
		int i;
		for(i = 0; i < BUFFER_SIZE; i++)
			buffer[i] = '\0';
	}
	
	close(fd);
	exit(0);
}
