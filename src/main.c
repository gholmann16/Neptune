#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iup/iup.h>
#include <neptune.h>

#define MAX_DIR_LEN 512
#define MAX_FILE_LENGTH 256 //might wanna look at these values in the future

#include "include.h"
#include "gui.h"
#include "meta.h"

int help();
int escalate(char *argv[]);

int main(int argc, char* argv[]) {

    char * ptr = strrchr(getenv("ARGV0"), '/');
    if (ptr == NULL) 
        ptr = getenv("ARGV0");
    else 
        ptr = ptr + 1;

    
    if(argc == 1 && strcmp(ptr, "nep") != 0 && strcmp(ptr, "Neptune-x86_64.AppImage") != 0) return run(ptr, 0, argv);
    else if(argc == 1) return help();
    //If not running Neptune, run program in sandbox (with args)
    else if(strcmp(ptr, "nep") != 0 && strcmp(ptr, "Neptune-x86_64.AppImage") != 0) return run(ptr, argc, argv);
    else if(strcmp(argv[1], "help\0") == 0) return help();
    else if(strcmp(argv[1], "list\0") == 0) return list();
    else if(strcmp(argv[1], "install\0") == 0 && geteuid() == 0) return install(argv[2]);
    else if(strcmp(argv[1], "install\0") == 0) return escalate(argv);
    else if(strcmp(argv[1], "remove\0") == 0 && geteuid() == 0) return destroy(argv[2]);
    else if(strcmp(argv[1], "remove\0") == 0) return escalate(argv);
    else if(strcmp(argv[1], "uninstall\0") == 0 && geteuid() == 0) return uninstall(argv[2]);
    else if(strcmp(argv[1], "uninstall\0") == 0) return escalate(argv);    
    else if(strcmp(argv[1], "reinstall\0") == 0 && geteuid() == 0) return reinstall(argv[2]);
    else if(strcmp(argv[1], "reinstall\0") == 0) return escalate(argv);
    else if(strcmp(argv[1], "update\0") == 0 && geteuid() == 0) return update(argc, argv[2]);
    else if(strcmp(argv[1], "update\0") == 0) return escalate(argv);
    else if(strcmp(argv[1], "find\0") == 0) return find(argv[2]);
    else if(strcmp(argv[1], "--install") == 0) return nepinstall();
    else if(strcmp(argv[1], "--uninstall") == 0) return nepuninstall();
    else {
        char* appimage = combine(getenv("OWD"), argv[1], 1);
        if(appimage_get_type(appimage, 0) != -1) return gui(argc, argv, appimage);
        free(appimage);
        help();
        return 4;
    }

}

int escalate(char *argv[]) {
    printf("You require admin privaleges for this command.");
    if(sexecl("/usr/bin/pkexec", "/etc/neptune/bin/nep", argv[1], argv[2])) printf("Privalege escalation failed, run with su for it to work.");
    return 0;
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