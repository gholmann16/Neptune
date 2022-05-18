#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <appimage/appimage.h>
#define MAX_DIR_LEN 512
#include "scrape.h"
#include "extra.h"

int main(int argc, char *argv[]) {

    if (strcmp(getenv("HOME"), "/") == 0) {
        return 10; //this is for nobody user
    }

    if(strcmp(argv[1], "-i\0") == 0) {

        if (appimage_is_registered_in_system(argv[2]))
            appimage_unregister_in_system(argv[2], 0); //in case you uninstall and reinstall Neptune
        
        appimage_register_in_system(argv[2], 0);
    
        char* localdata = getdir("/etc/neptune/userdata");
        memmove(localdata, localdata+2, strlen(localdata));  //remove ~/
        char* full;
        sprintf(full, "%s/%s", getenv("HOME"), localdata);
        mkdir(full, 0700);
        chdir(full);
        DIR* dir = opendir(argv[3]);

        if (dir) {
            /* Directory exists. */
            closedir(dir);
            printf("App already installed on this user, skipping installation.");
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
    else if(strcmp(argv[1], "-u\0") == 0) //TODO: delete data dir
        return appimage_unregister_in_system(argv[2], 0);
}