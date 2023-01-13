#include <ezxml.h>

GtkWidget * scrolled_window;
GtkWidget * installed_tab;
GtkContainer * interior;

struct App {
    char * name;
    GtkWidget * applications;
};

void settings_print(GtkWidget * button, GtkTextBuffer * buffer) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    puts(gtk_text_buffer_get_text(buffer, &start, &end, 0));

}

void openapp(GtkWidget * button, gpointer data) {
    struct App *application = (struct App*)data;
    puts(application->name);
    //gtk_container_remove(GTK_CONTAINER(scrolled_window), GTK_WIDGET(interior));
    gtk_widget_hide(GTK_WIDGET(interior));

    gtk_widget_show(application->applications);

}

int newapp(GtkContainer *object, char* appname, char* location, GtkWidget **applications) {
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

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    remove("/tmp/1.png");

    *applications = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    struct App *application = g_new0(struct App, 1);
    application->name = malloc(256);
    strcpy(application->name, appname);
    application->applications = *applications;

    g_signal_connect(button, "clicked", G_CALLBACK(openapp), application);

    gtk_container_add(GTK_CONTAINER(box), image);
    gtk_container_add(GTK_CONTAINER(box), label);
    gtk_box_pack_end(GTK_BOX(box), button, 0, 0, 0);
    gtk_container_add(object, box);

    //GtkWidget * image2 = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget * table = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget * title = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget * icon = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget * name = gtk_label_new(appname);
    GtkWidget * description = gtk_label_new("Work in progress");
    GtkWidget * permissions = gtk_label_new("Work in progress");

    gtk_container_add(GTK_CONTAINER(title), icon);
    gtk_container_add(GTK_CONTAINER(title), name);
    gtk_container_add(GTK_CONTAINER(table), title);
    gtk_container_add(GTK_CONTAINER(table), description);
    gtk_container_add(GTK_CONTAINER(table), permissions);

    gtk_container_add(GTK_CONTAINER(*applications), table);
    gtk_container_add(GTK_CONTAINER(installed_tab), *applications);
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


    /* Explore tab */

    GtkContainer *featured = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

    char path[MAX_DIR_LEN];
    strcpy(path, getenv("HOME"));
    strcat(path, "/.config/neptune/mirror");
    if (access(path, F_OK)) { 
        printf("Local mirror not availible. Downloading latest mirror.\n");
        if(fork())
            update(2, NULL);
    }
    else {
        ezxml_t app;
        ezxml_t mirror = ezxml_parse_file(path);
        for (app = ezxml_child(mirror, "app"); app; app = app->next) {
            if (ezxml_attr(app, "featured") != NULL)
                printf("featured app spotted! %s\n", ezxml_attr(app, "name"));
        }
    }


    /* Installed tab */

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
                arr[last] = (char *)malloc(256);
                strncpy(arr[last], dir->d_name, 256);
                last++;
            }
        }
        closedir(d);
    }

    interior = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkAdjustment * adj_v = gtk_adjustment_new (0,0,100,1,10,10);
    scrolled_window = gtk_scrolled_window_new (NULL, adj_v);
    installed_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget * installed_label = gtk_label_new("Installed apps");
    gtk_container_add (GTK_CONTAINER(interior), installed_label);

    GtkWidget * applications[last];
    int i = 0;
    for (i = 0; i < last; ++i) {
        newapp(interior, arr[i], location, &applications[i]);
        free(arr[i]);
    }

    GtkContainer *object = GTK_CONTAINER(gtk_builder_get_object(builder, "applist"));
    /*
    stack = gtk_stack_new();

    gtk_stack_add_named(GTK_STACK(stack), GTK_WIDGET(interior), "test");
    GtkWidget *test = gtk_label_new("test");
    gtk_stack_add_named(GTK_STACK(stack), test, "hello");
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "hello");
    */

    GtkWidget * overbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER(overbox), GTK_WIDGET(interior));
    gtk_container_add (GTK_CONTAINER(overbox), installed_tab);
    gtk_container_add (GTK_CONTAINER(scrolled_window), overbox);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
    gtk_box_pack_start (GTK_BOX (object), scrolled_window, TRUE, TRUE, 0);


    /* Settings tab */

    GtkContainer * settings = GTK_CONTAINER(gtk_builder_get_object(builder, "settings"));
    GtkWidget * folder = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkTextBuffer * buffer = gtk_text_buffer_new(NULL);
    char * data = getdir("userdata");
    gtk_text_buffer_set_text(buffer, data, -1);
    free(data);
    GtkWidget * text = gtk_text_view_new_with_buffer(buffer);
    GtkWidget * save = gtk_button_new_with_label("save");
    
    g_signal_connect(save, "clicked", G_CALLBACK(settings_print), buffer);
    
    gtk_container_add(GTK_CONTAINER(folder), text);
    gtk_container_add(GTK_CONTAINER(folder), save);
    gtk_container_add(settings, folder);

    gtk_widget_show_all (window);
    for (i = 0; i < last; ++i) {
        gtk_widget_hide(applications[i]);
    }

    gtk_main ();
    return 0;
}