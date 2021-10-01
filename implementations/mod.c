#include "mod.h"

size_t mod(size_t x, size_t y) {
  if (x < 0) {
    while (x < 0) x += y;
    return x;
  } else {
    x % y;
  }
}
