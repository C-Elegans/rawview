#include "graphics.h"
#include <gtk/gtk.h>
#include <math.h>
gchar* list_data_key = "item_key";
static int xoffset = 25;
static int yoffset = 20;
static GtkWidget *window;
static GtkWidget *list;
static GtkWidget *listitem;
static GtkWidget *drawingarea;
struct variable* currentvar = NULL;
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
  *offset = -(max+min)/2;
  printf("max:%f, min:%f\n",max,min);
  printf("scale: %f, offset: %f\n", *scale, *offset);
}
void draw_y_grid(cairo_t* cr, struct variable* var, size_t points,
		    int width, int height){
  double scale, offset;
  get_scale(var->data, points, &scale, &offset);
  double max = 0.5/scale;
  double min = -0.5/scale;
  cairo_set_source_rgb(cr, 0.5,0.5,0.5);
  cairo_set_line_width(cr,0.5);
  char str[128];
  /* for(double t=t0;t<tmax;t+=tstep){ */
  /*   float x = floor(t/dt * width + xoffset); */
  /*   cairo_move_to(cr,x+0.5,0); */
  /*   cairo_line_to(cr,x+0.5,height); */
  /*   cairo_move_to(cr,x,height+yoffset/2.0); */
  /*   snprintf(str,sizeof(str),"%.1es",t); */
  /*   cairo_show_text(cr,str); */
  /* } */
  /* cairo_stroke(cr); */
  
}
void draw_time_grid(cairo_t* cr, double* time, size_t points,
		    int width, int height){
  double t0 = time[0];
  double tmax = time[points-1];
  double dt = tmax-t0;
  double tstep = pow(10,floor(log10(dt)));
  cairo_set_source_rgb(cr, 0.5,0.5,0.5);
  cairo_set_line_width(cr,0.5);
  char str[128];
  for(double t=t0;t<tmax;t+=tstep){
    float x = floor(t/dt * width + xoffset);
    cairo_move_to(cr,x+0.5,0);
    cairo_line_to(cr,x+0.5,height);
    cairo_move_to(cr,x,height+yoffset/2.0);
    snprintf(str,sizeof(str),"%.1es",t);
    cairo_show_text(cr,str);
  }
  cairo_stroke(cr);
  
}
static gboolean expose_event( GtkWidget *widget, GdkEventExpose *event){
  guint width = widget->allocation.width - xoffset;
  guint height = widget->allocation.height - yoffset;
  cairo_t *cr;
  cr = gdk_cairo_create(gtk_widget_get_window(widget));
  cairo_set_source_rgb(cr,255,255,255);
  cairo_paint(cr);
  if(currentvar){
    size_t points = rf->points;
    double* data = currentvar->data;
    double* time = rf->variables[0].data;
    double t0 = time[0];
    double tmax = time[points-1];
    double dt = tmax-t0;
    double scale, offset;
    get_scale(data, points, &scale, &offset);
    draw_time_grid(cr,time,points,width,height);
    cairo_set_source_rgb(cr,1,0,0);
    cairo_set_line_width(cr, 1.5);
    for(size_t i=0;i<points;i++){
      int x = (time[i]-time[0])/dt * width + xoffset;
      int y = (-scale*(data[i] +offset) + 0.5) * height;
      if(i==0){
	cairo_move_to(cr,x,y);
      }
      else{
	cairo_line_to(cr,x,y);
      }
    }
    cairo_stroke(cr);

  }
  cairo_destroy(cr);
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
    currentvar = &rf->variables[item_index];
    dlist=dlist->next;
  }
  gtk_widget_queue_draw(drawingarea);
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
  gtk_box_pack_start(GTK_BOX(box), drawingarea, TRUE, TRUE, 10);
  gtk_widget_show(drawingarea);

  gtk_widget_show(box);
  gtk_widget_show(window);
  gtk_main();
}
