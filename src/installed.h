struct App {
    char * name;
    GtkWidget * applications;
    GtkContainer * interior;
};

void remove_app(GtkWidget * self, GString * name) {
    destroy(name->str);
}

void uninstall_app(GtkWidget * self, GString * name) {
    uninstall(name->str);
}

int real(const char * io) {
    char * copy = strdup(io);
    if(strchr(copy, ':') != NULL)
        copy[strchr(copy, ':') - copy] = '\0';
    DIR* dir = opendir(copy);
    if (dir) {
        closedir(dir);
        free(copy);
        return 1;
    }
    else {
        char * xdgs[8] = {"xdg-desktop", "xdg-download", "xdg-documents", "xdg-music", "xdg-pictures", "xdg-publicshare", "xdg-templates", "xdg-videos"};
        int i;
        for(i = 0; i < 8; i++) {
            if(strcmp(copy, xdgs[i]) == 0) {
                free(copy);
                return 1;
            }
        }
    }
    free(copy);
    return 0;
}

void remove_file(GtkInfoBar* self, gint response_id, GString * string) {
    GtkWidget * content_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(self));
    GList * list = gtk_container_get_children (GTK_CONTAINER(content_area));
    GtkWidget * inbox = list->data;
    char * perm = strdup(gtk_entry_get_text(GTK_ENTRY(inbox)));
    if(real(perm)) {
        strcat(perm, ";");
        permissions(-1, string->str, perm);
    }
    free(perm);
    gtk_widget_hide(GTK_WIDGET(self));
}

void text_completed(GtkEntry * self, GString * string) {
    char * permission = strdup(gtk_entry_get_text(self));
    if(real(permission)) {
        char * name = strdup(gtk_widget_get_name(GTK_WIDGET(self)));
        if(strcmp(name, "GtkEntry") != 0) {
            strcat(name, ";");
            permissions(-1, string->str, name);
        }
        free(name);

        gtk_widget_set_name(GTK_WIDGET(self), permission);
        strcat(permission, ";");
        permissions(1, string->str, permission);

    }
    else {
        printf("%s is not a valid permission.\n", permission);
    }
    free(permission);
}

void text_changed(GtkEntry* self) {
    gtk_entry_set_icon_from_icon_name(self, 0, "dialog-question");
    if(real(gtk_entry_get_text(self)))
        gtk_entry_set_icon_from_icon_name(self, 0, "dialog-ok");
}

void add_folder(GtkWidget * file_button, GString * string) {
    GtkWidget * filetitle = gtk_widget_get_parent(file_button);
    GtkWidget * advanced_widget = gtk_widget_get_parent(filetitle);
    GList * boxes = gtk_container_get_children(GTK_CONTAINER(advanced_widget));
    GtkWidget * find;
    while(1) {
        find = boxes->data;
        if (strcmp(gtk_widget_get_name(find), "infobars") == 0)
            break;
        boxes = boxes->next;
    }
    GtkWidget * infobar = gtk_info_bar_new();
    gtk_info_bar_set_show_close_button(GTK_INFO_BAR(infobar), TRUE);
    gtk_box_pack_start(GTK_BOX(find), infobar, 0, 0, 0);

    GtkWidget * content_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(infobar));
    GtkWidget * view = gtk_entry_new();
    GtkEntryBuffer * buffer = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_set_buffer(GTK_ENTRY(view), buffer);
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(view), 0, "dialog-question");
    gtk_box_pack_start(GTK_BOX(content_area), view, 0, 0, 0);
    //gtk_box_pack_end(GTK_BOX(content_area), write_access, 0, 0, 0);

    g_signal_connect(view, "changed", G_CALLBACK(text_changed), NULL);
    g_signal_connect(view, "activate", G_CALLBACK(text_completed), string);
    g_signal_connect(infobar, "response", G_CALLBACK(remove_file), string);

    gtk_widget_show_all(infobar);
}

void socket_changed(GtkWidget * switch_button, gboolean state, GString * string) {
    char perm[256];
    strcpy(perm, gtk_widget_get_name(switch_button));
    if(state) {
        gtk_switch_set_active(GTK_SWITCH(switch_button), TRUE);
        permissions(2, string->str, perm);
    }
    else {
        gtk_switch_set_active(GTK_SWITCH(switch_button), FALSE);
        permissions(-2, string->str, perm);
    }
}

void dri_changed(GtkWidget * switch_button, gboolean state, GString * string) {
    if(state) {
        gtk_switch_set_active(GTK_SWITCH(switch_button), TRUE);
        permissions(3, string->str, "dri");
    }
    else {
        gtk_switch_set_active(GTK_SWITCH(switch_button), FALSE);
        permissions(-3, string->str, "dri");
    }
}

void launch(GtkWidget * button) {
    char path[MAX_DIR_LEN];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.local/neptune/bin/");
    strcat(path, gtk_widget_get_name(button));
    puts(path);
    if(fork()) {
        sexecl(path, NULL);
        exit(0);
    }
}

void button_switch(GtkWidget * togglebutton, char * data) {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton))) {
        const char * label = gtk_button_get_label(GTK_BUTTON(togglebutton));
        FILE * f = fopen(data, "r+");
        int r;

        for(r = 0; r < 26; r++) // r < x where x is characters until the level variable
            fgetc(f);
        fputc(label[0], f);
        fclose(f);
        printf("Permissions level changed to %c\n", label[0]);
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
    
    //Find you what to do
    GString * gpath = g_string_new(appname);

    char path[MAX_DIR_LEN];
    strcpy(path, location);
    strncat(path, appname, 256);

    char cachepath[MAX_DIR_LEN];
    strcpy(cachepath, getenv("HOME"));
    strcat(cachepath, "/.cache/neptune/apps/");
    strcat(cachepath, appname);
    if(access(cachepath, F_OK)) {
        appimage_extract_file_following_symlinks(path, ".DirIcon", cachepath);
        char pic[1] = {0};
        int fd = open(cachepath, O_RDONLY);
        read(fd, pic, 1);
        if(pic[0] == '<') {
            rename(cachepath, "/tmp/temp.svg");
            system("/usr/bin/rsvg-convert /tmp/temp.svg > /tmp/temp.png");
            rename("/tmp/temp.png", cachepath);
        }
        sexecl("/usr/bin/convert", cachepath, "-resize", "32x32", cachepath, NULL);
    }

    GtkWidget *label = gtk_label_new(appname);
    //gtk_widget_set_hexpand(label, 1);
    GError* error = NULL;
    GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(cachepath, &error);
    if( error != NULL ) {
        printf(error->message);
        g_clear_error (&error);
    }
    GtkWidget * image = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget * settings = gtk_button_new_with_label("Settings");
    GtkWidget * button = gtk_button_new_with_label("Open");
    gtk_widget_set_name(button, appname);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,3);

    *applications = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    struct App *application = g_new0(struct App, 1);
    application->name = malloc(256);
    strcpy(application->name, appname);
    application->applications = *applications;
    application->interior = object;

    g_signal_connect(settings, "clicked", G_CALLBACK(openapp), application);
    g_signal_connect(button, "clicked", G_CALLBACK(launch), NULL);

    gtk_container_add(GTK_CONTAINER(box), image);
    gtk_container_add(GTK_CONTAINER(box), label);
    gtk_box_pack_end(GTK_BOX(box), button, 0, 0, 0);
    gtk_box_pack_end(GTK_BOX(box), settings, 0, 0, 0);
    gtk_container_add(object, box);

    GtkContainer * table = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    GtkWidget * back = gtk_button_new_with_label("back");
    GtkWidget * title = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * icon = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget * name = gtk_label_new(appname);
    GtkWidget * remove = gtk_button_new_with_label("remove");
    GtkWidget * uninstall = gtk_button_new_with_label("uninstall");
    GtkWidget * header = gtk_label_new("Description:");
    GtkWidget * description = gtk_label_new("No metdata found in appimage.");

    char * metapath = metadata(path);
    char * buffer = malloc(65536);
    unsigned long size = 0L;
    if (metapath != NULL) {
        appimage_read_file_into_buffer_following_symlinks(path, metapath, &buffer, &size);
        char * pointer = strstr(buffer, "<p>") + 3;
        buffer[strstr(buffer, "</p>") - buffer] = '\0';
        gtk_label_set_text(GTK_LABEL(description), pointer);
        gtk_label_set_line_wrap(GTK_LABEL(description), TRUE);
    }
    free(buffer);

    g_signal_connect(back, "clicked", G_CALLBACK(closeapp), application);
    g_signal_connect(remove, "clicked", G_CALLBACK(remove_app), gpath);
    g_signal_connect(uninstall, "clicked", G_CALLBACK(uninstall_app), gpath);

    gtk_container_add(GTK_CONTAINER(title), back);
    gtk_container_add(GTK_CONTAINER(title), icon);
    gtk_container_add(GTK_CONTAINER(title), name);
    gtk_box_pack_end(GTK_BOX(title), uninstall, 0, 0, 0);
    gtk_box_pack_end(GTK_BOX(title), remove, 0, 0, 0);

    gtk_container_add(table, title);
    gtk_container_add(table, header);
    gtk_container_add(table, description);

    //permissions area
    GtkWidget * stack = gtk_stack_new();
    GtkWidget * switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));

    //simple permissions    
    GtkWidget * simple = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget * simple_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * simple_label = gtk_label_new("Level: ");
    gtk_container_add(GTK_CONTAINER(simple_box), simple_label);

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
        gtk_container_add(GTK_CONTAINER(simple_box), levels[i]);
        g_signal_connect(levels[i], "released", G_CALLBACK(button_switch), permloc);
    }

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(levels[current]), TRUE);
    gtk_box_pack_start(GTK_BOX(simple), simple_box, 0, 0, 0);

    //advanced permissions
    GtkWidget * advanced = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);

    FILE * depth = fopen(permloc, "r");
    fgets(str, 32, depth);
    fgets(str, 32, depth);

    GtkWidget * file_access = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * file_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(file_title), "<big><b>File Access</b></big>");
    
    GtkWidget * folder = gtk_image_new_from_icon_name("folder-new", 3);
    GtkWidget * grant_acess = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON (grant_acess), folder);
    g_signal_connect(grant_acess, "clicked", G_CALLBACK(add_folder), gpath);

    gtk_box_pack_start(GTK_BOX(file_access), file_title, 0, 0, 0);
    gtk_box_pack_end(GTK_BOX(file_access), grant_acess, 0, 0, 0);
    gtk_widget_set_halign(file_title, 0);
    gtk_widget_set_halign(folder, 1);
    gtk_container_add(GTK_CONTAINER(advanced), file_access);

    GtkWidget * files = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(files, "infobars");
    gtk_container_add(GTK_CONTAINER(advanced), files);

    char scan[512];
    fgets(scan, 512, depth);
    char * pointer = strchr(scan, '=') + 1;
    char current_permission[256];
    char needle;
    int c = 0, v = 0;
    while (1) {
        needle = pointer[c];
        if(needle != ';') {
            if(needle == '\n')
                break;
            else
                current_permission[v] = needle;
        }
        else {
            current_permission[v] = '\0';
            char * last = current_permission + v - 3;

            GtkWidget * info = gtk_info_bar_new();
            GtkWidget * file_permission = gtk_entry_new();
            GtkEntryBuffer * file_buffer = gtk_entry_buffer_new(NULL, -1);

            gtk_entry_set_buffer(GTK_ENTRY(file_permission), file_buffer);
            gtk_entry_set_icon_from_icon_name(GTK_ENTRY(file_permission), 0, "dialog-ok");
            gtk_entry_set_text(GTK_ENTRY(file_permission), current_permission);

            gtk_info_bar_set_show_close_button(GTK_INFO_BAR(info), TRUE);
            gtk_box_pack_start(GTK_BOX(files), info, 0, 0, 0);

            GtkWidget * content_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(info));
            gtk_box_pack_start(GTK_BOX(content_area), file_permission, 0, 0, 0);

            g_signal_connect(info, "response", G_CALLBACK(remove_file), gpath);

            v = -1;
        }
        c++;
        v++;
    }


    GtkWidget * device_access = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(device_access), "<big><b>Device Access</b></big>");
    gtk_container_add(GTK_CONTAINER(advanced), device_access);

    GtkWidget * device_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * dri_switch = gtk_switch_new();
    GtkWidget * dri_permission = gtk_label_new("dri");
    gtk_container_add(GTK_CONTAINER(device_box), dri_switch);
    gtk_container_add(GTK_CONTAINER(device_box), dri_permission);
    gtk_container_add(GTK_CONTAINER(advanced), device_box);
    fgets(scan, 512, depth);
    char * newpointer = strchr(scan, '=') + 1;
    if (strcmp(newpointer, "dri;") == 0);
        gtk_switch_set_active(GTK_SWITCH(dri_switch), TRUE);

    GtkWidget * socket_access = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(socket_access), "<big><b>Socket Access</b></big>");
    gtk_container_add(GTK_CONTAINER(advanced), socket_access);
    
    g_signal_connect(dri_switch, "state_set", G_CALLBACK(dri_changed), gpath);

    fgets(scan, 512, depth);
    char * pointer3 = strchr(scan, '=') + 1;
    char * sockets[13] = {"alsa", "cgroup", "dbus", "ipc", "network", "pid", "pipewire", "pulseaudio", "session", "user", "uts", "wayland", "x11"};
    int rep;
    GtkWidget * socket_switch[13];
    for (rep = 0; rep < 13; rep++) {
        GtkWidget * socket_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        GtkWidget * dri_permission = gtk_label_new(sockets[rep]);
        socket_switch[rep] = gtk_switch_new();
        gtk_container_add(GTK_CONTAINER(socket_box), socket_switch[rep]);
        gtk_container_add(GTK_CONTAINER(socket_box), dri_permission);
        gtk_container_add(GTK_CONTAINER(advanced), socket_box);

        c = 0;
        v = 0;
        while (1) {
            needle = pointer3[c];
            if(needle != ';') {
                if(needle == '\n')
                    break;
                else
                    current_permission[v] = needle;
            }
            else {
                current_permission[v] = '\0';
                if (strcmp(current_permission, sockets[rep]) == 0)
                    gtk_switch_set_active(GTK_SWITCH(socket_switch[rep]), TRUE);
                v = -1;
            }
            c++;
            v++;
        }

        g_signal_connect(socket_switch[rep], "state_set", G_CALLBACK(socket_changed), gpath);
        gtk_widget_set_name(socket_switch[rep], sockets[rep]);
    }

    fclose(depth);
    
    gtk_stack_add_titled(GTK_STACK(stack), simple, "Simple", "Simple");
    gtk_stack_add_titled(GTK_STACK(stack), advanced, "Advanced", "Advanced");

    gtk_container_add(table, switcher);
    gtk_container_add(table, stack);

    gtk_container_add(GTK_CONTAINER(*applications), GTK_WIDGET(table));
    gtk_container_add(GTK_CONTAINER(installed_tab), *applications);
    return 0;
}