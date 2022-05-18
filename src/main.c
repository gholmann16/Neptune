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
int run(char file[MAX_FILE_LENGTH], int arg, char* argv[]);

const char *getFileExtension(const char *filename);

int main(int argc, char* argv[]) {

    char * ptr = strrchr(getenv("ARGV0"), '/');
    if (ptr == NULL) {
        ptr = getenv("ARGV0");
    }
    else {
    ptr = ptr + 1;
    }

    if(argc == 1) {
        char *array[1];
        return run(ptr, 0, array);
    }

    else {
        
        if (strcmp(ptr, "nep") == 0 || strcmp(ptr, "Neptune-x86_64.AppImage") == 0) {
            if(strcmp(argv[1], "help\0") == 0) 
                return help();
            else if(strcmp(argv[1], "list\0") == 0)  {
                if(!access("/etc/neptune/list", F_OK ))
                    return sexecl("/bin/cat", "/etc/neptune/list", NULL, NULL);
                else
                    return 6;
            }
            else if(strcmp(argv[1], "install\0") == 0) {
                checkroot();
                return install(argv[2], getdir("/etc/neptune/dir"));
            }
            else if(strcmp(argv[1], "remove\0") == 0) {
                checkroot();
                return delete(argv[2], getdir("/etc/neptune/dir"));
            }
            else if(strcmp(argv[1], "update\0") == 0) {
                checkroot();
                if(argc == 2) {
                    return sexecl(combine(getenv("APPDIR"), "git.sh", 1), NULL, NULL, NULL);
                    //git pull from appimage.github.io and download data folder
                }
                return update(argv[2], getdir("/etc/neptune/dir"));
            }
            else if(strcmp(argv[1], "find\0") == 0) {
                char cmd[256];
                sprintf(cmd, "ls /etc/neptune/data | grep \"^%s\"", argv[2]);
                return system(cmd);
            }
            else if(strcmp(argv[1], "run\0") == 0) {
                printf("Note: any arguments for this program will not be passed.\n");
                printf("To pass more arguments, use the program softlink found in path or /etc/neptune/bin.\n");
                char *array[1];
                return run(argv[2], 0, array);
            }
            else if(appimage_get_type(argv[1], 0) != -1) {
                checkroot();
                return install(argv[1], getdir("/etc/neptune/dir"));
            }
            else if(strcmp(argv[1], "--install") == 0)
                return execl(combine(getenv("APPDIR"), "/usr/bin/installer", 0), getenv("APPIMAGE"), "--install", NULL);
            else if(strcmp(argv[1], "--uninstall") == 0)
                return execl(combine(getenv("APPDIR"), "/usr/bin/installer", 0), getenv("APPIMAGE"), "--uninstall", NULL);
            else {
                help();
                return 4;
            }
        }
        else {
            return run(ptr, argc, argv);
        }
        return 0;
    }

}

int install(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {
    if(!access(combine(getenv("OWD"), file, 1), F_OK ))
        return integrate(combine(getenv("OWD"), file, 1), dir);
    else if(!access(combine("/etc/neptune/data/", file, 0), F_OK )) {
        char cmd[2048];
        sprintf(cmd, "/usr/bin/wget -i %s -q --show-progress -O %s", combine("/etc/neptune/data/", file, 0), combine("/tmp/", file, 0));
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
                return sexecl("/bin/cat", combine("/etc/neptune/data/", file, 0), NULL, NULL);
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
    if (ptr == NULL) 
        ptr = file; 
        // this used to be a much simpler function
        //idk if c library updated or sum but this shit is bloated
    else 
        ptr = ptr + 1;


    strcpy(finalfile, dir);
    strcat(finalfile, "/");
    strcat(finalfile, ptr);

    sexecl("/bin/mv", filenamecp, finalfile, NULL);
    
    printf("Registering into system.\n");

    char * softlink = combine("/etc/neptune/bin", ptr, 1);
    remove(softlink);
    symlink(finalfile, softlink);
    system_wide_registration(softlink, ptr);
    remove(softlink);
    symlink("/etc/neptune/bin/nep", softlink);
    free(softlink);
    registerApp(ptr);

    return 0;
}

int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {
    char *real = combine(dir, file, 1);
    char *link = combine("/etc/neptune/bin/", file, 0);
    printf("Deregistering from system.\n");
    system_wide_unregistration(link, NULL);
    remove(link);
    unregisterApp("/etc/neptune/list", file);
    if (remove(real) == 0)
        printf("Success\n");
    else
        perror("Unable to delete the file\n");
    free(link); //holy shit properly using my memory? what am I some kind of kernel dev??
    free(real);
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

int run(char file[MAX_FILE_LENGTH], int argc, char * argv[]) {

    if(strcmp(file, "nep") == 0)
        return help();

    char dir1[MAX_DIR_LEN+1];
    char dir2[MAX_DIR_LEN+1];
    
    strncpy(dir1, getdir("/etc/neptune/dir"), MAX_DIR_LEN);
    strncpy(dir2, getdir("/etc/neptune/userdata"), MAX_DIR_LEN);
    dir1[MAX_DIR_LEN] = '\0';
    dir2[MAX_DIR_LEN] = '\0';
    
    char *program = combine(dir1, file, 1);
    char *location = combine(dir2, file, 1);
    int ret;

    if(!access(program, F_OK)) {
        char cmd[4096];
        sprintf(cmd, "aisap-0.6.4-alpha-x86_64.AppImage --profile %s/metadata/permissions.ini --data-dir %s/apphome %s", location, location, program);
        if(argc > 0)
            for (int i = 1; i < argc; i++) {
                strcat(cmd, " ");
                strncat(cmd, argv[i], strlen(argv[i]));
                printf("cmd %d = %s\n", i, cmd);
            }
        printf("cmd = %s", cmd);
        ret = system(cmd);
    }
    else {
        printf("This program (%s) does not exist.\n", program);
        ret = 8;
    }

    free(location); // we stan proper memory management 
    free(program);
    return ret;
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

