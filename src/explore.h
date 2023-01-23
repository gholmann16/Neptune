void search_changed(GtkSearchEntry* self, GtkWidget * results) {
    GtkWidget * search_result = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * label = gtk_label_new("x application");
    GtkWidget * open = gtk_button_new_with_label("Open");
    gtk_container_add(GTK_CONTAINER(search_result), label);
    gtk_container_add(GTK_CONTAINER(search_result), open);
    gtk_container_add(GTK_CONTAINER(results), search_result);
    gtk_widget_show_all(results);
    puts("hello");
}

void stop_search() {
    puts("stop-search");
}