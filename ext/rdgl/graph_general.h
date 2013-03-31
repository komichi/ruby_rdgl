#ifndef RDGL_GRAPH_GENERAL_H
#define RDGL_GRAPH_GENERAL_H

//#define DEBUG
//#undef DEBUG

#include "ruby.h" // do ruby integration?
#define RDGL_FUNC_MASTER __PRETTY_FUNCTION__

#ifdef DEBUG
#define LOG_DEBUG(string, args...) \
  { fprintf(stderr, "DEBUG: %s: ", RDGL_FUNC_MASTER);  \
    fprintf(stderr, string "\n", ##args); }
#else
#define LOG_DEBUG(string, args...) ;
#endif

#define LOG_ERROR(string, args...) \
  { fprintf(stderr, "ERROR: %s: ", RDGL_FUNC_MASTER);  \
    fprintf(stderr, string "\n", ##args); }

#endif

#ifndef DBL_MAX
#ifdef IS_MACOSX
#include <float.h>
#elif IS_LINUX
#include <values.h>
#else
#error "where is DBL_MAX defined here?"
#endif
#define RDGL_GRAPH_INFINITY DBL_MAX
#endif

#ifndef NUM2ULL
#define NUM2ULL(x) rb_num2ull(x)
#endif

