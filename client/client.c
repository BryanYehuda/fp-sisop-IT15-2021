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
#include <time.h>

#define PORT 4443

struct allowed{
	char name[10000];
	char password[10000];
};

int cekAllowed(char *username, char *password){
	FILE *fp;
	struct allowed user;
	int id,found=0;
	fp=fopen("../database/databases/user.dat","rb");
	while(1){	
		fread(&user,sizeof(user),1,fp);
		// printf("%s %s\n", user.name, username);
		// printf("%s %s\n", user.password, password);
		// printf("masuk");
		if(strcmp(user.name, username)==0){
			// printf("MASUK!");
			if(strcmp(user.password, password)==0){
				found=1;
			}
		}
		if(feof(fp)){
			break;
		}
	}
	fclose(fp);
	// printf("found %d: \n",found);
	if(found==0){
		printf("You're Not Allowed\n");
		return 0;
	}else{
		return 1;
	}
	
}

void writelog(char *perintah, char *nama){
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
 
	char infoWriteLog[1000];

	FILE *file;
	char lokasi[10000];
	snprintf(lokasi, sizeof lokasi, "../database/log/log%s.log", nama);
	file = fopen(lokasi, "ab");

	sprintf(infoWriteLog, "%d-%.2d-%.2d %.2d:%.2d:%.2d:%s:%s;\n",timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, perintah);
	// sprintf(infoWriteLog, "%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, filepath);

	fputs(infoWriteLog, file);
	fclose(file);
	return;
}

int main(int argc, char *argv[]){
	// printf("%s\n", argv[0]);
	int allowed=0;
	int id_user = geteuid();
	char database_used[1000];
	if(geteuid() == 0){
		// printf("I AM ROOT");
		allowed=1;
	}else{
		int id = geteuid();
		// printf("ID kamu : %d", id);
		allowed = cekAllowed(argv[2],argv[4]);
	}
	if(allowed==0){
		return 0;
	}
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
		char copyinput[10000];
		char perintah[100][10000];
		char *token;
		int i=0;  
		scanf(" %[^\n]s", input);
		strcpy(copyinput, input);
		// printf("ini input %s\n", input);
		token = strtok(input, " ");
		while( token != NULL ) {
			strcpy(perintah[i], token);
			// printf("%s\n", perintah[i]);
			i++;
			token = strtok(NULL, " ");
		}
		int wrongCommand = 0;
		// printf("ini input %s\n", input);
		if(strcmp(perintah[0], "CREATE")==0){
			if(strcmp(perintah[1], "USER")==0 && strcmp(perintah[3], "IDENTIFIED")==0 && strcmp(perintah[4], "BY")==0){
				snprintf(buffer, sizeof buffer, "cUser:%s:%s:%d", perintah[2], perintah[5], id_user);
				// printf("%s\n", buffer);
				send(clientSocket, buffer, strlen(buffer), 0);
			}else if(strcmp(perintah[1], "DATABASE")==0){
				snprintf(buffer, sizeof buffer, "cDatabase:%s:%s:%d", perintah[2], argv[2], id_user);
				// printf("%s\n", buffer);
				send(clientSocket, buffer, strlen(buffer), 0);
			}else if(strcmp(perintah[1], "TABLE")==0){
				snprintf(buffer, sizeof buffer, "cTable:%s", copyinput);
				// printf("%s\n", buffer);
				send(clientSocket, buffer, strlen(buffer), 0);
			}
		}else if(strcmp(perintah[0], "GRANT")==0 && strcmp(perintah[1], "PERMISSION")==0 && strcmp(perintah[3], "INTO")==0){
			snprintf(buffer, sizeof buffer, "gPermission:%s:%s:%d", perintah[2],perintah[4], id_user);
			send(clientSocket, buffer, strlen(buffer), 0);
		}else if(strcmp(perintah[0], "USE")==0){
			// strncpy(database_used, perintah[1], sizeof(perintah[1]));
			// printf("database uses %s\n", database_used);
			snprintf(buffer, sizeof buffer, "uDatabase:%s:%s:%d", perintah[1], argv[2], id_user);
			send(clientSocket, buffer, strlen(buffer), 0);
		}else if(strcmp(perintah[0], "cekCurrentDatabase")==0){
			snprintf(buffer, sizeof buffer, "%s", perintah[0]);
			send(clientSocket, buffer, strlen(buffer), 0);
		}else if(strcmp(perintah[0], "DROP")==0){
			if(strcmp(perintah[1], "DATABASE")==0){
				snprintf(buffer, sizeof buffer, "dDatabase:%s:%s", perintah[2], argv[2]);
				send(clientSocket, buffer, strlen(buffer), 0);
			}else if(strcmp(perintah[1], "TABLE")==0){
				snprintf(buffer, sizeof buffer, "dTable:%s:%s", perintah[2], argv[2]);
				send(clientSocket, buffer, strlen(buffer), 0);
			}else if(strcmp(perintah[1], "COLUMN")==0){
				// printf("masuk\n");
				snprintf(buffer, sizeof buffer, "dColumn:%s:%s:%s", perintah[2], perintah[4] ,argv[2]);
				send(clientSocket, buffer, strlen(buffer), 0);
			}
		}else if(strcmp(perintah[0], "INSERT")==0 && strcmp(perintah[1], "INTO")==0){
            snprintf(buffer, sizeof buffer, "insert:%s", copyinput);
			send(clientSocket, buffer, strlen(buffer), 0);
        }else if(strcmp(perintah[0], "UPDATE")==0){
            snprintf(buffer, sizeof buffer, "update:%s", copyinput);
			send(clientSocket, buffer, strlen(buffer), 0);
        }else if(strcmp(perintah[0], "DELETE")==0){
            snprintf(buffer, sizeof buffer, "delete:%s", copyinput);
            send(clientSocket, buffer, strlen(buffer), 0);
        }else if(strcmp(perintah[0], "SELECT")==0){
            snprintf(buffer, sizeof buffer, "select:%s", copyinput);
            send(clientSocket, buffer, strlen(buffer), 0);
        }else if(strcmp(perintah[0], ":exit")!=0){
			wrongCommand = 1;
			char peringatan[] = "Wrong Command";
			send(clientSocket, peringatan, strlen(peringatan), 0);
			// send(clientSocket, perintah[0], strlen(perintah[0]), 0);
		}

		if(wrongCommand != 1){
			char namaSender[10000];
			if(id_user == 0){
				strcpy(namaSender, "root");
			}else{
				strcpy(namaSender, argv[2]);
			}
			writelog(copyinput, namaSender);
		}
		// if(wrongCommand != 1){
		// 	bzero(buffer, sizeof(buffer));
		// 	char namaSender[10000];
		// 	if(id_user == 0){
		// 		strcpy(namaSender, "root");
		// 	}else{
		// 		strcpy(namaSender, argv[2]);
		// 	}
		// 	snprintf(buffer, sizeof buffer, "log:%s:%s", copyinput, namaSender);
        //     send(clientSocket, buffer, strlen(buffer), 0);
		// }
		// for(int j=0;j<i;j++){
		// 	printf("%s\n", perintah[j]);
		// }
		// printf("printah 0 : %s\n", perintah[0]);
		if(strcmp(perintah[0], ":exit") == 0){
			send(clientSocket, perintah[0], strlen(perintah[0]), 0);
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}
		bzero(buffer, sizeof(buffer));
		if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("Server: \t%s\n", buffer);
		}
	}

	return 0;
}
