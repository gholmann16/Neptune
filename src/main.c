#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include <appimage/appimage.h>
#include <iup/iup.h>

#define MAX_DIR_LEN 512
#define MAX_FILE_LENGTH 256 //might wanna look at these values in the future
#define VERBOSE 0

#include "scrape.h"
#include "betterexec.h"
#include "extra.h"
#include "checkfile.h"

#include "commands/install.h"
#include "commands/remove.h"
#include "commands/list.h"
#include "commands/internal.h"

#include "gui.h"

int help();
int run(char file[MAX_FILE_LENGTH], int arg, char* argv[]);
int check(char file[MAX_FILE_LENGTH]);
int check_if_registered(char line[]);

int main(int argc, char* argv[]) {

    char * ptr = strrchr(getenv("ARGV0"), '/');
    if (ptr == NULL) 
        ptr = getenv("ARGV0");
    else 
        ptr = ptr + 1;

    if(argc == 1) return run(ptr, 0, argv);

    //If not running Neptune, run program in sandbox (with args)
    if (strcmp(ptr, "nep") != 0 && strcmp(ptr, "Neptune-x86_64.AppImage") != 0) return run(ptr, argc, argv);
    
    if(strcmp(argv[1], "help\0") == 0) return help();
    else if(strcmp(argv[1], "list\0") == 0) return list();
    else if(strcmp(argv[1], "install\0") == 0) return install(argv[2]);
    else if(strcmp(argv[1], "remove\0") == 0) return nepremove(argv[2]);
    else if(strcmp(argv[1], "reinstall\0") == 0) {
        sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/local/bin/neproot", 0), "/tmp/neproot", NULL);
        sexecl("/usr/bin/pkexec", "/tmp/neproot", "-R", argv[2]);
        sexecl("/usr/bin/pkexec", "/tmp/neproot", "-U", combine("/etc/neptune/cache/", argv[2], 0));
        remove("/tmp/neproot");
        return 0;
    }
    else if(strcmp(argv[1], "update\0") == 0) {
        
        int ret;

        if(argc == 2) {
            sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/local/bin/git.sh", 0), "/tmp/neproot", NULL);
            ret = sexecl("/usr/bin/pkexec", "/tmp/neproot", NULL, NULL);
            remove("/tmp/neproot");
            //git pull from appimage.github.io and download data folder
        }
        else {
            char *file = combine("/etc/neptune/apps", argv[2], 1);
            ret = 2;
            if (check(file)) {
                sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/local/bin/neproot", 0), "/tmp/neproot", NULL);
                ret = sexecl("/usr/bin/pkexec", "/tmp/neproot", "-Sy", argv[2]); //later add compaitibility for multiple packages
            }
            remove("/tmp/neproot");
            free(file);
        }

        return ret;
    }
    else if(strcmp(argv[1], "find\0") == 0) {
        char cmd[256];
        sprintf(cmd, "ls /etc/neptune/data | grep \"^%s\"", argv[2]);
        return system(cmd);
    }
    else if(strcmp(argv[1], "-i\0") == 0) {

        if (strcmp(getenv("HOME"), "/") == 0) {
            return 10; //this is for nobody user
        }

        if (appimage_is_registered_in_system(argv[2]))
            appimage_unregister_in_system(argv[2], VERBOSE); //in case you uninstall and reinstall Neptune
        
        appimage_register_in_system(argv[2], VERBOSE);
        char rlocaldata[MAX_DIR_LEN];
        char localdata[MAX_DIR_LEN];
        strncpy(rlocaldata, getdir("/etc/neptune/userdata"), MAX_DIR_LEN-1);
        rlocaldata[MAX_DIR_LEN] = '\0';
        for (int i = 0; i < strlen(rlocaldata); i++)
            localdata[i] = rlocaldata[i+2]; //could be problematic
        char* full = combine(getenv("HOME"), localdata, 1);
        mkdir(full, 0700);
        chdir(full);
        free(full);
        DIR* dir = opendir(argv[3]);

        if (dir) {
            /* Directory exists. */
            closedir(dir);
            printf("App already installed on this user, skipping installation.\n");
            return 9;
        } 
        
        else {
            mkdir(argv[3], 0700);
            chdir(argv[3]);
            mkdir("metadata", 0700);
            mkdir("apphome", 0700);
            // mkdir("appcopy", 0700);
            
            FILE *perms = fopen("metadata/permissions.ini", "w");
            fprintf(perms, "[X-App Permissions]\n");
            fprintf(perms, "Level=2\n");
            fprintf(perms, "Files=xdg-desktop;xdg-download:rw;\n");
            fprintf(perms, "Devices=dri;\n");
            fprintf(perms, "Sockets=x11;wayland;pulseaudio;network;\n");
            fclose(perms);

            // link(combine("appcopy/", argv[3], 0), combine(getdir("/etc/neptune/dir"), argv[3], 1));
            // the link is to make it easy to transfer these appimages
            // might implement some more functionality after I make sandboxing mandatory
            return 0;
        }
        
    }
    else if(strcmp(argv[1], "-u\0") == 0) { //TODO: delete data dir 
        if (strcmp(getenv("HOME"), "/") == 0) {
            return 10; //this is for nobody user
        }
        return appimage_unregister_in_system(argv[2], VERBOSE);
    }
    else if (strcmp(argv[1], "-d\0") == 0) {
        printf("You should not have accessed this function.\n"); //uninstall data
        return 0;
    }
    else if (strcmp(argv[1], "-e\0") == 0) {
        char **array = appimage_list_files(argv[2]);
        int i = 0;
        while (array[i] != NULL) {
            if (strcmp(getFileExtension(array[i]), "desktop") == 0) {
                FILE *desktop = fopen("/tmp/filepath", "w");
                fprintf(desktop, "%s", array[i]);
                fclose(desktop);
                return 0;
            }
            i++;
        }
        return 1; //not found;
    }
    else if(strcmp(argv[1], "--install") == 0) {
        sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/installer", 0), "/tmp/installer", NULL);
        int ret = sexecl("/usr/bin/pkexec", "/tmp/installer", getenv("APPIMAGE"), "--install");
        remove("/tmp/installer");
        return ret;
    }
    else if(strcmp(argv[1], "--uninstall") == 0) {
        sexecl("/bin/cp", combine(getenv("APPDIR"), "/usr/bin/installer", 0), "/tmp/installer", NULL);
        int ret = sexecl("/usr/bin/pkexec", "/tmp/installer", getenv("APPIMAGE"), "--uninstall");
        remove("/tmp/installer");
        return ret;
    }
    else {
        char* appimage = combine(getenv("OWD"), argv[1], 1);
        if(appimage_get_type(appimage, VERBOSE) != -1) return gui(argc, argv, appimage);
        free(appimage);
        help();
        return 4;
    }

}

int check_if_registered(char line[]) {

    char buffer[MAX_FILE_LENGTH];

    FILE *file;
    file = fopen("/etc/neptune/list", "r");
    strcat(line, "\n");

    if (file == NULL)
    {
        printf("Error opening file(s).\n");
        return 0;
    }

    bool keep_reading = true;
    do {

        // stores the next line from the file into the buffer        
        fgets(buffer, MAX_FILE_LENGTH, file);

        // if we've reached the end of the file, stop reading from the file
        if (feof(file)) keep_reading = false;
        else if (strcmp(buffer, line) == 0) {
            return 1;
        }

    } while (keep_reading);

    fclose(file);
    return 0;
}


int run(char file[MAX_FILE_LENGTH], int argc, char * argv[]) {

    if(strcmp(file, "nep") == 0)
        return help();

    char dir1[MAX_DIR_LEN+1];
    char dir2[MAX_DIR_LEN+1];
    
    strncpy(dir1, "/etc/neptune/apps", MAX_DIR_LEN);
    strncpy(dir2, getdir("/etc/neptune/userdata"), MAX_DIR_LEN);
    dir1[MAX_DIR_LEN] = '\0';
    dir2[MAX_DIR_LEN] = '\0';
    
    char *program = combine(dir1, file, 1);
    char *location = combine(dir2, file, 1);
    int ret;

    if(!access(program, F_OK)) {
        char cmd[4096];
        sprintf(cmd, "aisap-0.6.6-alpha-x86_64.AppImage --profile %s/metadata/permissions.ini --data-dir %s/apphome %s", location, location, program);
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