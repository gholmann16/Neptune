#include <stdio.h>
#include <string.h>
#include <appimage/appimage.h>
int main(int argc, char *argv[]) {
    if(strcmp(argv[1], "-i\0") == 0) 
        return appimage_register_in_system(argv[2], 0);
    else if(strcmp(argv[1], "-u\0") == 0) 
        return appimage_unregister_in_system(argv[2], 0);
}