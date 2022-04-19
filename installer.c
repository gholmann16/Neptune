#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "src/registeration.h"
#include "src/checkroot.h"
#include "src/extra.h"

#define MAX_DIR_LEN 512

int install();
int uninstall();
char *getdir();
int help() {
    printf("Commands:\n");
    printf("--install\tinstalls Neptune\n");
    printf("--uninstall\tuninstall Neptune\n");
}

int main(int argc, char* argv[]) {

    if(argc == 1) {
        help();
        return 0;
    }

    else {
        if(strcmp(argv[1], "--install\0") == 0) {
            checkroot();
            return install();
        }
        else if(strcmp(argv[1], "--uninstall\0") == 0) {
            checkroot();
            return uninstall();
        }
        else {
            help();
            return 1;
        }
    }

}

int install() {

    //Essentially if data dir does not exist, install Neptune

    mkdir("/etc/Neptune", 0755);
    FILE *dirFile = fopen("/etc/Neptune/dir", "w");

    char patha[MAX_DIR_LEN + 10] = "export PATH=";

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

    char resting0[MAX_DIR_LEN + 64];
    strcpy(resting0, answer);
    strcat(resting0, "/Neptune.home/");

    mkdir(resting0, 0755);
    strcat(resting0,".local");

    sexecl("/bin/ln", "-s", "/usr/", resting0);

    //take packages out and install them
    //delete installer
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/Neptune-x86_64.AppImage", 0), combine(answer, "/Neptune", 0), NULL);
    sexecl("/bin/cp", combine(getenv("APPDIR"), "/appimageupdatetool-x86_64.AppImage", 0), combine(answer, "/appimageupdatetool", 0), NULL);

    char resting[MAX_DIR_LEN + 8];
    strcpy(resting, answer);
    strcat(resting, "/Neptune");
    
    sexecl("/bin/ln", "-s", resting, "nep");
    sexecl("/bin/mv", "nep", "/usr/local/bin", NULL);

    printf("Shortcut nep created. Type \'nep\' to start program anywhere.\n");
    printf("NOTE: Program \"Neptune\" might not be accessible by sudo due to sudo not using the $PATH variable. Command: nep will most likely work everywhere.\n");
    printf("To uninstall Neptune use \"sudo ./Neptune.Installer-x86_64.AppImage --uninstall\"\n");
    
    return 0;
}

int uninstall() {

    char patha[MAX_DIR_LEN + 10] = "export PATH=";
    strcat(patha, getdir());
    strcat(patha, ":$PATH");

    unregisterApp("/etc/profile", patha);

    strcpy(patha, getdir());
    strcat(patha, "/Neptune");
    remove(patha);

    //Leaves directory in case user has apps there

    system("rm -rf /etc/Neptune");
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