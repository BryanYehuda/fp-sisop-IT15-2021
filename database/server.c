#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 4443

struct allowed{
	char name[10000];
	char password[10000];
};

struct allowed_database{
	char database[10000];
	char name[10000];
};

struct table{
	int jumlahkolom;
	char type[100][10000];
	char data[100][10000];
};

void createUser(char *nama, char *password){
	struct allowed user;
	strcpy(user.name, nama);
	strcpy(user.password, password);
	printf("%s %s\n", user.name, user.password);
	char fname[]={"databases/user.dat"};
	FILE *fp;
	fp=fopen(fname,"ab");
	fwrite(&user,sizeof(user),1,fp);
	fclose(fp);
}

int cekUserExist(char *username ){
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
			return 1;
		}
		if(feof(fp)){
			break;
		}
	}
	fclose(fp);
	// printf("found %d: \n",found);
	return 0;
	
}

void insertPermission(char *nama, char *database){
	struct allowed_database user;
	strcpy(user.name, nama);
	strcpy(user.database, database);
	printf("%s %s\n", user.name, user.database);
	char fname[]={"databases/permission.dat"};
	FILE *fp;
	fp=fopen(fname,"ab");
	fwrite(&user,sizeof(user),1,fp);
	fclose(fp);
}

int cekAllowedDatabase(char *nama, char *database){
	FILE *fp;
	struct allowed_database user;
	int id,found=0;
	printf("nama = %s  database = %s", nama, database);
	fp=fopen("../database/databases/permission.dat","rb");
	while(1){	
		fread(&user,sizeof(user),1,fp);
		if(strcmp(user.name, nama)==0){
			// printf("MASUK!");
			if(strcmp(user.database, database)==0){
				return 1;
			}
		}
		if(feof(fp)){
			break;
		}
	}
	fclose(fp);
	return 0;
}

int findColumn(char *table, char *kolom){
	FILE *fp;
	struct table user;
	int id,found=0;
	fp=fopen(table,"rb");	
	fread(&user,sizeof(user),1,fp);
    // printf("%d\n", user.jumlahkolom);
	int index=-1;
	for(int i=0; i < user.jumlahkolom; i++){
        // printf("%s\n", user.data[i]);
		if(strcmp(user.data[i], kolom)==0){
			index = i;
		}
	}
    if(feof(fp)){
		return -1;
	}
	fclose(fp);
	return index;
}

int deleteColumn(char *table, int index){
	FILE *fp, *fp1;
	struct table user;
	int id,found=0;
	fp=fopen(table,"rb");
	fp1=fopen("temp","wb");
	while(1){	
		fread(&user,sizeof(user),1,fp);
        if(feof(fp)){
			break;
		}
		struct table userCopy;
		int iterasi=0;
		for(int i=0; i< user.jumlahkolom; i++){
			if(i == index){
				continue;
			}
			strcpy(userCopy.data[iterasi], user.data[i]);
            // printf("%s\n", userCopy.data[iterasi]);
			strcpy(userCopy.type[iterasi], user.type[i]);
			printf("%s\n", userCopy.data[iterasi]);
			iterasi++;
		}
		userCopy.jumlahkolom = user.jumlahkolom-1;
		fwrite(&userCopy,sizeof(userCopy),1,fp1);
	}
	fclose(fp);
	fclose(fp1);
	remove(table);
	rename("temp", table);
	return 0;
}

int deleteTable(char *table, char *namaTable){
    FILE *fp, *fp1;
	struct table user;
	int id,found=0;
	fp=fopen(table,"rb");
    fp1=fopen("temp","ab");	
	fread(&user,sizeof(user),1,fp);
    // printf("%d\n", user.jumlahkolom);
	int index=-1;
    struct table userCopy;
	for(int i=0; i < user.jumlahkolom; i++){
        strcpy(userCopy.data[i], user.data[i]);
        strcpy(userCopy.type[i], user.type[i]);
	}
    userCopy.jumlahkolom = user.jumlahkolom;
    fwrite(&userCopy,sizeof(userCopy),1,fp1);
    fclose(fp);
	fclose(fp1);
    remove(table);
	rename("temp", table);
	return 1;
}

int updateColumn(char *table, int index, char *ganti){
    FILE *fp, *fp1;
	struct table user;
	int id,found=0;
	fp=fopen(table,"rb");
    fp1=fopen("temp","ab");
    int datake = 0;
	while(1){	
		fread(&user,sizeof(user),1,fp);
        if(feof(fp)){
			break;
		}
        struct table userCopy;
        int iterasi=0;
        for(int i=0; i< user.jumlahkolom; i++){
            if(i == index && datake!=0){
                strcpy(userCopy.data[iterasi], ganti);
            }else{
                strcpy(userCopy.data[iterasi], user.data[i]);
            }
            printf("%s\n", userCopy.data[iterasi]);
            strcpy(userCopy.type[iterasi], user.type[i]);
            printf("%s\n", userCopy.data[iterasi]);
            iterasi++;
        }
        userCopy.jumlahkolom = user.jumlahkolom;
        fwrite(&userCopy,sizeof(userCopy),1,fp1);
        datake++;
	}
	fclose(fp);
	fclose(fp1);
	remove(table);
	rename("temp", table);
	return 0;
}

int updateColumnWhere(char *table, int index ,char *ganti, int indexGanti ,char *where){
    FILE *fp, *fp1;
	struct table user;
	int id,found=0;
	fp=fopen(table,"rb");
    fp1=fopen("temp","ab");
    int datake = 0;
	while(1){	
		fread(&user,sizeof(user),1,fp);
        if(feof(fp)){
			break;
		}
        struct table userCopy;
        int iterasi=0;
        for(int i=0; i< user.jumlahkolom; i++){
            if(i == index && datake!=0 && strcmp(user.data[indexGanti], where)==0){
                strcpy(userCopy.data[iterasi], ganti);
            }else{
                strcpy(userCopy.data[iterasi], user.data[i]);
            }
            printf("%s\n", userCopy.data[iterasi]);
            strcpy(userCopy.type[iterasi], user.type[i]);
            printf("%s\n", userCopy.data[iterasi]);
            iterasi++;
        }
        userCopy.jumlahkolom = user.jumlahkolom;
        fwrite(&userCopy,sizeof(userCopy),1,fp1);
        datake++;
	}
	fclose(fp);
	fclose(fp1);
	remove(table);
	rename("temp", table);
	return 0;
}

int deleteTableWhere(char *table, int index, char *kolom, char *where){
    FILE *fp, *fp1;
	struct table user;
	int id,found=0;
	fp=fopen(table,"rb");
    fp1=fopen("temp","ab");
    int datake = 0;
	while(1){	
        found = 0;
		fread(&user,sizeof(user),1,fp);
        if(feof(fp)){
			break;
		}
        struct table userCopy;
        int iterasi=0;
        for(int i=0; i< user.jumlahkolom; i++){
            if(i == index && datake!=0 && strcmp(user.data[i], where)==0){
                found = 1;
            }
            strcpy(userCopy.data[iterasi], user.data[i]);
            printf("%s\n", userCopy.data[iterasi]);
            strcpy(userCopy.type[iterasi], user.type[i]);
            printf("%s\n", userCopy.data[iterasi]);
            iterasi++;
        }
        userCopy.jumlahkolom = user.jumlahkolom;
        if(found != 1){
            fwrite(&userCopy,sizeof(userCopy),1,fp1);
        }
        datake++;
	}
	fclose(fp);
	fclose(fp1);
	remove(table);
	rename("temp", table);
	return 0;
}

void writelog(char *perintah, char *nama){
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
 
	char infoWriteLog[1000];

	FILE *file;
	file = fopen("logUser.log", "ab");

	sprintf(infoWriteLog, "%d-%.2d-%.2d %.2d:%.2d:%.2d::%s::%s\n",timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, perintah);
	// sprintf(infoWriteLog, "%.2d%.2d%d-%.2d:%.2d:%.2d::%s::%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, nama, filepath);

	fputs(infoWriteLog, file);
	fclose(file);
	return;
}

int main(){

	int sockfd, ret;
	 struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 4444);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}


	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
				recv(newSocket, buffer, 1024, 0);
				// printf("buffer %s\n", buffer);
				char *token;
				char buffercopy[32000];
				strcpy(buffercopy, buffer);
				char perintah[100][10000];
				token = strtok(buffercopy, ":");
				int i=0;
				char database_used[1000];
				while( token != NULL ) {
					strcpy(perintah[i], token);
					// printf("%s\n", perintah[i]);
					i++;
					token = strtok(NULL, ":");
				}
				if(strcmp(perintah[0], "cUser")==0){
					if(strcmp(perintah[3], "0")==0){
						createUser(perintah[1], perintah[2]);
					}else{
						char peringatan[] = "You're Not Allowed";
						send(newSocket, peringatan, strlen(peringatan), 0);
						bzero(buffer, sizeof(buffer));
					}
				}else if(strcmp(perintah[0], "gPermission")==0){
					if(strcmp(perintah[3], "0")==0){
						int exist = cekUserExist(perintah[2]);
						if(exist == 1){
							insertPermission(perintah[2], perintah[1]);
						}else{
							char peringatan[] = "User Not Found";
							send(newSocket, peringatan, strlen(peringatan), 0);
							bzero(buffer, sizeof(buffer));
						}
					}else{
						char peringatan[] = "You're Not Allowed";
						send(newSocket, peringatan, strlen(peringatan), 0);
						bzero(buffer, sizeof(buffer));
					}
				}else if(strcmp(perintah[0], "cDatabase")==0){
					char lokasi[20000];
					snprintf(lokasi, sizeof lokasi, "databases/%s", perintah[1]);
					printf("lokasi = %s, nama = %s , database = %s\n", lokasi, perintah[2], perintah[1]);
					mkdir(lokasi,0777);
					insertPermission(perintah[2], perintah[1]);
				}else if(strcmp(perintah[0], "uDatabase") == 0){
					if(strcmp(perintah[3], "0") != 0){
						int allowed = cekAllowedDatabase(perintah[2], perintah[1]);
						if(allowed != 1){
							char peringatan[] = "Access_database : You're Not Allowed";
							send(newSocket, peringatan, strlen(peringatan), 0);
							bzero(buffer, sizeof(buffer));
						}else{
							strncpy(database_used, perintah[1], sizeof(perintah[1]));
							char peringatan[] = "Access_database : Allowed";
							printf("database_used = %s\n", database_used);
							send(newSocket, peringatan, strlen(peringatan), 0);
							bzero(buffer, sizeof(buffer));
						}
					}
				}else if(strcmp(perintah[0], "cekCurrentDatabase")==0){
					if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
					}
					send(newSocket, database_used, strlen(database_used), 0);
					bzero(buffer, sizeof(buffer));
				}else if(strcmp(perintah[0], "cTable")==0){
					printf("%s\n", perintah[1]);
					char *tokens;
					if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
					}else{
                        char daftarQuery[100][10000];
                        char copyPerintah[20000];
                        snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
                        tokens = strtok(copyPerintah, "(), ");
                        int jumlah=0;
                        while( tokens != NULL ) {
                            strcpy(daftarQuery[jumlah], tokens);
                            printf("%s\n", daftarQuery[jumlah]);
                            jumlah++;
                            tokens = strtok(NULL, "(), ");
                        }
                        char buatTable[20000];
                        snprintf(buatTable, sizeof buatTable, "../database/databases/%s/%s", database_used, daftarQuery[2]);
                        int iterasi = 0;
                        int iterasiData = 3;
                        struct table kolom;
                        while(jumlah > 3){
                            strcpy(kolom.data[iterasi], daftarQuery[iterasiData]);
                            printf("%s\n", kolom.data[iterasi]);
                            strcpy(kolom.type[iterasi], daftarQuery[iterasiData+1]);
                            iterasiData = iterasiData+2;
                            jumlah=jumlah-2;
                            iterasi++;
                        }
                        kolom.jumlahkolom = iterasi;
                        printf("iterasi = %d\n", iterasi);
                        FILE *fp;
                        printf("%s\n", buatTable);
                        fp=fopen(buatTable,"ab");
                        fwrite(&kolom,sizeof(kolom),1,fp);
                        fclose(fp);
                    }
				}else if(strcmp(perintah[0], "dDatabase")==0){
					int allowed = cekAllowedDatabase(perintah[2], perintah[1]);
					if(allowed != 1){
						char peringatan[] = "Access_database : You're Not Allowed";
						send(newSocket, peringatan, strlen(peringatan), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}else{
						char hapus[20000];
						snprintf(hapus, sizeof hapus, "rm -r databases/%s", perintah[1]);
						system(hapus);
						char peringatan[] = "Database Has Been Removed";
						send(newSocket, peringatan, strlen(peringatan), 0);
						bzero(buffer, sizeof(buffer));
					}
				}else if(strcmp(perintah[0], "dTable")==0){
					if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					char hapus[20000];
					snprintf(hapus, sizeof hapus, "databases/%s/%s", database_used ,perintah[1]);
					remove(hapus);
					char peringatan[] = "Table Has Been Removed";
					send(newSocket, peringatan, strlen(peringatan), 0);
					bzero(buffer, sizeof(buffer));
				}else if(strcmp(perintah[0], "dColumn")==0){
					if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
					char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, perintah[2]);
					int index = findColumn(buatTable, perintah[1]);
                    if(index == -1){
                        char peringatan[] = "Column Not Found";
                        send(newSocket, peringatan, strlen(peringatan), 0);
                        bzero(buffer, sizeof(buffer));
                        continue;
                    }
                    // printf("%d\n", index);
					deleteColumn(buatTable, index);
					char peringatan[] = "Column Has Been Removed";
					send(newSocket, peringatan, strlen(peringatan), 0);
					bzero(buffer, sizeof(buffer));
				}else if(strcmp(perintah[0], "insert")==0){
                    if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
                    char daftarQuery[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
                    // printf("%s\n", copyPerintah);
                    char *tokens;
                    tokens = strtok(copyPerintah, "\'(), ");
					int jumlah=0;
					while( tokens != NULL ) {
						strcpy(daftarQuery[jumlah], tokens);
						// printf("%s\n", daftarQuery[jumlah]);
						jumlah++;
						tokens = strtok(NULL, "\'(), ");
					}
                    char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[2]);
                    // printf("buat table = %s\n", buatTable);
                    FILE *fp;
					// printf("%s\n", buatTable);
                    int banyakKolom;
					fp=fopen(buatTable,"r");
                    if (fp == NULL){
                        char peringatan[] = "TABLE NOT FOUND";
                        send(newSocket, peringatan, strlen(peringatan), 0);
                        bzero(buffer, sizeof(buffer));
                        continue;
                    }else{
                        struct table user;
                        fread(&user,sizeof(user),1,fp);
                        banyakKolom=user.jumlahkolom;
                        fclose(fp);
                    }
					int iterasi = 0;
					int iterasiData = 3;
					struct table kolom;
					while(jumlah > 3){
						strcpy(kolom.data[iterasi], daftarQuery[iterasiData]);
                        printf("%s\n", kolom.data[iterasi]);
						strcpy(kolom.type[iterasi], "string");
						iterasiData++;
						jumlah=jumlah-1;
						iterasi++;
					}
					kolom.jumlahkolom = iterasi;
                    // printf("iterasi = %d\n", iterasi);
                    // printf("%d\n", user.jumlahkolom);
                    if(banyakKolom != kolom.jumlahkolom){
                        char peringatan[] = "YOUR INPUT NOT MATCH THE COLUMN";
                        send(newSocket, peringatan, strlen(peringatan), 0);
                        bzero(buffer, sizeof(buffer));
                        continue;
                    }   
					// fwrite(&kolom,sizeof(kolom),1,fp);
					printf("iterasi = %d\n", iterasi);
                    FILE *fp1;
                    printf("%s\n", buatTable);
                    fp1=fopen(buatTable,"ab");
                    fwrite(&kolom,sizeof(kolom),1,fp1);
                    fclose(fp1);
                    char peringatan[] = "Data Has Been Inserted";
					send(newSocket, peringatan, strlen(peringatan), 0);
					bzero(buffer, sizeof(buffer));
                }else if(strcmp(perintah[0], "update")==0){
                    if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
                    char daftarQuery[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
                    // printf("%s\n", copyPerintah);
                    char *tokens;
                    tokens = strtok(copyPerintah, "\'(),= ");
					int jumlah=0;
					while( tokens != NULL ) {
						strcpy(daftarQuery[jumlah], tokens);
						printf("%s\n", daftarQuery[jumlah]);
						jumlah++;
						tokens = strtok(NULL, "\'(),= ");
					}
                    printf("jumlah = %d\n", jumlah);
                    char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[1]);
                    if(jumlah==5){
                        printf("buat table = %s, kolumn = %s", buatTable, daftarQuery[3]);
                        int index = findColumn(buatTable, daftarQuery[3]);
                        if(index == -1){
                            char peringatan[] = "Column Not Found";
                            send(newSocket, peringatan, strlen(peringatan), 0);
                            bzero(buffer, sizeof(buffer));
                            continue;
                        }
                        printf("index = %d\n", index);
                        updateColumn(buatTable, index, daftarQuery[4]);
                    }else if(jumlah==8){
                        printf("buat table = %s, kolumn = %s", buatTable, daftarQuery[3]);
                        int index = findColumn(buatTable, daftarQuery[3]);
                        if(index == -1){
                            char peringatan[] = "Column Not Found";
                            send(newSocket, peringatan, strlen(peringatan), 0);
                            bzero(buffer, sizeof(buffer));
                            continue;
                        }
                        printf("%s\n", daftarQuery[7]);
                        int indexGanti = findColumn(buatTable, daftarQuery[6]);
                        updateColumnWhere(buatTable, index, daftarQuery[4], indexGanti ,daftarQuery[7]);
                    }else{
                        char peringatan[] = "Data Has Been Deleted";
                        send(newSocket, peringatan, strlen(peringatan), 0);
                        bzero(buffer, sizeof(buffer));
                        continue;
                    }
                    char peringatan[] = "Data Has Been Updated";
					send(newSocket, peringatan, strlen(peringatan), 0);
					bzero(buffer, sizeof(buffer));

                }else if(strcmp(perintah[0], "delete")==0){
                    if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
                    char daftarQuery[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
                    // printf("%s\n", copyPerintah);
                    char *tokens;
                    tokens = strtok(copyPerintah, "\'(),= ");
					int jumlah=0;
					while( tokens != NULL ) {
						strcpy(daftarQuery[jumlah], tokens);
						printf("%s\n", daftarQuery[jumlah]);
						jumlah++;
						tokens = strtok(NULL, "\'(),= ");
					}
                    printf("jumlah = %d\n", jumlah);
                    char buatTable[20000];
					snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[2]);
                    if(jumlah==3){
                        deleteTable(buatTable, daftarQuery[2]);
                    }else if(jumlah==6){
                        int index = findColumn(buatTable, daftarQuery[4]);
                        if(index == -1){
                            char peringatan[] = "Column Not Found";
                            send(newSocket, peringatan, strlen(peringatan), 0);
                            bzero(buffer, sizeof(buffer));
                            continue;
                        }
                        printf("index  = %d\n", index);
                        deleteTableWhere(buatTable, index, daftarQuery[4], daftarQuery[5]);
                    }else{
                        char peringatan[] = "Input Salah";
                        send(newSocket, peringatan, strlen(peringatan), 0);
                        bzero(buffer, sizeof(buffer));
                        continue;
                    }
                    char peringatan[] = "Data Has Been Deleted";
					send(newSocket, peringatan, strlen(peringatan), 0);
					bzero(buffer, sizeof(buffer));
                }else if(strcmp(perintah[0], "select")==0){
                    if(database_used[0] == '\0'){
						strcpy(database_used, "You're not selecting database yet");
						send(newSocket, database_used, strlen(database_used), 0);
						bzero(buffer, sizeof(buffer));
						continue;
					}
                    char daftarQuery[100][10000];
					char copyPerintah[20000];
					snprintf(copyPerintah, sizeof copyPerintah, "%s", perintah[1]);
                    // printf("%s\n", copyPerintah);
                    char *tokens;
                    tokens = strtok(copyPerintah, "\'(),= ");
					int jumlah=0;
					while( tokens != NULL ) {
						strcpy(daftarQuery[jumlah], tokens);
						printf("%s\n", daftarQuery[jumlah]);
						jumlah++;
						tokens = strtok(NULL, "\'(),= ");
					}
					printf("ABC\n");
                    if(jumlah == 4){
						char buatTable[20000];
						snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[3]);
						printf("buat table = %s", buatTable);
                        char perintahKolom[1000];
                        printf("masuk 4\n");
                        if(strcmp(daftarQuery[1], "*")==0){
                            // showTableAll(buatTable, "ALL");
                            FILE *fp, *fp1;
                            struct table user;
                            int id,found=0;
                            fp=fopen(buatTable,"rb");
                            char buffers[40000];
                            char sendDatabase[40000];
                            bzero(buffer, sizeof(buffer));
                            bzero(sendDatabase, sizeof(sendDatabase));
                            while(1){	
                                char enter[] = "\n";
                                // bzero(enter, sizeof(enter));
                                fread(&user,sizeof(user),1,fp);
                                snprintf(buffers, sizeof buffers, "\n");
                                // send(newSocket, enter, strlen(enter), 0);
                                if(feof(fp)){
                                    break;
                                }
                                for(int i=0; i< user.jumlahkolom; i++){
                                    char padding[20000];
                                    snprintf(padding, sizeof padding, "%s\t",user.data[i]);
                                    strcat(buffers, padding);
                                    // send(newSocket, buffer, strlen(buffer), 0);
                                    // bzero(buffer, sizeof(buffer));
                                }
                                // printf("%s", buffers);
                                strcat(sendDatabase, buffers);
                            }
                            // printf("ini send fix\n%s\n", sendDatabase);
                            send(newSocket, sendDatabase, strlen(sendDatabase), 0);
                            bzero(sendDatabase, sizeof(sendDatabase));
                            bzero(buffer, sizeof(buffer));
                            fclose(fp);
                        }else{
                            // showTable(buatTable, perintah[1]);
                            int index = findColumn(buatTable, daftarQuery[1]);
                            printf("%d\n", index);
                            FILE *fp, *fp1;
                            struct table user;
                            int id,found=0;
                            fp=fopen(buatTable,"rb");
                            char buffers[40000];
                            char sendDatabase[40000];
                            bzero(buffer, sizeof(buffer));
                            bzero(sendDatabase, sizeof(sendDatabase));
                            while(1){	
                                char enter[] = "\n";
                                // bzero(enter, sizeof(enter));
                                fread(&user,sizeof(user),1,fp);
                                snprintf(buffers, sizeof buffers, "\n");
                                // send(newSocket, enter, strlen(enter), 0);
                                if(feof(fp)){
                                    break;
                                }
                                for(int i=0; i< user.jumlahkolom; i++){
                                    if(i == index){
                                        char padding[20000];
                                        snprintf(padding, sizeof padding, "%s\t",user.data[i]);
                                        strcat(buffers, padding);
                                    }
                                    // send(newSocket, buffer, strlen(buffer), 0);
                                    // bzero(buffer, sizeof(buffer));
                                }
                                // printf("%s", buffers);
                                strcat(sendDatabase, buffers);
                            }
                            printf("ini send fix\n%s\n", sendDatabase);
                            fclose(fp);
                            send(newSocket, sendDatabase, strlen(sendDatabase), 0);
                            bzero(sendDatabase, sizeof(sendDatabase));
                            bzero(buffer, sizeof(buffer));
                        }
                    }else if(jumlah == 7 && strcmp(daftarQuery[4], "WHERE")==0){
						char buatTable[20000];
						snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[3]);
						printf("buat table = %s", buatTable);
                        char perintahKolom[1000];
                        printf("masuk 4\n");
                        if(strcmp(daftarQuery[1], "*")==0){
                            // showTableAll(buatTable, "ALL");
                            FILE *fp, *fp1;
                            struct table user;
                            int id,found=0;
                            fp=fopen(buatTable,"rb");
                            char buffers[40000];
                            char sendDatabase[40000];
                            int index = findColumn(buatTable, daftarQuery[5]);
                            printf("%d\n", index);
                            bzero(buffer, sizeof(buffer));
                            bzero(sendDatabase, sizeof(sendDatabase));
                            while(1){	
                                char enter[] = "\n";
                                // bzero(enter, sizeof(enter));
                                fread(&user,sizeof(user),1,fp);
                                snprintf(buffers, sizeof buffers, "\n");
                                // send(newSocket, enter, strlen(enter), 0);
                                if(feof(fp)){
                                    break;
                                }
                                for(int i=0; i< user.jumlahkolom; i++){
                                    if(strcmp(user.data[index], daftarQuery[6])==0){
                                        char padding[20000];
                                        snprintf(padding, sizeof padding, "%s\t",user.data[i]);
                                        strcat(buffers, padding);
                                    }
                                    // send(newSocket, buffer, strlen(buffer), 0);
                                    // bzero(buffer, sizeof(buffer));
                                }
                                // printf("%s", buffers);
                                strcat(sendDatabase, buffers);
                            }
                            // printf("ini send fix\n%s\n", sendDatabase);
                            send(newSocket, sendDatabase, strlen(sendDatabase), 0);
                            bzero(sendDatabase, sizeof(sendDatabase));
                            bzero(buffer, sizeof(buffer));
                            fclose(fp);
                        }else{
                            // showTable(buatTable, perintah[1]);
                            int index = findColumn(buatTable, daftarQuery[1]);
                            printf("%d\n", index);
                            FILE *fp, *fp1;
                            struct table user;
                            int id,found=0;
                            int indexGanti = findColumn(buatTable, daftarQuery[5]);
                            fp=fopen(buatTable,"rb");
                            char buffers[40000];
                            char sendDatabase[40000];
                            bzero(buffer, sizeof(buffer));
                            bzero(sendDatabase, sizeof(sendDatabase));
                            while(1){	
                                char enter[] = "\n";
                                // bzero(enter, sizeof(enter));
                                fread(&user,sizeof(user),1,fp);
                                snprintf(buffers, sizeof buffers, "\n");
                                // send(newSocket, enter, strlen(enter), 0);
                                if(feof(fp)){
                                    break;
                                }
                                for(int i=0; i< user.jumlahkolom; i++){
                                    if(i == index && (strcmp(user.data[indexGanti], daftarQuery[6])==0 || strcmp(user.data[i],daftarQuery[5])==0)){
                                        char padding[20000];
                                        snprintf(padding, sizeof padding, "%s\t",user.data[i]);
                                        strcat(buffers, padding);
                                    }
                                    // send(newSocket, buffer, strlen(buffer), 0);
                                    // bzero(buffer, sizeof(buffer));
                                }
                                // printf("%s", buffers);
                                strcat(sendDatabase, buffers);
                            }
                            printf("ini send fix\n%s\n", sendDatabase);
                            fclose(fp);
                            send(newSocket, sendDatabase, strlen(sendDatabase), 0);
                            bzero(sendDatabase, sizeof(sendDatabase));
                            bzero(buffer, sizeof(buffer));
                        }
                    }else{
						printf("ini query 3 %s", daftarQuery[jumlah-3]);
						if(strcmp(daftarQuery[jumlah-3], "WHERE")!= 0){
							char buatTable[20000];
							snprintf(buatTable, sizeof buatTable, "databases/%s/%s", database_used, daftarQuery[jumlah-1]);
							printf("buat table = %s", buatTable);
							printf("tanpa where");
							int index[100];
							int iterasi=0;
							for(int i=1; i<jumlah-2; i++){
								index[iterasi] = findColumn(buatTable, daftarQuery[i]);
								printf("%d\n", index[iterasi]);
								iterasi++;
							}
						}else if(strcmp(daftarQuery[jumlah-3], "WHERE")== 0){
							printf("dengan where");
						}
					}
                }else if(strcmp(perintah[0], "log")==0){
					writelog(perintah[1], perintah[2]);
					char peringatan[] = "\n";
					send(newSocket, peringatan, strlen(peringatan), 0);
					bzero(buffer, sizeof(buffer));
				}
				if(strcmp(buffer, ":exit") == 0){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else{
					printf("Client: %s\n", buffer);
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
				}
			}
		}

	}

	close(newSocket);


	return 0;
}
