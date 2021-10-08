#ifndef ASSERT
#define ASSERT

#define assert(cond)                                    \
  if (!(cond)) {                                        \
    printf("[assert failed] \'%s\' is false\n", #cond); \
    exit(1);                                            \
  }

#endif
