int newapp(GtkContainer *object, char* appname, char* location) {
    char path[256 + 32];
    strcpy(path, location);
    strncat(path, appname, 256);
    char* pic = icon(path);
    printf("pic = %s\n", pic);
    appimage_extract_file_following_symlinks(path, pic, "/tmp/1.png");
    free(pic);
    sexecl("/usr/bin/convert", "/tmp/1.png", "-resize", "64x64", "/tmp/1.png", NULL);

    GtkWidget *label = gtk_label_new(appname);
    GtkWidget *image = gtk_image_new_from_file("/tmp/1.png"); 
    remove("/tmp/1.png");
    GtkContainer *box = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0));
    gtk_container_add(box, label);
    gtk_container_add(box, image);
    gtk_container_add(object, GTK_WIDGET(box));
    return 0;
}

int self()
{
    GtkWidget *window;
    GtkBuilder *builder;

    /* Initialize gtk+*/
    gtk_init (NULL, NULL);

    builder = gtk_builder_new ();
    initialize_store(builder);

    window = GTK_WIDGET (gtk_builder_get_object (builder, "a_window"));
    gtk_builder_connect_signals (builder, NULL);
    g_signal_connect (window, "delete-event", gtk_main_quit, NULL);

    char *arr[2048];
    int last = 0;
    char location[512];
    strcpy(location, getenv("HOME"));
    strcat(location, "/.local/neptune/apps/");

    DIR *d;
    struct dirent *dir;
    d = opendir(location);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
                arr[last] = malloc(256);
                strncpy(arr[last], dir->d_name, 256);
                last++;
            }
        }
        closedir(d);
    }

    GtkContainer *object = GTK_CONTAINER(gtk_builder_get_object(builder, "applist"));

    printf("%d = last\n");
    int i = 0;
    for (int i = 0; i < last; ++i) {
        newapp(object, arr[i], location);
        free(arr[i]);
    }

    gtk_widget_show_all (window);

    gtk_main ();
    return 0;
}