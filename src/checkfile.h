int check(char file[MAX_FILE_LENGTH]) {
    if (appimage_get_type(file, VERBOSE) == -1) {
        printf("This file is not an AppImage.\n");
        return 2;
    }
    else 
        return 0;
}