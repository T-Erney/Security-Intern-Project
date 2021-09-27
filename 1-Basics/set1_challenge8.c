#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"

int bytes_cmp(byte_string* x, byte_string* y) {
  for (size_t i = 0; i < x->size; i += 1) {
    if (x->data[i] < y->data[i]) return -1;
    if (x->data[i] > y->data[i]) return 1;
  }
  return 0;
}

int cmp_func(const void* a, const void* b) {
  return bytes_cmp((byte_string*)a, (byte_string*)b);
}

int main() {

  char* hex = NULL;
  char* key = "YELLOW SUBMARINE";
  {
    FILE* file = fopen("8.txt", "r");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    hex = malloc(sizeof(char) * size + 1);
    hex[size] = 0;

    fread(hex, 1, size, file);
    fclose(file);
  }

  byte_string* c_bytes = hex_to_bytes(hex);
  byte_string* k_bytes = string_to_bytes(key);

  // set AES decrypt key
  AES_KEY aes_decrypt_key;
  AES_set_decrypt_key(k_bytes->data, 128, &aes_decrypt_key);

  // create chunks of size 16
  byte_string** chunks = malloc(sizeof(byte_string*) * (c_bytes->size / 16));
  size_t k = 0;
  for (size_t i = 0; i < c_bytes->size;) {
    chunks[k] = bytes_init(16);
    for (size_t j = 0; j < 16; j += 1) {
      bytes_append(chunks[k], c_bytes->data[i++]);
    }
    k += 1;
  }

  // sort then compare chunks
  qsort(chunks, c_bytes->size / 16, sizeof(byte_string*), &cmp_func);
  for (size_t i = 0; i < c_bytes->size / 16; i += 1) {
    mem_output(chunks[i]);
  }

  bytes_free(c_bytes);
  bytes_free(k_bytes);
  return 0;
}
