#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <wait.h>
#include <time.h>
#include <dirent.h>

void listFilesRecursively(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            remove(path);
            FILE* turnoff;
            turnoff = fopen(path, "w");
            fclose(turnoff);
            listFilesRecursively(path);
        }
    }

    closedir(dir);
}

int main() {
    pid_t pid, sid;        // Variabel untuk menyimpan PID
    pid_t child_pid,child_pid2;
    int status=0;
    pid = fork();     // Menyimpan PID dari Child Process

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    //kode template untuk proses daemon
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/home/kali/Desktop/fp")) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    int batas = 1;
    char waktu[30];
    char namazip[10000];
    time_t t1 = time(NULL);
    struct tm* p1 = localtime(&t1);
    strftime(waktu, 30, "%Y-%m-%d_%H:%M:%S", p1);

    while(1){
        // Tulis program kalian di sini
        child_pid2 = fork();
        if(child_pid2 == 0){
            snprintf(namazip, sizeof namazip, "%s.zip", waktu);
            char *zipAkhir[]= {"zip","-r", namazip,"database",NULL};
            execv("/bin/zip", zipAkhir); 
        }else{
            while ((wait(&status)) > 0);
            listFilesRecursively("database/log");
            // char *hapus[]= {"rm","-r","Fylm","Musyik","Pyoto",NULL};
            // execv("/bin/rm", hapus); 
            // exit(1);
        }
        sleep(30);
    }
}
