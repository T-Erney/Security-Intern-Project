#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"
#include "hamming_dist.h"

static char letter_freq_order[] = {
                                   ' ', 'e', 't', 'a', 'o', 'i', 'n', 's', 'h', 
                                   'r', 'd', 'l', 'c', 'u', 'm', 'w', 'f', 'g', 
                                   'y', 'p', 'b', 'v', 'k', 'j', 'x', 'q', 'z'
};

static float letter_freq_value[] = {
                                    13.000, 12.702, 9.056, 8.167, 7.507, 6.966, 6.749, 6.327, 6.094,
                                    5.987,  4.253, 4.025, 2.782, 2.758, 2.406, 2.360, 2.228, 2.015,
                                    1.974,  1.929, 1.492, 0.978, 0.772, 0.153, 0.150, 0.095, 0.074
};


// ---

char* substr(char* str, size_t a, size_t b) {
  char* s = malloc(sizeof(char) * b + 1);
  memcpy(s, str + a, b);
  s[b] = 0;

  return s;
}

// ---



int main() {
  char* b64 = NULL;
  {
    FILE* file = fopen("6.txt", "r");
    size_t size = 0;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    b64 = malloc(sizeof(char) * size);
    fread(b64, 1, size - 1, file);
    b64[size - 1] = 0;

    fclose(file);
  }

  { // TEST OUR BASE64
    byte_string* bytes = base64_to_bytes(b64);
    printf("\t*** bytes->size  = %ld ***\n", bytes->size);
    mem_output(bytes); // dump the bytes into view
    bytes_free(bytes);
  }

  byte_string* c_bytes = base64_to_bytes(b64);


  double normal_dist = 999.0;
  size_t K = 0;
  for (size_t key = 2; key < 40; key += 1) {
    double dist = 0;
    double iter = 0;
    for (size_t i = 0; i < c_bytes->size; i += key) {
      byte_string* x_bytes = bytes_init(key);
      byte_string* y_bytes = bytes_init(key);
      memcpy(x_bytes->data, c_bytes->data + i, key);
      memcpy(y_bytes->data, c_bytes->data + i + key, key);
      x_bytes->size = key;
      y_bytes->size = key;

      dist += hamming_dist(x_bytes, y_bytes) / key;
      iter += 1;

      bytes_free(x_bytes);
      bytes_free(y_bytes);
    }

    dist /= iter;

    printf("*** key %ld *** normal_dist/item = %lf || dist = %lf ***\n", key, normal_dist/iter, dist/iter);
    if ((normal_dist) > (dist)) {
      normal_dist = dist;
      K = key;
    }
  }

  printf("*** Key size is %ld ***\n", K);

  bytes_free(c_bytes);
  return 0;
}
