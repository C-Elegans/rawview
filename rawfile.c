#include "rawfile.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

char prefix(char* pre, char* str){
  return strncmp(pre, str, strlen(pre)) == 0;
}




char* read_file(char* filename){
  FILE* f = fopen(filename, "r");
  fseek(f, 0, SEEK_END);
  ssize_t size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* buf = malloc(size+1);
  fread(buf,1,size,f);
  fclose(f);
  return buf;
}
int field_key(char** buf, char** key){
  char c;
  char* str = *buf;
  *key = str;
  while(((c = *str)) && (c != ':')){
    str++;
  }
  if(!c) goto fail;
  *str = 0;
  *buf = ++str;
  return 1;
 fail:
  return 0;
}
int field_val(char** buf, char** val){
  char c;
  char* str = *buf;
  *val = str;
  while(((c = *str)) && (c != '\n') && (c == ' ')){
      (*val)++;
      str++;
  }
  if(!c) goto fail;
  while(((c = *str)) && (c != '\n')){
    str++;
  }
  if(!c) goto fail;
  *str = 0;
  *buf = ++str;
  return 1;
 fail:
  return 0;
}
void skip_space(char** str){
  char c;
  while((c=**str) && isspace(c)){
    (*str)++;
  }
}

void read_variable_data(struct rawfile* rf){
  printf("%x\n", *((char*)rf->buf));
  double* buf = (double*) rf->buf;
  for(size_t p=0;p<rf->points;p++){
    for(ssize_t v=0;v<rf->nvariables;v++){
      assert(!(rf->flags &FLAG_COMPLEX));
      rf->variables[v].data[p]=*buf++;
    }
  }
}


struct rawfile* parse_rawfile(char* filename){
  struct rawfile* rf = malloc(sizeof(*rf));
  memset(rf,0,sizeof(*rf));
  char* sav = read_file(filename);
  char* buf = sav;
  char *key, *val;
  while(1){
    char cont = field_key(&buf, &key);
    if(strcasecmp(key, "Title") ==0){
      field_val(&buf,&val);
      rf->title = val;
    }
    else if(strcasecmp(key, "date")==0){
      field_val(&buf,&val);
      rf->date = val;
    }
    else if(strcasecmp(key, "plotname")==0){
      field_val(&buf,&val);
      rf->plotname = val;
    }
    else if(strcasecmp(key, "flags")==0){
      field_val(&buf,&val);
      rf->flags = 0;
      if(strcasecmp(val,"real")==0)
	rf->flags |= FLAG_REAL;
    }
    else if(strcasecmp(key, "no. variables")==0){
      field_val(&buf,&val);
      rf->nvariables = strtol(val,NULL,10);
    }
    else if(strcasecmp(key, "no. points")==0){
      field_val(&buf,&val);
      rf->points = strtol(val,NULL,10);
    }
    else if(strcasecmp(key, "variables")==0){
      rf->variables = malloc(rf->nvariables * sizeof(struct variable));
      while(*buf++ !='\n');
      for(int i=0;i<rf->nvariables;i++){
	size_t index = strtol(buf,&buf,0);
	skip_space(&buf);
	rf->variables[index].name = buf;
	rf->variables[index].data = calloc(rf->points,sizeof(double));
	while(*buf && !isspace(*buf)){buf++;}
	*buf++=0;
	skip_space(&buf);
	if(prefix("time", buf)){
	  rf->variables[index].type = VT_TIME;
	}
	if(prefix("voltage", buf)){
	  rf->variables[index].type = VT_VOLTAGE;
	}
	if(prefix("current", buf)){
	  rf->variables[index].type = VT_CURRENT;
	}
	
	while(*buf && !isspace(*buf)){buf++;}
	skip_space(&buf);
      }
    }
    else if(strcasecmp(key, "binary")==0){
      printf("buf\n");
      rf->buf = buf+1;
    }
    if(!cont)
      break;
  }
  read_variable_data(rf);
  return rf;
}
