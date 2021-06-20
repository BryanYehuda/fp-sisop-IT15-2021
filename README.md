# fp-sisop-IT15-2021

Kami membuat temple kode untuk client server nya. Berikut adalah template yang kami gunakan dalam membuat socket.  
kode template untuk client.
*ini client*
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

#define PORT 4443

//Fungsi lain

int main(int argc, char *argv[]){
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[32000];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");
	while(1){
		printf("Client: \t");
		char input[10000];
		char perintah[100][10000];
		char *token;
		int i=0;
		scanf(" %[^\n]s", input);
		printf("ini input %s\n", input);
		token = strtok(input, " ");
		while( token != NULL ) {
			strcpy(perintah[i], token);
			i++;
			token = strtok(NULL, " ");
		}
		// kodelain
		// printf("printah 0 : %s\n", perintah[0]);
		if(strcmp(perintah[0], ":exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

		if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("Server: \t%s\n", buffer);
		}
	}

	return 0;
}
```