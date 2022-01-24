#include "../headers/pair.h"

#include <stdlib.h>

pair* create_pair(void* a, void* b) {
  pair* p_ = malloc(sizeof(pair));
  *p_ = (pair) { a, b };
  return p_;
}
