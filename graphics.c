#include "graphics.h"
#include <gtk/gtk.h>
gchar* list_data_key = "item_key";
static GtkWidget *window;
static GtkWidget *list;
static GtkWidget *listitem;
static void list_item_selected(GtkWidget *gtklist, gpointer data){
  GList *dlist = GTK_LIST(gtklist)->selection;
  if(!dlist){
    return;
  }
  while(dlist){
    GtkObject *item;
    gchar* item_string;
    item = GTK_OBJECT(dlist->data);
    item_string = gtk_object_get_data(item,list_data_key);
    g_print("%s\n", item_string);

    dlist=dlist->next;
  }
}
static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data){
  return FALSE;
}
static void destroy( GtkWidget *widget, gpointer data) {
  gtk_main_quit ();
}

void graphics_init(int* argc, char*** argv){
  
  gtk_init(argc, argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
  g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
}

void graphics_run(struct rawfile* rf){

  list = gtk_list_new();
  gtk_container_add(GTK_CONTAINER(window), list);
  gtk_widget_show(list);
  gtk_signal_connect(GTK_OBJECT(list), "selection_changed",
		     GTK_SIGNAL_FUNC(list_item_selected), NULL);
  for(int i=1; i<rf->nvariables;i++){
    GtkWidget *label;
    char* text = rf->variables[i].name;
    label = gtk_label_new(text);
    listitem=gtk_list_item_new();
    gtk_container_add(GTK_CONTAINER(listitem), label);
    gtk_widget_show(label);
    gtk_container_add(GTK_CONTAINER(list), listitem);
    gtk_object_set_data(GTK_OBJECT(listitem), list_data_key, text);
    gtk_widget_show(listitem);
  }

  
  gtk_widget_show(window);
  gtk_main();
}
