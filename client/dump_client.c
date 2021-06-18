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
    FILE *fp;
    char lokasi[10000];
	snprintf(lokasi, sizeof lokasi, "../database/log/log%s.log", argv[2]);
    fp = fopen(lokasi, "rb");
    char buffer[20000];
    char perintah[100][10000];
    int found=0;
    while (fgets(buffer, 20000 , fp)){
        char *token;
        char buffercopy[20000];
        snprintf(buffercopy, sizeof buffercopy, "%s", buffer);
        token = strtok(buffercopy, ":");
        int i=0;
        while ( token != NULL){
            strcpy(perintah[i], token);
			// printf("%s\n", perintah[i]);
			i++;
			token = strtok(NULL, ":");
        }
        // printf("%s\n", perintah[4]);
        char *b = strstr(perintah[4], "USE");
        if(b != NULL){
            char *tokens;
            char perintahCopy[20000];
            strcpy(perintahCopy, perintah[4]);
            tokens = strtok(perintahCopy, "; ");
            int j=0;
            char perintahUse[100][10000];
            while ( tokens != NULL){
                strcpy(perintahUse[j], tokens);
                // printf("%s\n", perintahUse[j]);
                j++;
                tokens = strtok(NULL, "; ");
            }
            char databaseTarget[20000];
            // sprintf(databaseTarget, "%s;", argv[5]);
            // printf("%s\n", perintahUse[1]);
            // printf("%s\n", databaseTarget);
            if(strcmp(perintahUse[1], argv[5])==0){
                found = 1;
            }else{
                found = 0;
            }
        }
        // printf("%d\n", found);
        if(found == 1){
            printf("%s", buffer);
        }
        // printf("%s", buffer);
    }
    fclose(fp);
}
