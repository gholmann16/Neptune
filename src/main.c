#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <appimage/appimage.h>
#include "registeration.h"
#include "checkroot.h"

#define MAX_FILE_LENGTH 128
#define MAX_DIR_LEN 512

int help();
int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);\
//int test(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);

char *getfile(char *argv);
char *getdir();
int checkroot();

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
        else if(strcmp(argv[1], "install\0") == 0) {
            checkroot();
            return install(argv[2], getdir());
        }
        else if(strcmp(argv[1], "remove\0") == 0) {
            checkroot();
            return delete(argv[2], getdir());
        }
        else if(appimage_get_type(argv[2], 0) != -1) {
            checkroot();
            return install(argv[2], getdir());
        }
        else {
            help();
            return 4;
        }
        return 0;
    }

}

int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    if (appimage_get_type(file, 0) == -1) {
        printf("This file is not an AppImage.");
        return 2;
    }

    char filenamecp[MAX_FILE_LENGTH];
    char finalfile[MAX_DIR_LEN+MAX_FILE_LENGTH];
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
    
    int index;

    ptr = strrchr(file, '/');
    if (ptr == NULL)
        ptr = file;

    strcpy(finalfile, dir);
    strcat(finalfile, "/");
    strcat(finalfile, ptr);

    sexecl("/bin/mv", filenamecp, finalfile, NULL);
    
    printf("Registering into system.\n");
    appimage_register_in_system(finalfile, 0);
    registerApp(file);

    return 0;
}

int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    printf("Deregistering from system.\n");
    char cmd[MAX_FILE_LENGTH + MAX_DIR_LEN + 2];
    sprintf(cmd, "%s/%s", dir, file);
    if (remove(cmd) == 0) {
        printf("Deleted successfully.\n");
        unregisterApp("/etc/Neptune/list", file);
    }
    else
        printf("Unable to delete the file.\n");

    return 0;
}

int help() {
    printf("Commands:\n");
    printf("install - installs a program\nremove - removes a program\nhelp - displays help menu\nlist - lists current apps.\n");
}

char *getdir() {
    char pdir[64] = "/etc/Neptune/dir";
    static char dir[MAX_DIR_LEN];
    FILE *fp = fopen(pdir, "r");                 // do not use "rb"
    while (fgets(dir, sizeof(dir), fp)) {
        printf("Modifying Files.\n");
    }
    fclose(fp);
    return dir;
}