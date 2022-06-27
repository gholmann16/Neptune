int download(char file[MAX_FILE_LENGTH]) {
    if(!access(combine("/etc/neptune/data/", file, 0), F_OK )) {
        char cmd[2048];
        sprintf(cmd, "/usr/bin/wget -i %s -q --show-progress -O %s", combine("/etc/neptune/data/", file, 0), combine("/tmp/", file, 0));
        system(cmd);
        if(!access(combine("/tmp/", file, 0), F_OK )) {
            struct stat st;
            stat(combine("/tmp/", file, 0), &st);
            if(st.st_size > 0)
                return 1; //successful, 1 here means it worked
            else {
                printf("File download failed.\n");
                remove(combine("/tmp/", file, 0));
                printf("Contents of %s database file:\n", file);
                sexecl("/bin/cat", combine("/etc/neptune/data/", file, 0), NULL, NULL);
                return 0;
            }
        }
        else {
            printf("File download failed.\n");
            exit(5); //idk when this is supposed to happen
                     //prolly bad internet?
        }
    }
    else {
        printf("No program (%s) found in database or directory. ", file);
        printf("If you have not updated it in a while or this is your accessing it, run nep update to update your local program database.\n");
        return 0;
    }  
}

int install(char *name) {
    int ret = 2;
    char *file = combine(getenv("OWD"), name, 1);
    char *nepexecutable = combine(getenv("APPDIR"), "/usr/local/bin/neproot", 0);
    sexecl("/bin/cp", nepexecutable, "/tmp/neproot", NULL);
    free(nepexecutable);

    if(!access(file, F_OK ) && !check(file)) 
        ret = sexecl("/usr/bin/pkexec", "/tmp/neproot", "-U", file);
    else if(!access(name, F_OK) && !check(name))
        ret = sexecl("/usr/bin/pkexec", "/tmp/neproot", "-U", name);
    else if (download(name)) {
        char *newfile = combine("/tmp/", name, 0);
        if(!check(newfile))
            ret = sexecl("/usr/bin/pkexec", "/tmp/neproot", "-U", newfile);
        else 
            remove(newfile);
        free(newfile);
    }
    free(file);
    remove("/tmp/neproot");
    return ret; //unexpected error
}