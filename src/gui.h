int integration(GtkWidget *widget, gpointer *data) {
    printf("Integrating AppImage: %s\n", (char *) data);
    install((char *) data);
    gtk_main_quit();
    return 0;
}

int run_once(GtkWidget *widget, gpointer *data) {
    if (access((char *) data, F_OK)) {
        printf("Cannot find %s\n", (char*) data);
        exit(1);
    }
    printf("Running executable: %s\n", (char *) data);
    if (access((char *) data, X_OK)) {
        struct stat fs;
        stat((char *) data,&fs);
        chmod((char *) data, fs.st_mode | S_IXUSR);
    }
    sexecl((char *) data, NULL);
    gtk_main_quit();
    return 0;
}

int gui(char* appimage) {
    
    gtk_init(NULL, NULL);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "delete-event", gtk_main_quit, NULL);

    GtkWidget * box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget * label = gtk_label_new("Would you like to integrate this AppImage or Run it once?\n\nIf you integrate the AppImage, it will be installed to your computer like a windows exe. The AppImage will also get a simple sandboxing profile to protect your computer's data.\n\nIf you click run once, the AppImage will run unprotected like a standard linux executable.");
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_width_chars(GTK_LABEL(label), 63);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 72);

    gtk_box_pack_start(GTK_BOX(box), label, 0, 0, 0);
    GtkWidget * box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget * install = gtk_button_new_with_label("Integrate");
    g_signal_connect (install, "clicked", G_CALLBACK(integration), (gpointer) appimage);
    GtkWidget * run = gtk_button_new_with_label("Run Once");
    g_signal_connect (run, "clicked", G_CALLBACK(run_once), (gpointer) appimage);

    gtk_box_pack_start(GTK_BOX(box2), install, 0, 0, 0);
    gtk_box_pack_end(GTK_BOX(box2), run, 0, 0, 0);
    gtk_box_pack_end(GTK_BOX(box), box2, 0, 0, 0);

    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_widget_show_all (window);

    gtk_main ();
    return 0;

}