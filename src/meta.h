int defineDir(char *file, char *defo, char *message, char *starter, bool start) {

    FILE *dirFile = fopen(file, "w");
    char answer[MAX_DIR_LEN];

    printf("Where would you like to store %s Default is %s%s\n%s", message, starter, defo, starter);
    fgets(answer, sizeof answer, stdin);
    if (answer[0] == '\n') {
        printf("Storing to %s%s directory.\n", starter, defo);
        if (start) {
            char *savedir = combine(starter, defo, 0);
            mkdir(savedir, 0755);
            fprintf(dirFile, "%s", savedir);
            free(savedir);
        }
        else {
            mkdir(defo, 0755);
            fprintf(dirFile, "%s", defo);
        }
    }
    else {
        answer[strcspn(answer, "\n")] = 0;
        printf("Valid directory dected, saving prefrences.\n");
        if(start) {
            char *newdir = combine(starter, answer, 0);
            mkdir(newdir, 0755);
            fprintf(dirFile, "%s", newdir);
            free(newdir);
            }
        else {
            mkdir(answer, 0755);
            fprintf(dirFile, "%s", answer);
        }
    }

    fclose(dirFile);
    return 0;
}

int nepinstall() {

    //Essentially if data dir does not exist, install Neptune

    mkdir("/etc/neptune", 0755);
    mkdir("/etc/neptune/bin", 0755);
    mkdir("/etc/neptune/apps", 0755);
    mkdir("/etc/neptune/cache", 0755);
    mkdir("/etc/neptune/data", 0755);
    defineDir("/etc/neptune/dir", "opt", "applications installed by this program?", "/", 1);
    defineDir("/etc/neptune/userdata", ".AppImageData", "data from AppImages?", "~/", 0);

    FILE *path = fopen("/etc/profile.d/neptune.sh", "w");
    fprintf(path, "export PATH=/etc/neptune/bin:$PATH");
    fclose(path);
    
    if(rename(getenv("APPIMAGE"), "/etc/neptune/bin/nep")) {
        perror("Rename fails with");
        printf("Couldn't rename %s.\n", getenv("APPIMAGE"));
    }
    printf("Open a new bash shell to get /etc/neptune/bin/nep on your path.\n");
    printf("To uninstall Neptune use \"nep --uninstall\"\n");

}

int nepuninstall() {

    remove("/etc/profile.d/neptune.sh");

    //Leaves directory in case user has apps there
    //this function needs to be fixed lfmaoooo

    remove("/etc/neptune/bin/nep");
    remove("/etc/neptune/dir");
    remove("/etc/neptune/userdata");
    system("rm -rf /etc/neptune/cache");
    system("rm -rf /etc/neptune/data");
    
    return 0;
}