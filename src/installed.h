struct App {
    char * name;
    GtkWidget * applications;
    GtkContainer * interior;
};


void button_switch(GtkWidget * togglebutton, char * data) {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton))) {
        const char * label = gtk_button_get_label(GTK_BUTTON(togglebutton));
        FILE * f = fopen(data, "r+");
        int r;

        for(r = 0; r < 26; r++) // r < x where x is characters until the level variable
            fgetc(f);
        fputc(label[0], f);
        fclose(f);
    }
}

void openapp(GtkWidget * button, gpointer data) {
    struct App *application = (struct App*)data;
    gtk_widget_hide(GTK_WIDGET(application->interior));
    gtk_widget_show(application->applications);
}

void closeapp(GtkWidget * button, gpointer data) {
    struct App *application = (struct App*)data;
    gtk_widget_hide(application->applications);
    gtk_widget_show(GTK_WIDGET(application->interior));
}

int newapp(GtkContainer *object, char* appname, char* location, GtkWidget **applications, GtkWidget* installed_tab) {
    char path[256 + 32];
    strcpy(path, location);
    strncat(path, appname, 256);
    char* pic = icon(path);
    appimage_extract_file_following_symlinks(path, pic, "/tmp/1.png");
    free(pic);
    sexecl("/usr/bin/convert", "/tmp/1.png", "-resize", "32x32", "/tmp/1.png", NULL);

    GtkWidget *label = gtk_label_new(appname);
    //gtk_widget_set_hexpand(label, 1);
    GError* error = NULL;
    GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file("/tmp/1.png", &error);
    GtkWidget * image = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget * button = gtk_button_new_with_label("Open");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    remove("/tmp/1.png");

    *applications = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    struct App *application = g_new0(struct App, 1);
    application->name = malloc(256);
    strcpy(application->name, appname);
    application->applications = *applications;
    application->interior = object;

    g_signal_connect(button, "clicked", G_CALLBACK(openapp), application);

    gtk_container_add(GTK_CONTAINER(box), image);
    gtk_container_add(GTK_CONTAINER(box), label);
    gtk_box_pack_end(GTK_BOX(box), button, 0, 0, 0);
    gtk_container_add(object, box);

    GtkContainer * table = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget * back = gtk_button_new_with_label("back");
    GtkWidget * title = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * icon = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget * name = gtk_label_new(appname);
    GtkWidget * description = gtk_label_new("Work in progress");

    g_signal_connect(back, "clicked", G_CALLBACK(closeapp), application);

    gtk_container_add(GTK_CONTAINER(title), back);
    gtk_container_add(GTK_CONTAINER(title), icon);
    gtk_container_add(GTK_CONTAINER(title), name);
    gtk_container_add(table, title);
    gtk_container_add(table, description);

    //permissions area
    GtkWidget * stack = gtk_stack_new();
    GtkWidget * switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));

    //simple permissions    
    GtkWidget * simple = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * simple_label = gtk_label_new("Level: ");
    gtk_container_add(GTK_CONTAINER(simple), simple_label);

    char * permloc = malloc(MAX_DIR_LEN);
    strcpy(permloc, getenv("HOME"));
    strcat(permloc, "/.local/share/aisap/profiles/");
    strcat(permloc, appname);

    FILE * permsfile = fopen(permloc, "r");

    char str[32];
    fgets(str, 32, permsfile);
    fgets(str, 32, permsfile);
    fclose(permsfile);
    int current = str[strlen(str) - 2] - 48;

    GtkWidget * levels[4];
    char num[4][2] = {"0\0", "1\0", "2\0", "3\0"};
    levels[0] = gtk_radio_button_new_with_label(NULL, num[0]);
    long i;
    for (i = 0; i < 4; i++) {
        levels[i] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(levels[0]), num[i]);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(levels[0]), GTK_RADIO_BUTTON(levels[i]));
        gtk_container_add(GTK_CONTAINER(simple), levels[i]);
        g_signal_connect(levels[i], "released", G_CALLBACK(button_switch), permloc);
    }

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(levels[current]), TRUE);

    //advanced permissions
    GtkWidget * advanced = gtk_label_new("test 2");
    
    gtk_stack_add_titled(GTK_STACK(stack), simple, "Simple", "Simple");
    gtk_stack_add_titled(GTK_STACK(stack), advanced, "Advanced", "Advanced");

    gtk_container_add(table, switcher);
    gtk_container_add(table, stack);

    gtk_container_add(GTK_CONTAINER(*applications), GTK_WIDGET(table));
    gtk_container_add(GTK_CONTAINER(installed_tab), *applications);
    return 0;
}