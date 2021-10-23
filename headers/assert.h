#ifndef ASSERT
#define ASSERT

#include <stdio.h>

#define assert(cond)                                    \
  if (!(cond)) {                                        \
    printf("%s:%d: [assert failed] \'%s\' is false\n", __FILE__, __LINE__, #cond); \
    exit(1);                                            \
  }

#define assertm(cond, msg)                                       \
  if (!(cond)) {                                                 \
    printf("%s:%d: [assert failed] \'%s\' is false :: %s", __FILE__, __LINE__, #cond, msg); \
    exit(1);                                                     \
  }

#endif
