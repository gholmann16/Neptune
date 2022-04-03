#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "src/register.h"
#include "src/betterexec.h"

#define MAX_DIR_LEN 4096

int main () {

    if(geteuid() != 0) {
        printf("You need to be root to use this program, as it will edit system wide programs.\n");
        return 1;
    }

    //Essentially if data dir does not exist, install Neptune

    mkdir("/etc/Neptune", 0755);
    FILE *dirFile = fopen("/etc/Neptune/dir", "w");

    char patha[MAX_DIR_LEN + 10] = "PATH=";

    printf("Directory prefrence hast not yet been set.\n");

    int invalid = 1;
    char answer[MAX_DIR_LEN];
    while(invalid) {
        printf("Where would you like to store applications installed by this program? Default is /Applications/\n");
        fgets(answer, sizeof answer, stdin);
        if (answer[0] == '\n') {
            printf("Storing to /Applications/ directory.\n");
            mkdir("/Applications", 0755);
            fprintf(dirFile, "/Applications");
            strcat(patha, "/Applications");
            strncpy(answer, "/Applications", MAX_DIR_LEN);
            invalid = 0;
        }
        else if (answer[0] == '/') {
            answer[strcspn(answer, "\n")] = 0;
            printf("Valid directory dected, saving prefrences.\n");
            mkdir(answer, 0755);
            fprintf(dirFile, "%s", answer);
            strcat(patha, answer);
            invalid = 0;
        }
        else {
            printf("Invalid directory, try again.\n");
        }
    }

    fclose(dirFile);

    strcat(patha, ":$PATH");

    FILE *path = fopen("/etc/profile", "a");
    fprintf(path, "%s\n", patha);
    fclose(path);

    //take packages out and install them
    //delete installer
    char cmd[8192];
    char cmd2[8192];

    sprintf(cmd, "cp %s/appimaged-x86_64.AppImage %s/appimaged", getenv("APPDIR"), answer);
    system(cmd);
    sprintf(cmd2, "cp %s/Neptune-x86_64.AppImage %s/Neptune", getenv("APPDIR"), answer);
    system(cmd2);

    registerApp("appimaged");
    registerApp("Neptune");

    char resting[MAX_DIR_LEN + 8];
    strcat(resting, answer);
    strcat(resting, "/Neptune");
    
    sexecl("/bin/ln", "-s", resting, "nep");
    sexecl("/bin/mv", "nep", "/usr/local/bin", NULL);

    printf("Shortcut nep created. Type \'nep\' to start program anywhere.\n");
    printf("NOTE: Program \"Neptune\" might not be accessible by sudo due to sudo not using the $PATH variable. Command: nep will most likely work everywhere.\n");
    
    printf("Deleting installer...\n");

    remove(getenv("APPIMAGE"));
    return 0;
}