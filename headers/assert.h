#ifndef ASSERT
#define ASSERT

#include <stdio.h>

#define assert(cond)                                    \
  if (!(cond)) {                                        \
    printf("[assert failed] \'%s\' is false\n", #cond); \
    exit(1);                                            \
  }

#define assertm(cond, msg)                                       \
  if (!(cond)) {                                                 \
    printf("[assert failed] \'%s\' is false\n\t%s", #cond, msg); \
    exit(1);                                                     \
  }

#endif
