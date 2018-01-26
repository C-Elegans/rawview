#ifndef _RAWFILE_H_
#define _RAWFILE_H_
#include <stdlib.h>
enum vartype{
  VT_VOLTAGE,
  VT_CURRENT,
  VT_TIME
};
enum flags{
  FLAG_REAL=1,
  FLAG_COMPLEX=2
};
struct variable {
  char* name;
  enum vartype type;
  double* data;
};
struct rawfile {
  char* title;
  char* date;
  char* plotname;
  enum flags flags;
  int nvariables;
  size_t points;
  struct variable* variables;
  void* buf;
};

struct rawfile* parse_rawfile(char* filename);
char* unit_from_vartype(enum vartype);

#endif
