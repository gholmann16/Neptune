#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h> 

#include <gtk/gtk.h>
#include <neptune.h>

#define MAX_DIR_LEN 512
#define MAX_FILE_LENGTH 256 //might wanna look at these values in the future

#include "include.h"
#include "installer.h"
#include "store.h"
#include "gui.h"
#include "application.h"

int installed();
int help();

int main(int argc, char* argv[]) {

    char path[MAX_FILE_LENGTH];
    realpath("/proc/self/exe", path);
    char * ptr = strrchr(argv[0], '/');
    if (ptr == NULL)
        ptr = argv[0];
    else
        ptr = ptr + 1;
    char * ptr2 = strrchr(path, '/');
    if (ptr2 == NULL)
        ptr2 = path;
    else
        ptr2 = ptr2 + 1;

    if(installed()) install_preferences();
    if(strcmp(ptr, ptr2) != 0) return run(ptr, argc, argv);
    else if(argc == 1) return self();
    else if(strcmp(argv[1], "help\0") == 0) return help();
    else if(strcmp(argv[1], "list\0") == 0) return list();
    else if(strcmp(argv[1], "install\0") == 0) return install(argv[2]);
    else if(strcmp(argv[1], "remove\0") == 0) return destroy(argv[2]);
    else if(strcmp(argv[1], "uninstall\0") == 0) return uninstall(argv[2]);    
    else if(strcmp(argv[1], "reinstall\0") == 0) return reinstall(argv[2]);
    else if(strcmp(argv[1], "update\0") == 0) return update(argc, argv[2]);
    else if(strcmp(argv[1], "find\0") == 0) return find(argv[2]);
    else if(strcmp(argv[1], "upgrade\0") == 0) return upgrade();
    else if(strcmp(argv[1], "permissions\0") == 0) return self();
    else if(strcmp(argv[1], "--uninstall\0") == 0) return remove_preferences();
    else if(appimage_get_type(argv[1], 0) != -1) return gui(argv[1]);
    else return help();

    return 0;

}

int installed() {
    char data[MAX_DIR_LEN];
    strcpy(data, getenv("HOME"));
    strcat(data, "/.config/neptune/dir");
    return access(data, F_OK);
}

int help() {
    printf("Commands:\n");
    printf("install - installs a program\n");
    printf("update - updates an appimage if availible. If run with no arguments it updates Neptune's download database\n");
    printf("remove - removes a program\n");
    printf("uninstall - removes a program and all it's data! Only use if you know what you're doing.\n");
    printf("find - searches for a program in Neptune's database\n");
    printf("list - lists current apps.\n");
    printf("help - displays help menu\n");
    printf("--uninstall - uninstalls local Neptune data\n");
    return 0;
}
