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
    GtkWidget *window;
    GtkBuilder *builder;
    
    gtk_init(NULL, NULL);

    builder = gtk_builder_new ();
    initialize_installer (builder);

    window = GTK_WIDGET (gtk_builder_get_object (builder, "a_window"));
    gtk_builder_connect_signals (builder, NULL);
    g_signal_connect (window, "delete-event", gtk_main_quit, NULL);

    GtkWidget *install;
    install = GTK_WIDGET (gtk_builder_get_object (builder, "install_button"));
    g_signal_connect (install, "clicked", G_CALLBACK(integration), (gpointer) appimage);

    GtkWidget *run;
    run = GTK_WIDGET (gtk_builder_get_object (builder, "run_button"));
    g_signal_connect (run, "clicked", G_CALLBACK(run_once), (gpointer) appimage);

    gtk_widget_show_all (window);

    gtk_main ();
    return 0;

}