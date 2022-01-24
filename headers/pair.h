#ifndef PAIR_H
#define PAIR_H

typedef struct pair_t {
  void* a;
  void* b;
} pair;

pair* create_pair(void*, void*);

#endif // PAIR_H
