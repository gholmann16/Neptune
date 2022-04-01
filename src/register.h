#include <stdio.h>

int registerApp(char file[]) {
    FILE *list = fopen("/etc/Neptune/list", "a");
    
    fprintf(list, "%s\n", file);
    fclose(list);
}