#include <ezxml.h>
#include "explore.h"
#include "installed.h"
#include "settings.h"

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

    GtkBox *featured = GTK_BOX(gtk_builder_get_object(builder, "featured"));;

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
            if (ezxml_attr(app, "featured") != NULL) {
                printf("featured app spotted! %s\n", ezxml_attr(app, "name"));
                char thumbnail[MAX_DIR_LEN];
                strcpy(thumbnail, getenv("HOME"));
                strcat(thumbnail, "/.cache/neptune/");
                strcat(thumbnail, ezxml_attr(app, "name"));
                if(access(thumbnail, F_OK))
                    sexecl("/usr/bin/wget", ezxml_child(app, "image")->txt, "-q", "-O", thumbnail, NULL);
                GtkWidget * pic = gtk_image_new_from_file(thumbnail);

                GtkWidget * featured_box = gtk_frame_new(ezxml_attr(app, "name"));
                gtk_frame_set_label_align(GTK_FRAME(featured_box), 0.5, 0);

                gtk_container_add(GTK_CONTAINER(featured_box), pic);
                gtk_box_pack_start(featured, featured_box, 1, 1, 0);
            }
        }
    }

    GtkWidget * search = GTK_WIDGET(gtk_builder_get_object(builder, "search"));
    GtkWidget * results = GTK_WIDGET(gtk_builder_get_object(builder, "results"));
    g_signal_connect(search, "search-changed", G_CALLBACK(search_changed), results);

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

    GtkContainer * interior = GTK_CONTAINER(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkAdjustment * adj_v = gtk_adjustment_new (0,0,100,1,10,10);
    GtkWidget * scrolled_window = gtk_scrolled_window_new (NULL, adj_v);
    GtkWidget * installed_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget * installed_label = gtk_label_new("Installed apps");
    gtk_container_add (GTK_CONTAINER(interior), installed_label);

    GtkWidget * applications[last];
    int i = 0;
    for (i = 0; i < last; ++i) {
        newapp(interior, arr[i], location, &applications[i], installed_tab);
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
    char * data = getdir("dir");
    gtk_text_buffer_set_text(buffer, data, -1);
    free(data);

    GtkWidget * explanation = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(explanation), "<b>Location to store your AppImages</b>");
    gtk_widget_set_halign(explanation, GTK_ALIGN_START);

    GtkWidget * text = gtk_text_view_new_with_buffer(buffer);
    GtkWidget * save = gtk_button_new_with_label("save");

    g_signal_connect(save, "clicked", G_CALLBACK(app_directory_modify), buffer);
    
    gtk_container_add(GTK_CONTAINER(folder), text);
    gtk_container_add(GTK_CONTAINER(folder), save);
    gtk_container_add(settings, explanation);
    gtk_container_add(settings, folder);

    GtkWidget * data_folder = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkTextBuffer * data_buffer = gtk_text_buffer_new(NULL);
    char * data_data = getdir("userdata");
    gtk_text_buffer_set_text(data_buffer, data_data, -1);
    free(data_data);

    GtkWidget * data_explanation = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(data_explanation), "<b>Location to store your AppImages' data</b>");
    gtk_widget_set_halign(data_explanation, GTK_ALIGN_START);

    GtkWidget * data_text = gtk_text_view_new_with_buffer(data_buffer);
    GtkWidget * data_save = gtk_button_new_with_label("save");
    //gtk_button_new_from_icon_name("system-file-manager", 4);

    g_signal_connect(data_save, "clicked", G_CALLBACK(data_directory_modify), data_buffer);
    
    gtk_container_add(GTK_CONTAINER(data_folder), data_text);
    gtk_container_add(GTK_CONTAINER(data_folder), data_save);
    gtk_container_add(settings, data_explanation);
    gtk_container_add(settings, data_folder);

    GtkIconTheme * icon_theme = gtk_icon_theme_get_default();

    gtk_window_set_icon_name(GTK_WINDOW(window), "test");

    gtk_widget_show_all (window);
    for (i = 0; i < last; ++i) {
        gtk_widget_hide(applications[i]);
    }
    gtk_widget_hide(results);

    gtk_main ();
    return 0;
}