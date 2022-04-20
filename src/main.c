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
#include "extra.h"

#define MAX_FILE_LENGTH 128
#define MAX_DIR_LEN 512

int help();
int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int integrate(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int update(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);

char *getdir();
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
                return system("cat /etc/Neptune/list");
        }
        else if(strcmp(argv[1], "install\0") == 0) {
            checkroot();
            return install(argv[2], getdir());
        }
        else if(strcmp(argv[1], "remove\0") == 0) {
            checkroot();
            return delete(argv[2], getdir());
        }
        else if(strcmp(argv[1], "update\0") == 0) {
            checkroot();
            if(argc == 2) {
                return sexecl(combine(getenv("APPDIR"), "git.sh", 1), NULL, NULL, NULL);
                //git pull from appimage.github.io and download data folder
            }
            return update(argv[2], getdir());
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
    if(!access(file, F_OK ))
        return integrate(file, dir);
    else if(!access(combine("/etc/Neptune/data/", file, 0), F_OK )) {
        char cmd[2048];
        sprintf(cmd, "/bin/wget -i %s -q --show-progress -O %s", combine("/etc/Neptune/data/", file, 0), combine("/tmp/", file, 0));
        system(cmd);
        if(!access(combine("/tmp/", file, 0), F_OK )) {
            return integrate(combine("/tmp/", file, 0), dir);
        }
        else {
            printf("File download fail.\n");
            return 6;
        }
    }
    else if(strcmp(getFileExtension(file), "AppImage") == 0) 
        printf("No file of that name found.\n");
    else {
        printf("No program found in database. ");
        printf("If you have not updated it in a while or this is your accessing it, run sudo nep update to update your local program database.\n");
    }
}

int integrate(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {
    
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
    else if(strcmp(getFileExtension(file), "AppImage") == 0)
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
    appimage_unregister_in_system(combine(dir, file, 1), 0);
    if (remove(combine(dir, file, 1)) == 0) {
        printf("Deleted successfully.\n");
        unregisterApp("/etc/Neptune/list", file);
    }
    else
        printf("Unable to delete the file.\n");

    return 0;
}

int update(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    if (appimage_get_type(combine(dir, file, 1), 0) == -1) {
        printf("This file is not an AppImage.");
        return 2;
    }

    const char *old = combine(file, ".zs-old", 0);
    sexecl(combine(dir, "/appimageupdatetool", 0), "-O", combine(dir, file, 1), NULL);
    if( access(combine(dir, old, 1), F_OK ) == 0 ) 
        sexecl("/bin/rm", combine(dir, old, 1), NULL, NULL);
}

int help() {
    printf("Commands:\n");
    printf("install - installs a program\n");
    printf("update - updates an appimage if availible.\n");
    printf("remove - removes a program\n");
    printf("help - displays help menu\n");
    printf("list - lists current apps.\n");
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

const char *getFileExtension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

