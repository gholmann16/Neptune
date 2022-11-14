int newapp(GtkContainer *object, char* appname, char* location) {
    char path[256 + 32];
    strcpy(path, location);
    strncat(path, appname, 256);
    char* pic = icon(path);
    appimage_extract_file_following_symlinks(path, pic, "/tmp/1.png");
    free(pic);
    sexecl("/usr/bin/convert", "/tmp/1.png", "-resize", "32x32", "/tmp/1.png", NULL);

    GtkWidget *label = gtk_label_new(appname);
    //gtk_widget_set_hexpand(label, 1);
    GtkWidget *image = gtk_image_new_from_file("/tmp/1.png"); 
    GtkWidget *button = gtk_button_new_with_label("Open");

    remove("/tmp/1.png");
    GtkContainer *box = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0));
    gtk_container_add(box, image);
    gtk_container_add(box, label);
    gtk_box_pack_end(GTK_BOX(box), button, 0, 0, 0);
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

    GtkContainer *interior = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    int i = 0;
    for (int i = 0; i < last; ++i) {
        newapp(interior, arr[i], location);
        free(arr[i]);
    }

    GtkContainer *object = GTK_CONTAINER(gtk_builder_get_object(builder, "applist"));
    GtkAdjustment * adj_v = gtk_adjustment_new (0,0,100,1,10,10);
    GtkWidget * scrolled_window = gtk_scrolled_window_new (NULL, adj_v);
    gtk_container_add (GTK_CONTAINER (scrolled_window), GTK_WIDGET(interior));
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
    gtk_box_pack_start (GTK_BOX (object), scrolled_window, TRUE, TRUE, 0);

    gtk_widget_show_all (window);

    gtk_main ();
    return 0;
}