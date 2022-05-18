#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "registeration.h"
#include "checkroot.h"
#include "extra.h"

#define MAX_DIR_LEN 512

int install();
int uninstall();
char *getdir();
int defineDir();
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
            if(geteuid() != 0) {
                printf("You need to be root to use this program, as it will edit system wide programs.\n");
                exit(1); //return 1
            }
            return install(argv);
        }
        else if(strcmp(argv[1], "--uninstall\0") == 0) {
            if(geteuid() != 0) {
                printf("You need to be root to use this program, as it will edit system wide programs.\n");
                exit(1); //return 1
            }
            return uninstall(argv);
        }
        else {
            help();
            return 1;
        }
    }

}

int install(char* argv[]) {

    //Essentially if data dir does not exist, install Neptune

    mkdir("/etc/neptune", 0755);
    mkdir("/etc/neptune/bin", 0755);
    defineDir("/etc/neptune/dir", "/Applications", "applications installed by this program", "/");
    defineDir("/etc/neptune/userdata", "~/.AppImageData", "data from AppImages?", "~/");

    FILE *path = fopen("/etc/profile.d/neptune.sh", "w");
    fprintf(path, "export PATH=/etc/neptune/bin:$PATH");
    fclose(path);

    rename(argv[0], "/etc/neptune/bin/nep");

    printf("Open a new bash shell to get /etc/neptune/bin/nep on your path.\n");
    printf("To uninstall Neptune use \"sudo nep --uninstall\"\n");
    
    return 0;
}

int uninstall(char* argv[]) {

    remove("/etc/profile.d/neptune.sh");
    remove(argv[0]);

    //Leaves directory in case user has apps there

    system("rm -rf /etc/neptune");
}

char *getdir() {
    char pdir[64] = "/etc/neptune/dir";
    static char dir[MAX_DIR_LEN];
    FILE *fp = fopen(pdir, "r");                 // do not use "rb"
    while (fgets(dir, sizeof(dir), fp)) {
        printf("Modifying Files.\n");
    }
    fclose(fp);
    return dir;
}

int defineDir(char *file, char *defo, char *message, char *starter) {

    FILE *dirFile = fopen(file, "w");
    char answer[MAX_DIR_LEN];

    printf("Where would you like to store %s Default is %s\n%s", message, defo, starter);
    fgets(answer, sizeof answer, stdin);
    if (answer[0] == '\n') {
        printf("Storing to %s directory.\n", defo);
        mkdir(defo, 0755);
        fprintf(dirFile, "%s", defo);
    }
    else {
        answer[strcspn(answer, "\n")] = 0;
        printf("Valid directory dected, saving prefrences.\n");
        mkdir(combine(starter, answer, 0), 0755);
        fprintf(dirFile, "%s", combine(starter, answer, 0));
    }

    fclose(dirFile);
    return 0;
}