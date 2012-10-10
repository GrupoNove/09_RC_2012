//stat.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 59000
#define BUFFER_SIZE 128

/* store data structure */
typedef struct server {
	int count;
	char *id;
} server;

typedef struct user {
	int count;
	char *id;
} user;

//implementar a estrutura de dados para armazenar as palavras independetemente e contá-las
typedef struct list {
	void *obj;
	struct list *next;
} list;

//roundrobin de mensagens
list *buildMsgs() {
	list *msgs, *aux, *first;
	char *msg;
	
	msgs = (list*)malloc(sizeof(list));
	msg = (char*)malloc(sizeof(char)*5);
	strcpy(msg, "Olá!\n");
	msgs->obj = (void*)msg;
	first = aux = msgs;

	msgs = (list*)malloc(sizeof(list));
	msg = (char*)malloc(sizeof(char)*10);
	strcpy(msg, "Benvindo!\n");
	msgs->obj = (void*)msg;
	aux->next = msgs;
	aux = msgs;

	msgs = (list*)malloc(sizeof(list));
	msg = (char*)malloc(sizeof(char)*20);
	strcpy(msg, "Welcome dear user!\n");
	msgs->obj = (void*)msg;
	aux->next = msgs;
	aux = msgs;

	aux->next = first;

	return msgs;
}


//build lists
list *buildServerList() {
	list *serverList;

	serverList = (list*)malloc(sizeof(list));
	memset(serverList, '\0', sizeof(list));
	serverList->obj = NULL;
	serverList->next = NULL;

	return serverList;
}
list *buildUserList() {
	list *userList;

	userList = (list*)malloc(sizeof(list));
	memset(userList, '\0', sizeof(list));
	userList->obj = NULL;
	userList->next = NULL;

	return userList;
}

// test if server is already registered, else add it to list
int addServer(list *servers, char *message) {
	list *nova;
	server *s;
	list *aux, *auxOld;

	if(servers->obj==NULL) {
		s = (server*)malloc(sizeof(server));
		memset(s, '\0', sizeof(server));
		s->id = message;
		s->count = 1;
		servers->obj = (void*)s;

		return 1;
	}

	for(aux=servers; aux != NULL; aux=aux->next) {
		auxOld = aux;
		if(strcmp(message, ((server*)(aux->obj))->id)==0)
			return ++(((server*)(aux->obj))->count);
	}

	nova = (list*)malloc(sizeof(list));
	memset(nova, '\0', sizeof(list));
	s = (server*)malloc(sizeof(server));
	memset(s, '\0', sizeof(server));
	s->id = message;
	s->count = 1;
	nova->obj = (void*)s;
	nova->next = NULL;
	auxOld->next = nova;

	return 1;
}

// test if user is already registered, else add it to list
int addUser(list *users, char *message) {
	list *nova, *aux, *auxOld;
	user *u;

	if(users->obj==NULL) {
		u = (user*)malloc(sizeof(user));
		memset(u, '\0', sizeof(user));
		u->id = message;
		u->count = 1;
		users->obj = (void*)u;

		return 1;
	}

	for(aux=users; aux != NULL; aux=aux->next) {
		auxOld = aux;
		if(strcmp(message, ((user*)(aux->obj))->id)==0)
			return ++(((user*)(aux->obj))->count);
	}

	nova = (list*)malloc(sizeof(list));
	memset(nova, '\0', sizeof(list));
	u = (user*)malloc(sizeof(user));
	memset(u, '\0', sizeof(user));
	u->id = message;
	u->count = 1;
	nova->obj = (void*)u;
	nova->next = NULL;
	auxOld->next = nova;

	return 1;
}

//clear list
void clearList(list *l) {
	list *aux, *auxOld;
	
	for(aux=l; aux != NULL; aux = aux->next) {
		free(aux->obj);
		auxOld = aux;
		aux = aux->next;
		free(auxOld);
	}
}


// main
int main(int argc, char **argv) {
	int fd, addrlen, ret, newfd;
	int nread, nwritten, nbytes;
	struct sockaddr_in clientaddr;
	struct sockaddr_in addr;
	char *ptrBuffer, bufferReceived[BUFFER_SIZE], bufferSend[BUFFER_SIZE], *bufferPrint;
	char *aux;
	int i;
	list *userList, *serverList, *msgs;

	//obter lista round-robin de mensagens
	msgs = buildMsgs();

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
	
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		fprintf(stderr, "ERROR: Couldn't create socket...\n");
		exit(1);
	}
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(STATport);
	
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret == -1) {
		fprintf(stderr, "ERROR: Couldn't bind socket...\n");
		exit(1);
	}
	
	serverList = buildServerList();
	userList = buildUserList();

	while(1) {
		addrlen = sizeof(addr);

		nread = recvfrom(fd, bufferReceived, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &addrlen);
		
		if(nread == -1) {
			fprintf(stderr, "ERROR: message couldn't be read from SMB...\n");
			exit(1);
		}
	
		if(strncmp(bufferReceived, "REG", 3)==0) {
			bufferPrint = (char*)malloc(sizeof(char)*BUFFER_SIZE);
			memset(bufferPrint, '\0', sizeof(char)*BUFFER_SIZE);
			strncpy(bufferPrint, bufferReceived+4, strlen(bufferReceived));
			bufferPrint = strtok(bufferPrint, "\n");
			printf("Server: %s - #%d\n", bufferPrint, addServer(serverList,bufferPrint));

			memset(bufferSend, '\0', sizeof(bufferSend));
			ptrBuffer = strcpy(bufferSend, "STAT: OK smb was registered\n");
			
			nwritten = sendto(fd, ptrBuffer, strlen(ptrBuffer), 0, (struct sockaddr*)&clientaddr, addrlen);
		
			if(nwritten == -1) {
				fprintf(stderr, "ERROR: message couldn't be sent to SMB...\n");
				exit(1);
			}
		}
		else if(strncmp(bufferReceived, "UDP", 3)==0) {
			aux = strtok(bufferReceived," ");
			for(i=0;i<3;i++) {
				aux = strtok(NULL," \n");
				if(aux==NULL) {
					fprintf(stderr, "ERROR: missing user in UDP message...\n");
					return 1;
				}
			}
			bufferPrint = (char*)malloc(sizeof(char)*BUFFER_SIZE);
			memset(bufferPrint, '\0', sizeof(char)*BUFFER_SIZE);
			strncpy(bufferPrint, aux, strlen(aux));
			printf("Client: %s - #%d\n", bufferPrint, addUser(userList,bufferPrint));

			strcpy(ptrBuffer, "STAT: OK user was registered\n");
			
			nwritten = sendto(fd, ptrBuffer, strlen(ptrBuffer), 0, (struct sockaddr*)&clientaddr, addrlen);
		
			if(nwritten == -1) {
				fprintf(stderr, "ERROR: message couldn't be sent to SMB...\n");
				exit(1);
			}
		}
		else {
			char buffer[BUFFER_SIZE];
			ptrBuffer = strcpy(buffer, "KO\n");
			nwritten = sendto(fd, ptrBuffer, strlen(ptrBuffer), 0, (struct sockaddr*)&clientaddr, addrlen);
			fprintf(stderr, "ERROR: message unknown...\n");
			
			if(nwritten == -1) {
				fprintf(stderr, "ERROR: message couldn't be sent to SMB...\n");
				exit(1);
			}
		}


	}

	clearList(userList);
	clearList(serverList);
	
	close(fd);
	exit(0);
}

/* vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */
