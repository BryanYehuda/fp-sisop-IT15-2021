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

2. Kami membuat proses bagaimana cara root menggunakan kode server
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

2. Setelah membuat bagaimana cara root mengakses program, sekarang kami membuat proses bagaimana cara root untuk membuat CREATE USER
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

3. Membuat proses login user menggunakan program
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
