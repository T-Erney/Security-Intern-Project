#ifndef PAIR_H
#define PAIR_H

struct pair {
  void* a;
  void* b;
};

struct pair* create_pair(void*, void*);

#endif // PAIR_H
