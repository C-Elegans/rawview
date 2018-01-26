#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "rawfile.h"
#include "graphics.h"
int main(int argc, char** argv){
  if(argc != 2){
    fprintf(stderr, "Usage: %s [rawfile]\n",argv[0]);
    return 1;
  }
  graphics_init(&argc, &argv);
  struct rawfile* rf = parse_rawfile(argv[1]);
  
  graphics_run(rf);

}
