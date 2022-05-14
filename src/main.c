#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <appimage/appimage.h>

#define MAX_DIR_LEN 512
#define MAX_FILE_LENGTH 128

#include "betterexec.h"
#include "registeration.h" //depends on betterexec
#include "extra.h"
#include "system.h" //depends on betterexec and extra
#include "checkroot.h"
#include "scrape.h"

int help();
int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int integrate(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int update(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int run(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);

const char *getFileExtension(const char *filename);

int main(int argc, char* argv[]) {

    if(argc == 1) 
        return help();

    else {
        if(strcmp(argv[1], "help\0") == 0) 
            return help();
        else if(strcmp(argv[1], "list\0") == 0)  {
            if(!access("/etc/Neptune/list", F_OK ))
                return sexecl("/bin/cat", "/etc/Neptune/list", NULL, NULL);
            else
                return 6;
        }
        else if(strcmp(argv[1], "install\0") == 0) {
            checkroot();
            return install(argv[2], getdir("/etc/Neptune/dir"));
        }
        else if(strcmp(argv[1], "remove\0") == 0) {
            checkroot();
            return delete(argv[2], getdir("/etc/Neptune/dir"));
        }
        else if(strcmp(argv[1], "update\0") == 0) {
            checkroot();
            if(argc == 2) {
                return sexecl(combine(getenv("APPDIR"), "git.sh", 1), NULL, NULL, NULL);
                //git pull from appimage.github.io and download data folder
            }
            return update(argv[2], getdir("/etc/Neptune/dir"));
        }
        else if(strcmp(argv[1], "find\0") == 0) {
            char cmd[256];
            sprintf(cmd, "ls /etc/Neptune/data | grep \"^%s\"", argv[2]);
            return system(cmd);
        }
        else if(strcmp(argv[1], "run\0") == 0) 
            return run(argv[2], getdir("/etc/Neptune/dir"));
        else if(appimage_get_type(argv[1], 0) != -1) {
            checkroot();
            return install(argv[1], getdir("/etc/Neptune/dir"));
        }
        else if(strcmp(argv[1], "--install") == 0)
            return sexecl(combine(getenv("APPDIR"), "/usr/bin/installer", 0), "--install", NULL, NULL);
        else if(strcmp(argv[1], "--uninstall") == 0)
            return execl(combine(getenv("APPDIR"), "/usr/bin/installer", 0), getenv("APPIMAGE"), "--uninstall", NULL);
        else {
            help();
            return 4;
        }
        return 0;
    }

}

int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {
    if(!access(combine(getenv("OWD"), file, 1), F_OK ))
        return integrate(combine(getenv("OWD"), file, 1), dir);
    else if(!access(combine("/etc/Neptune/data/", file, 0), F_OK )) {
        char cmd[2048];
        sprintf(cmd, "/usr/bin/wget -i %s -q --show-progress -O %s", combine("/etc/Neptune/data/", file, 0), combine("/tmp/", file, 0));
        system(cmd);
        if(!access(combine("/tmp/", file, 0), F_OK )) {
            struct stat st;
            stat(combine("/tmp/", file, 0), &st);
            if(st.st_size > 0)
                return integrate(combine("/tmp/", file, 0), dir);
            else {
                printf("File download failed.\n");
                remove(combine("/tmp/", file, 0));
                printf("Contents of %s database file:\n", file);
                return sexecl("/bin/cat", combine("/etc/Neptune/data/", file, 0), NULL, NULL);
            }
        }
        else {
            printf("File download failed.\n");
            return 6;
        }
    }
    else if(strcmp(getFileExtension(file), "AppImage") == 0) {
        printf("No file of that name (%s) found.\n", file);
        return 5;
    }
    else {
        printf("No program found in database. ");
        printf("If you have not updated it in a while or this is your accessing it, run sudo nep update to update your local program database.\n");
        return 0;
    }
}

int integrate(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    if (appimage_get_type(file, 0) == -1) {
        printf("This file is not an AppImage.\n");
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
    ptr = ptr + 1;
    if (ptr == NULL)
        ptr = file;

    strcpy(finalfile, dir);
    strcat(finalfile, "/");
    strcat(finalfile, ptr);

    sexecl("/bin/mv", filenamecp, finalfile, NULL);
    
    printf("Registering into system.\n");
    system_wide_registration(finalfile, ptr);
    registerApp(ptr);

    return 0;
}

int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {
    char newfile[MAX_DIR_LEN+MAX_FILE_LENGTH+1];
    strcat(newfile, combine(dir, file, 1));
    printf("Deregistering from system.\n");
    system_wide_unregistration(newfile, NULL);
    unregisterApp("/etc/Neptune/list", file);
    if (remove(newfile) == 0)
        printf("Success\n");
    else
        perror("Unable to delete the file\n");
    return 0;
}

int update(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    if (appimage_get_type(combine(dir, file, 1), 0) == -1) {
        printf("This file is not an AppImage.");
        return 2;
    }

    const char *old = combine(file, ".zs-old", 0);
    sexecl(combine(getenv("APPDIR"), "/usr/bin/appimageupdatetool-x86_64.AppImage", 0), "-O", combine(dir, file, 1), NULL);
    if( access(combine(dir, old, 1), F_OK ) == 0 ) 
        sexecl("/bin/rm", combine(dir, old, 1), NULL, NULL);
    return 0;
}

int run(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {
    
    char *location = combine(getdir("/etc/Neptune/userdata"), file, 1);
    
    char cmd[2048];
    sprintf(cmd, "aisap-0.6.4-alpha-x86_64.AppImage --profile %s/metadata/permissions.ini --data-dir %s/apphome %s/appcopy/%s", location, location, location, file);
    free(location); // we stan proper memory management 

    return system(cmd);
}

int help() {
    printf("Commands:\n");
    printf("install - installs a program\n");
    printf("update - updates an appimage if availible. If run with no arguments it updates Neptune's download database\n");
    printf("remove - removes a program\n");
    printf("find - searches for a program in Neptune's database\n");
    printf("list - lists current apps.\n");
    printf("help - displays help menu\n");
    printf("--install - installs Neptune\n");
    printf("--uninstall - uninstalls Neptune\n");
    return 0;
}

const char *getFileExtension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

