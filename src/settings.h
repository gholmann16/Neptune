void app_directory_modify(GtkWidget * button, GtkTextBuffer * buffer) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char * pointer = gtk_text_buffer_get_text(buffer, &start, &end, 0);
    char path[MAX_DIR_LEN];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.config/neptune/dir");
    FILE * initial = fopen(path, "r");
    char current[MAX_DIR_LEN];
    char ch;
    int i = 0;
    while ((ch = fgetc(initial)) != EOF) {
        current[i] = ch;
        ++i;
    }
    current[i] = '\0';
    fclose(initial);
    FILE * f = fopen(path, "w");
    fprintf(f, pointer);
    fclose(f);


    char location[MAX_DIR_LEN];
    strcpy(location, getenv("HOME"));
    strcat(location, "/.local/neptune/apps/");

    char old_directory[MAX_DIR_LEN];
    strcpy(old_directory, getenv("HOME"));
    strcat(old_directory, "/");
    strcat(old_directory, current);
    strcat(old_directory, "/");

    char new_directory[MAX_DIR_LEN];
    strcpy(new_directory, getenv("HOME"));
    strcat(new_directory, "/");
    strcat(new_directory, pointer);
    strcat(new_directory, "/");

    mkdir(new_directory, 0700);

    DIR *d;
    struct dirent *dir;
    d = opendir(location);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
                char old_desktop[MAX_DIR_LEN];
                strcpy(old_desktop, old_directory);
                strcat(old_desktop, dir->d_name);
                strcat(old_desktop, ".desktop");

                char new_desktop[MAX_DIR_LEN];
                strcpy(new_desktop, new_directory);
                strcat(new_desktop, dir->d_name);
                strcat(new_desktop, ".desktop");

                rename(old_desktop, new_desktop);

            }
        }
        closedir(d);
    }

    rmdir(old_directory);
}

void data_directory_modify(GtkWidget * button, GtkTextBuffer * buffer) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char * pointer = gtk_text_buffer_get_text(buffer, &start, &end, 0);
    char path[MAX_DIR_LEN];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.config/neptune/userdata");

    FILE * initial = fopen(path, "r");
    char current[MAX_DIR_LEN];
    char ch;
    int i = 0;
    while ((ch = fgetc(initial)) != EOF) {
        current[i] = ch;
        ++i;
    }
    current[i] = '\0';
    fclose(initial);

    FILE * f = fopen(path, "w");
    fprintf(f, pointer);

    int fd = fileno(f);
    ftruncate(fd, strlen(pointer));
    
    fclose(f);

    char old_directory[MAX_DIR_LEN];
    strcpy(old_directory, getenv("HOME"));
    strcat(old_directory, "/");
    strcat(old_directory, current);
    strcat(old_directory, "/");

    char new_directory[MAX_DIR_LEN];
    strcpy(new_directory, getenv("HOME"));
    strcat(new_directory, "/");
    strcat(new_directory, pointer);
    strcat(new_directory, "/");

    rename(old_directory, new_directory);
}