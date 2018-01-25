#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_file(char* filename){
  FILE* f = fopen(filename, "r");
  fseek(f, 0, SEEK_END);
  ssize_t size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* buf = malloc(size+1);
  fread(buf,1,size,f);
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

int main(int argc, char** argv){
  if(argc != 2){
    fprintf(stderr, "Usage: %s [rawfile]\n",argv[0]);
    return 1;
  }
  char* sav = read_file(argv[1]);
  char* buf = sav;
  char *key, *val;
  while(1){
    if(field_key(&buf, &key)){
      if(strcmp(key, "Title") ==0){
	field_val(&buf,&val);
	printf("%s - %s\n",key,val);
      }
    }
    else{
      break;
    }
  }
  

  free(sav);

}
