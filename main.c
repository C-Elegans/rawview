#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rawfile.h"


int main(int argc, char** argv){
  if(argc != 2){
    fprintf(stderr, "Usage: %s [rawfile]\n",argv[0]);
    return 1;
  }
  struct rawfile* rf = parse_rawfile(argv[1]);

  printf("title: %s, variables: %d\n", rf->title, rf->nvariables);
  for(int i=0;i<rf->nvariables;i++){
    printf("var %d: %s %d\n", i, rf->variables[i].name, rf->variables[i].type);
  }
  for(size_t i=0;i<rf->points;i++){
    printf("%f\n", rf->variables[1].data[i]);
  }


}
