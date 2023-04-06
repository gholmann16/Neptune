void search_changed(GtkSearchEntry* self, ezxml_t mirror) {

    GtkWidget * explore_box = gtk_widget_get_parent(GTK_WIDGET(self));
    GList *children = gtk_container_get_children(GTK_CONTAINER(explore_box));
    GtkWidget * results = children->next->data;

    GList * iter, *grandchildren;
    grandchildren = gtk_container_get_children(GTK_CONTAINER(results));
    for(iter = grandchildren; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(grandchildren);


    ezxml_t app;
    const char * file = gtk_entry_get_text(GTK_ENTRY(self));
    if (file[0] != '\0') 
        for (app = ezxml_child(mirror, "app"); app; app = app->next) {
            if (strstr(ezxml_attr(app, "name"), file)) {
                GtkWidget * search_result = gtk_button_new_with_label(ezxml_attr(app, "name"));
                gtk_container_add(GTK_CONTAINER(results), search_result);
            }
        }
    
    gtk_widget_show_all(results);
    
}

void stop_search() {
    puts("stop-search");
}

void view_app() {
    puts("Viewing app");
}