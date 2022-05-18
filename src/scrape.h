char *getdir(char pdir[]) {

    if(access(pdir, F_OK )) {
        printf("You have not selected your directory preferences yet. Run nep --install to install Neptune to your system");
        exit(1);
    }
    
    static char dir[MAX_DIR_LEN];
    FILE *fp = fopen(pdir, "r");
    while (fgets(dir, sizeof(dir), fp)) 
        ;
    fclose(fp);
    return dir;
}