#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_DIR_LEN 512
#define MAX_FILE_LENGTH 128

#include "betterexec.h"
#include "registeration.h" //depends on betterexec
#include "extra.h"
#include "system.h" //depends on betterexec and extra
#include "scrape.h"

int integrate(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]);
int update(char file[MAX_FILE_LENGTH], char dir[18]);

int main(int argc, char* argv[]) {

    if(geteuid() != 0) {
        printf("You need to be root to use this program, as it will edit system wide programs.\n");
        exit(1); //return 1
    }

    if(strcmp(argv[1], "-U\0") == 0)
        return integrate(argv[2], getdir("/etc/neptune/dir"));
    else if(strcmp(argv[1], "-R\0") == 0)
        return delete(argv[2], getdir("/etc/neptune/dir"));
    else if(strcmp(argv[1], "-Sy\0") == 0)
        return update(argv[2], "/etc/neptune/apps");
    else {
        printf("404, Internal Command not found.\n");
        return 4;
    }

}
int integrate(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {

    char filenamecp[MAX_FILE_LENGTH];
    char finalfile[MAX_DIR_LEN];
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


    strcpy(finalfile, "/etc/neptune/apps/");
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

    if (sexecl("/etc/neptune/bin/nep", "-e", finalfile, NULL)) {
        printf("Desktop file not found.");
        return 0; //I'm going to start returning 0 so long as the program finishes
    }
    
    strcpy(finalfile, dir);
    strcat(finalfile, "/");
    strcat(finalfile, ptr);
    strcat(finalfile, ".desktop");

    sexecl("/bin/cp", getdir("/tmp/filepath"), finalfile, NULL);
    chmod(finalfile, 0755);
    remove("/tmp/filepath");

    return 0;
}

int delete(char file[MAX_FILE_LENGTH], char dir[MAX_DIR_LEN]) {
    char *real = combine(dir, file, 1);
    char *real2 = combine(real, ".desktop", 1);
    char *link = combine("/etc/neptune/bin/", file, 0);
    char *app = combine("/etc/neptune/apps/", file, 0);
    char *cache = combine("/etc/neptune/cache/", file, 0);
    printf("Deregistering from system.\n");
    system_wide_unregistration(link, NULL);
    remove(link);
    unregisterApp("/etc/neptune/list", file);
    remove(real2);
    if (rename(app, cache) == 0)
        printf("Success\n");
    else
        perror("Unable to cache the file\n");
    free(link); //holy shit properly using my memory? what am I some kind of kernel dev??
    free(real);
    free(app);
    free(real2);
    free(cache);
    return 0;
}

int update(char file[MAX_FILE_LENGTH], char dir[18]) {

    const char *old = combine(file, ".zs-old", 0);
    sexecl(combine(getenv("APPDIR"), "/usr/bin/appimageupdatetool-x86_64.AppImage", 0), "-O", combine(dir, file, 1), NULL);
    if( access(combine(dir, old, 1), F_OK ) == 0 ) 
        sexecl("/bin/rm", combine(dir, old, 1), NULL, NULL);
    return 0;
}