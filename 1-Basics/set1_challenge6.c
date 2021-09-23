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

static double letter_freq_value[] = {
                                    13.000, 12.702, 9.056, 8.167, 7.507, 6.966, 6.749, 6.327, 6.094,
                                    5.987,  4.253, 4.025, 2.782, 2.758, 2.406, 2.360, 2.228, 2.015,
                                    1.974,  1.929, 1.492, 0.978, 0.772, 0.153, 0.150, 0.095, 0.074
};

double get_letter_freq(char c) {
  double val = 0.0;
  for (size_t i = 0; i < 27; i += 1) {
    if (c == letter_freq_order[i]) val = letter_freq_value[i];
  }

  return val;
}

// ---

char* substr(char* str, size_t a, size_t b) {
  char* s = malloc(sizeof(char) * b + 1);
  memcpy(s, str + a, b);
  s[b] = 0;

  return s;
}

// ---

double get_score(byte_string* bytes) {
  double score = 0.0;
  for (size_t i = 0; i < bytes->size; i += 1) {
    score += get_letter_freq(bytes->data[i]);
  }

  return score / bytes->size;
}

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

    if ((normal_dist) > (dist)) {
      normal_dist = dist;
      K = key;
    }
  }

  byte_string** chunks = malloc(sizeof(byte_string) * K);
  for (size_t i = 0; i < K; i += 1) {
    chunks[i] = bytes_init(c_bytes->size / K);
    for (size_t j = i; j < c_bytes->size; j += K) {
      bytes_append(chunks[i], c_bytes->data[j]);
    }
  }

  char* _key = malloc(sizeof(char) * K + 1);
  _key[K] = 0;

  for (size_t i = 0; i < K; i += 1) {
    size_t best_score = 0;
    size_t best_key   = 0;
    byte_string* bytes = chunks[i];

    for (unsigned char j = 0; j < 128; j += 1) {
      byte_string* t_bytes = bytes_init(bytes->size);

      for (size_t k = 0; k < bytes->size; k += 1) {
        unsigned char xor = _xor(bytes->data[k], j);
        bytes_append(t_bytes, xor);
      }

      double score = get_score(t_bytes);

      if (score > best_score) {
        best_score = score;
        best_key = j;
        bytes_free(t_bytes);
      }
    }

    _key[i] = best_key;
  }

  printf("_key = %s\n", _key);

  free(_key);
  bytes_free(c_bytes);
  return 0;
}
