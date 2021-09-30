#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"


int bytes_cmp(byte_string** a, byte_string** b) {
  byte_string* x = a[0];
  byte_string* y = b[0];
  for (size_t i = 0; i < x->size; i += 1) {
    if (x->data[i] < y->data[i]) return -1;
    if (x->data[i] > y->data[i]) return 1;
  }
  return 0;
}

int cmp_func(const void* a, const void* b) {
  return bytes_cmp((byte_string**)a, (byte_string**)b);
}

int main() {

  char*  hex = NULL;
  size_t lines = 0,
         cols  = 0;
  {
    FILE* file = fopen("8.txt", "r");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(sizeof(char) * size + 1);
    buffer[size] = 0;

    fread(buffer, 1, size, file);
    fclose(file);

    // get line count
    for (size_t i = 0; i < size; i += 1) {
      if (buffer[i] == '\n') lines += 1;
    }
    // get col count
    for (size_t i = 0; i < size; i += 1) {
      if (buffer[i] == '\n') break;
      cols += 1;
    }

    // create hex buffer with no spaces
    hex = calloc(sizeof(char), size);
    size_t i = 0;
    char* nl = buffer;
    char* old_nl = NULL;
    size_t l = 0;
    while (1) {
      old_nl = nl;
      if ((nl = strchr(old_nl, '\n')) == NULL) {
        break;
      }
      nl += 1;
      size_t len = (size_t)(nl - old_nl) - 1;
      memcpy(hex + i, old_nl, len);
      i += len;
    }

    free(buffer);
  }

  byte_string* c_bytes = hex_to_bytes(hex);

  // create chunks of size 16
  byte_string** chunks = malloc(sizeof(byte_string*) * (c_bytes->size / 16));
  byte_string** sorted_chunks = malloc(sizeof(byte_string*) * (c_bytes->size / 16));
  size_t k = 0;
  for (size_t i = 0; i < c_bytes->size;) {
    chunks[k] = bytes_init(16);
    sorted_chunks[k] = bytes_init(16);
    for (size_t j = 0; j < 16; j += 1) {
      bytes_append(chunks[k], c_bytes->data[i]);
      bytes_append(sorted_chunks[k], c_bytes->data[i]);
      i += 1;
    }
    k += 1;
  }

  // sort then compare chunks
  size_t index = 0;
  size_t repeated = 0;
  qsort(sorted_chunks, c_bytes->size / 16, sizeof(byte_string*), &cmp_func);
  for (size_t i = 0; i < c_bytes->size / 16 - 1; i += 1) {
    if (bytes_cmp(&sorted_chunks[i], &sorted_chunks[i + 1]) == 0) {
      printf("[MATCH] sorted_chunks[%ld] * sorted_chunks[%ld]:\n", i, i + 1);
      mem_output(sorted_chunks[i]);
      index = i;
      repeated += 1;
    }
  }

  // find original chunk and line number
  size_t origin = 0;
  for (int i = 0; i < c_bytes->size / 16; i += 1) {
    if (bytes_cmp(&sorted_chunks[index], &chunks[i]) == 0) {
      origin = i;
      break;
    }
  }

  size_t beginning_index_of_chunk = 16 * origin;

  // brute force line calculation
  size_t line = 0;
  for (int i = 0; i < lines; i += 1) {
    for (int j = 0; j < cols; j += 1) {
      if ((i * cols + j) == beginning_index_of_chunk) {
        line = i;
      }
    }
  }

  printf("Ciphertext Hex:   %s\n",  bytes_to_hex(chunks[index]));
  printf("Repeating Blocks: %ld\n", repeated);

  bytes_free(c_bytes);
  return 0;
}
