#include "../headers/pair.h"

#include <stdlib.h>

struct pair* create_pair(void* a, void* b) {
  struct pair* p_ = malloc(sizeof(struct pair));
  *p_ = (struct pair) { a, b };
  return p_;
}
