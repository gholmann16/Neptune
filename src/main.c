#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "betterexec.h"
#include "deleteline.h"
#include "register.h"

#define MAX_FILE_LENGTH 128
#define MAX_DIR_LEN 256

int help();
int list();
int start(int c);
int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
const char *getFileExtension(const char *filename);

int main(int argc, char* argv[]) {

    if(argc == 1) {
        help();
        return 0;
    }

    else {
        if(strcmp(argv[1], "help\0") == 0) 
            help();
        else if(strcmp(argv[1], "list\0") == 0)  {
            if(!access("/etc/Neptune/list", F_OK ))
                system("cat /etc/Neptune/list");
        }
        else if((strcmp(argv[1], "install\0") == 0) || (strcmp(argv[1], "remove\0") == 0)) {
            
            if(geteuid() != 0) {
                printf("You need to be root to use this program, as it will edit system wide programs.\n");
                return 1;
            }
            
            char pdir[64] = "/etc/Neptune/dir";
            char dir[MAX_DIR_LEN];
            FILE *fp = fopen(pdir, "r");                 // do not use "rb"
            while (fgets(dir, sizeof(dir), fp))
                printf("Modifying Files.\n");
            fclose(fp);
            char file[MAX_FILE_LENGTH];
            strcpy(file, argv[2]);

            if(strcmp(argv[1], "install\0") == 0) {
                return install(file, dir);
            }
            else if(strcmp(argv[1], "remove\0") == 0) {
                return delete(file, dir);
            }
        }
        else { 
            help();
            return 4;
        }
        return 0;
    }

}

int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    if (strcmp(getFileExtension(file), "AppImage") != 0) {
        printf("This file is not an AppImage.");
        return 2;
    }
    
    char filenamecp[MAX_FILE_LENGTH];
    char finalfile[MAX_FILE_LENGTH + MAX_DIR_LEN] = "";
    strcpy(filenamecp, file);

    chown(file, 0, 0);
    chmod(file, 0755);

    // Don't add part after -
    char *ptr;

    ptr = strchr(file, '-');
    if (ptr != NULL)
        *ptr = '\0';
    else 
        file[strlen(file)-9] = '\0';

    strcat(finalfile, dir);
    strcat(finalfile, "/");
    strcat(finalfile, file);

    sexecl("/bin/mv", filenamecp, finalfile, NULL);
    
    registerApp(file);

    return 0;
}

int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    char cmd[MAX_FILE_LENGTH + MAX_DIR_LEN + 2];
    sprintf(cmd, "%s/%s", dir, file);
    if (remove(cmd) == 0) {
        printf("Deleted successfully.\n");
        rmln("/etc/Neptune/list", file);
    }
    else
        printf("Unable to delete the file.\n");

    return 0;
}

int help() {
    printf("Commands:\n");
    printf("install - installs a program\nremove - removes a program\nhelp - displays help menu\nlist - lists current apps.\n");
}

const char *getFileExtension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
