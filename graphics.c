#include "graphics.h"
#include <gtk/gtk.h>
gchar* list_data_key = "item_key";
static GtkWidget *window;
static GtkWidget *list;
static GtkWidget *listitem;
static GtkWidget *drawingarea;
static GdkPixmap *pixmap = NULL;
static struct rawfile* rf = NULL;
static void get_scale(double* data, size_t items, double* scale, double* offset){
  double min = data[0];
  double max = data[0];
  for(size_t i=1;i<items;i++){
    if(data[i] > max)
      max = data[i];
    if(data[i] < min)
      min = data[i];
  }
  *scale = 0.9/(max-min);
  *offset = (max+min)+ 0.5;
}
static void graph_data(GdkPixmap *pixmap, GtkWidget *widget,
		       int vindex, GdkColor color){
  gdk_draw_rectangle (pixmap,
		      widget->style->white_gc,
		      TRUE,
		      0, 0,
		      widget->allocation.width,
		      widget->allocation.height);
  gdk_color_alloc(gtk_widget_get_colormap(widget),&color);
  GdkGC* gc = gdk_gc_new(pixmap);
  gdk_gc_set_foreground(gc,&color);
  double* data = rf->variables[vindex].data;
  int x =0,y=0;
  int x_prev, y_prev;
  double scale, offset;
  get_scale(data, rf->points, &scale, &offset);
  for(size_t i=0;i<rf->points;i++){
    x = (int) (((double)i/rf->points) * widget->allocation.width);
    double val = -scale * data[i] + offset;
    y = val *widget->allocation.height;
    if(i!=0){
      gdk_draw_line(pixmap, gc, x_prev, y_prev, x,y);
    }
    y_prev = y;
    x_prev = x;
  }
  gtk_widget_queue_draw_area(widget,0,0,widget->allocation.width, widget->allocation.height);

}
static gboolean configure_event( GtkWidget *widget, GdkEventConfigure *event){
  if (pixmap)
    g_object_unref(pixmap);

  pixmap = gdk_pixmap_new(widget->window,
			  widget->allocation.width,
			  widget->allocation.height,
			  -1);
  gdk_draw_rectangle (pixmap,
		      widget->style->white_gc,
		      TRUE,
		      0, 0,
		      widget->allocation.width,
		      widget->allocation.height);

  return TRUE;
}
static gboolean expose_event( GtkWidget *widget, GdkEventExpose *event){
  gdk_draw_drawable(widget->window,
		    widget->style->fg_gc[gtk_widget_get_state (widget)],
		    pixmap,
		    event->area.x, event->area.y,
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);

  return FALSE;
}
static void list_item_selected(GtkWidget *gtklist, gpointer data){
  GList *dlist = GTK_LIST(gtklist)->selection;
  if(!dlist){
    return;
  }
  while(dlist){
    GtkObject *item;
    gint item_index;
    item = GTK_OBJECT(dlist->data);
    item_index = GPOINTER_TO_INT(gtk_object_get_data(item,list_data_key));
    printf("%d\n", item_index);
    GdkColor color;
    color.red = 65535;
    color.green =0;
    color.blue = 0;
    graph_data(pixmap, drawingarea, item_index, color);
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

void graphics_run(struct rawfile* _rf){
  rf = _rf;
  GtkWidget* box;
  box = gtk_hbox_new(FALSE, 0);
  list = gtk_list_new();
  gtk_container_add(GTK_CONTAINER(window), box);
  gtk_box_pack_start(GTK_BOX(box), list, FALSE, FALSE, 10);
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
    gtk_object_set_data(GTK_OBJECT(listitem), list_data_key, GINT_TO_POINTER(i));
    gtk_widget_show(listitem);
  }

  drawingarea = gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA(drawingarea), 500,500);
  gtk_signal_connect(GTK_OBJECT(drawingarea), "expose_event",
		     (GtkSignalFunc) expose_event, NULL);
  gtk_signal_connect(GTK_OBJECT(drawingarea), "configure_event",
		     (GtkSignalFunc) configure_event, NULL);
  gtk_box_pack_start(GTK_BOX(box), drawingarea, FALSE, FALSE, 10);
  gtk_widget_show(drawingarea);

  gtk_widget_show(box);
  gtk_widget_show(window);
  gtk_main();
}
