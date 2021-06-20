# fp-sisop-IT15-2021

Kami membuat temple kode untuk client server nya. Berikut adalah template yang kami gunakan dalam membuat socket.  
kode template untuk client.  
`*ini client*`
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
`*Ini Server*`
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4443

struct allowed{
	char name[10000];
	char password[10000];
};

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
				printf("buffer %s\n", buffer);
				//kode lain
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
```
Setelah kami membuat kode template untuk komunikasi client-server dengan menggunakan socket. setelah itu kami akan memproses bagaimana cara client dan server berkomunikasi. 

## 1. Kami membuat proses bagaimana cara root menggunakan kode server
```c
//Library lain

//fungsi lain

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

    //Kode lain
	
    return 0;
}

```
cara yang kami lakukan adalah dengan mengambil uid dari user yang memanggil program ini. Jika UID nya adalah 0, maka dia adalah user root.

## 2. Setelah membuat bagaimana cara root mengakses program, sekarang kami membuat proses bagaimana cara root untuk membuat CREATE USER
```c
    //kode lain
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
    //kode lain
```
setiap input user langsung kami pisahkan berdasarkan spasi menggunakan strtok, lalu kami salin ke variable perintah. Nah setelah itu kami memproses input user tersebut dengan kode dibawah ini
```c
if(strcmp(perintah[0], "CREATE")==0){
    if(strcmp(perintah[1], "USER")==0 && strcmp(perintah[3], "IDENTIFIED")==0 && strcmp(perintah[4], "BY")==0){
        snprintf(buffer, sizeof buffer, "cUser:%s:%s:%d", perintah[2], perintah[5], id_user);
        send(clientSocket, buffer, strlen(buffer), 0);
    }else if(strcmp(perintah[1], "DATABASE")==0){
        // kode lain
    }else if(strcmp(perintah[1], "TABLE")==0){
        //kode lain
    }
}
```
Hal yang kami lakukan adalah kami mengirimkan flag ke server dimana awal flag tersebut adalah cUSER lalu kami mengirimkan data seperti username dan password serta UID ke server. Untuk diserver, akan dihandle dengan kode dibawah ini.
`*Server*`
```c
token = strtok(buffercopy, ":");
int i=0;
char database_used[1000];
while( token != NULL ) {
    strcpy(perintah[i], token);
    i++;
    token = strtok(NULL, ":");
}
```
Karna kami mengirimkan dari client menggunakan ":", sehingga pada server kami akan memecah informasi tersebut menggunakan strtok lalu menyalinnya ke variabel perintah. Setelah kita mendapatkan semua perintah, untuk pemrosesannya menggunakan kode berikut ini
```c
if(strcmp(perintah[0], "cUser")==0){
    if(strcmp(perintah[3], "0")==0){
        createUser(perintah[1], perintah[2]);
    }else{
        char peringatan[] = "You're Not Allowed";
        send(newSocket, peringatan, strlen(peringatan), 0);
        bzero(buffer, sizeof(buffer));
    }
}
//kode lain
```
pada kode ini kita lihat kami memfilter flag cUSER yang dikirim dari client. Karena create user hanya bisa dilakukan oleh ROOT maka kami membuat handler bagaimana jika ada yang menjalankan perintah tersebut tetapi bukan root dengan mengirimkan pesan "Youre not allowed" kepada user. JIka yang mengakses tersebut adalah root, maka jalankan fungsi createUser. Fungsi createuser yang digunakan adalah sebagai berikut
```c
struct allowed{
	char name[10000];
	char password[10000];
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
```
Pertama kami menyimpan data yang diparsingkan ke fungsi createUser kedalam struct user. setelah kami menyimpan dalam struct, variabel tersebut kami write ke file yang bernama user.dat

## 3. Membuat proses login user menggunakan program
```c
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

    //Kode lain
	
    return 0;
}
```
Pada kode tersebut kami melihat apakah user merupakan root atau tidak. Jika tidak root, maka kami akan cek apakah user tersebut berhak mengakses program yang dipanggil melalui fungsi cekAllowed. Fungsi cekAllowed adalah sebagai berikut :
```c
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
		if(strcmp(user.name, username)==0){
			if(strcmp(user.password, password)==0){
				found=1;
			}
		}
		if(feof(fp)){
			break;
		}
	}
	fclose(fp);
	if(found==0){
		printf("You're Not Allowed\n");
		return 0;
	}else{
		return 1;
	}
}
```
Untuk mengecek apakah user bisa mengakses program, hal pertama yang kami lakukan adalah membaca file user.dat dan karena data yang ada disimpan dalam bentuk struct maka kita juga harus membaca dalam bentuk struct. Setelah itu kami baca perline setiap filenya lalu kami bandingkan apakah usernama dan password nya sama. setelah sama, maka dia akan return 1 yang mengartikan dia berhak menggunakan program.

## 4. Membuat proses bagaimana cara root menggunakan perintah grant permission
```c
else if(strcmp(perintah[0], "GRANT")==0 && strcmp(perintah[1], "PERMISSION")==0 && strcmp(perintah[3], "INTO")==0){
    snprintf(buffer, sizeof buffer, "gPermission:%s:%s:%d", perintah[2],perintah[4], id_user);
    send(clientSocket, buffer, strlen(buffer), 0);
}
```
Hal yang kami lihat apakah user menggunakan perintah GRANT PERMISSION namaDB INTO user. setelah itu kami kirimkan flag gPermission serta beberapa variable lain kerserver. untuk handler di server, kami memisahkan menggunakan strtok seperti yang sudah kami jelaskan sebelumnya diatas. Lalu diproses menggunakan kode dibawah ini.
```c
else if(strcmp(perintah[0], "gPermission")==0){
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
}
```
jika flag yang dikirimkan oleh user adalah "gPermission" lalu kami cek dahulu apa user yang ingin diberikan permission ada atau tidak. jika ada panggil fungsi insertPermission. JIka tidak ditemukan, balas ke client bahwa user not found. Ttetapi jika yang mengirimkan perintah bukan root, maka balas you're not allowed. Untuk fungsi inserPermission berikut adalah kode yang kami gunakan
```c
struct allowed_database{
	char database[10000];
	char name[10000];
};

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
```
kami juga menyimpan daftar izin setiap user menggunakan struct pada file yang bernama permission.dat.

## 5. Handle perintah USE oleh user
Berikut adalah kode yang kami gunakan pada sisi client untuk melakukan handle USE
```c
else if(strcmp(perintah[0], "USE")==0){
    snprintf(buffer, sizeof buffer, "uDatabase:%s:%s:%d", perintah[1], argv[2], id_user);
    send(clientSocket, buffer, strlen(buffer), 0);
}
```
Hal yang penting dari ini adalah kami mengirimkan flag uDatabase sebagai tanda user menggunakan perintah use database. pada server kami menggunakan perintah dibawah ini untuk menghandle perintah tersebut
```c
else if(strcmp(perintah[0], "uDatabase") == 0){
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
}
```
Setelah memastikan flag yang dikirim user, kami memanggil fungsi cekAllowedDatabase untuk mengecek apakah user boleh menggunakan database tersbut. Jika boleh maka kami memasukkan database yang digunakan ke dalam variabel database_used. Jika tidak boleh, maka akan membalas ke user acces database not allowed. Berikut adalah fungsi dari cekAllowedDatabase.
```c
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
```
karna pada insert permission kami menggnuakan struct dalam menyimpan permission, maka untuk membaca nya kami juga menggunakan struct. Untuk setiap data yang dibaca dari file, maka kami bandingkan dengan inputan dari user untuk di cek apakah sama atau tidak. Jika tidak sama return 0 atau jika sama return 1.
## 6. Create Database oleh user
Berikut adalah kode yang kami gunakan pada sisi client untuk melakukan handle database
```c
if(strcmp(perintah[0], "CREATE")==0){
    if(strcmp(perintah[1], "USER")==0 && strcmp(perintah[3], "IDENTIFIED")==0 && strcmp(perintah[4], "BY")==0){
        // kodelain
    }else if(strcmp(perintah[1], "DATABASE")==0){
        snprintf(buffer, sizeof buffer, "cDatabase:%s:%s:%d", perintah[2], argv[2], id_user);
        send(clientSocket, buffer, strlen(buffer), 0);
    }else if(strcmp(perintah[1], "TABLE")==0){
        // Kode lain
    }
}
```
flag yang dikirimkan oleh user ketika menjalankan perintah ini ke server adalah cDatabase dan juga beberapa informasi lainnya. Untuk pada server kami menghandle dengan cara berikut
```c
else if(strcmp(perintah[0], "cDatabase")==0){
    char lokasi[20000];
    snprintf(lokasi, sizeof lokasi, "databases/%s", perintah[1]);
    printf("lokasi = %s, nama = %s , database = %s\n", lokasi, perintah[2], perintah[1]);
    mkdir(lokasi,0777);
    insertPermission(perintah[2], perintah[1]);
}
```
nah karena membuat database, bearti membuat sebuah folder, maka kami menggunakan mkdir. dan karna yang membuat database, langsung mempunyai akses maka kami memanggil fungsi insertPermission(sudah dijelaskan diatas) untuk memasukkan permission user tersebut.
## 7. Create Table oleh user
## 8. Drop Database oleh user
## 9. Drop Table oleh user
## 10. Drop column oleh user
## 11. Insert oleh user
## 12. Update dan Update Where oleh user
## 13. Delete dan Delete where oleh user
## 14. Select dan Select where oleh user
## 15. Logging untuk setiap perintah user
Setiap perintah yang dimasukkan user akan dicek apakah valid atau tidak. Hal tersebut kami lakukan dengan memanggil fungsi dibawah ini
```c
if(wrongCommand != 1){
    char namaSender[10000];
    if(id_user == 0){
        strcpy(namaSender, "root");
    }else{
        strcpy(namaSender, argv[2]);
    }
    writelog(copyinput, namaSender);
}
```
jika perintah yang dimasukkan user valid dan bukan perintah yang salah, maka kita cek siapa yang menjalankan perintah dengan menggunakan UID. jika 0 maka root, selain itu ambil nama user dari argv[2]. setelah itu panggil fungsi writelog. fungsi writelog adalah sebagai berikut
```c
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
```
dari kode diatas dapat dilihat bahwa kami menyimpan log nya itu untuk setiap user yang menjalankannya. hal yang kami write ke dalam file itu sesuai dengan format yang sudah diatur atau di tentukan pada ketentuan FP.
## 16. Reliability
Ketika user menjalankan perintah tersebut maka kami akan memasukkan log yang dipunyai oleh user, kedalam file yang telah ditentukan oleh user, berikut adalah kode yang kami gunakan.
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
```
Hal yang kami lakukan adalah mengecek apakah user boleh menggunakan program ini atau tidak. Menggunakan cara yang sama dengan program yang sebelumnya dimana untuk setiap user akan dicek apakah allowed atau tidak(sama dengan yang sudah dijelaskan sebelumnya). Setelah itu kita buka file log yang dipunyai user. Lalu baca log tersebut, ketika user menggunakan database yang sama dengan yang diinputkan user, maka salin semua perintah yang dijalankan user sampai mengganti current directory kedalam file yang telah dibuat atau ditentukan oleh user.

## 17. Error Handling
Untuk error handling sudah dijelaskan bersaman dengan dijalankannya perintah lain

## Kendala Pengerjaan 
1. Waktu yang sedikit dan banyak nya tugas membuat tidak bisa maksimal dalam mengerjakannya.
