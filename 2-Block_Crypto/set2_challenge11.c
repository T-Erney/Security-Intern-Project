#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/pkcs7.h"
#include "../headers/mod.h"
#include "../headers/memory_output.h"

byte_string* gen_rand(size_t size) {
  byte_string* k = bytes_init(size);

  srand(time(NULL));
  for (uint8_t i = 0; i < size; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}

size_t repeated_blocks(byte_string* c_bytes, size_t block_size) {
  size_t chunk_count = c_bytes->size / block_size;
  byte_string** chunks = malloc(sizeof(byte_string*) * chunk_count);

  for (size_t i = 0, j = 0; i < chunk_count; i += 1) {
    chunks[j] = bytes_from((char*)c_bytes->data + (i * block_size), block_size);
    j += 1;
  }

  qsort(chunks, chunk_count, sizeof(byte_string*),
       ({int comp(const void* a, const void* b) {
           byte_string ** x = (byte_string**)a, ** y = (byte_string**)b;
           for (size_t i = 0; i < (*x)->size; i += 1) {
             if (x[0]->data[i] < y[0]->data[i]) return -1;
             if (x[0]->data[i] > y[0]->data[i]) return 1;
           }
           return 0;
        } comp; }));

  size_t rep = 0;
  for (size_t i = 0; i < chunk_count - 1; i += 1) {
    assert(chunks[i]);
    assert(chunks[i + 1]);
    if (bytes_cmp(chunks[i], chunks[i + 1]) == 0) {
      rep += 1;
    }
  }

  for (size_t i = 0; i < chunk_count; i += 1) {
    bytes_free(chunks[i]);
  }
  free(chunks);
  return rep;
}

byte_string* aes_encrypt_oracle(byte_string* p_bytes, byte_string* k_bytes, size_t block_size) {
  // get count of repeated blocks. If there are > 0 repeated blocks, we can assume ebc, else we assume cbc
  byte_string* bytes = NULL;
  srand(time(NULL));
  int op = rand() % 2;
  if (op == 0) { // ECB Mode
    printf("Encrypt with ECB\n");

    bytes = bytes_init(p_bytes->size);
    for (size_t i = 0; i < p_bytes->size / block_size; i += 1) {
      byte_string* tmp_bytes = bytes_from(p_bytes->data + (i * block_size), block_size);
      byte_string* enc_bytes = aes_ecb_encrypt(tmp_bytes, k_bytes, block_size);

      for (size_t j = 0; j < block_size; j += 1) bytes_append(bytes, enc_bytes->data[j]);

      bytes_free(tmp_bytes);
      bytes_free(enc_bytes);
    }
  } else {  // CBC Mode
    printf("Encrypt with CBC\n");

    byte_string* iv_bytes = gen_rand(16);
    bytes = aes_cbc_encrypt(p_bytes, k_bytes, iv_bytes, block_size);
    bytes_free(iv_bytes);
  }

  printf("p_bytes->size :: %ld\n", p_bytes->size);
  printf("bytes->size   :: %ld\n", bytes->size);

  return bytes;
}



int main() {

  char* buffer = NULL;
  {
    FILE* file = fopen("res/11.txt", "r");
    assert(file != NULL);

    fseek(file, 0, SEEK_END);
    size_t s = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(s + 1);
    buffer[s] = 0;

    fread(buffer, 1, s, file);
    fclose(file);
  }

  byte_string* k_bytes = gen_rand(16);
  byte_string* p_bytes = string_to_bytes(buffer);
  byte_string* c_bytes = aes_encrypt_oracle(p_bytes, k_bytes, 16);
  uint8_t      is_ecb  = repeated_blocks(c_bytes, 16);

  printf("k_bytes := \n");
  mem_output(k_bytes);
  printf("\n");

  printf("is_ecb :: %d\n", is_ecb);
  printf("is_ecb detected that ");
  if (is_ecb > 0) printf(" encryption aes_ecb was performed\n");
  else            printf(" encryption aes_cbc was performed\n");

  free(buffer);
  bytes_free(k_bytes);
  bytes_free(p_bytes);
  bytes_free(c_bytes);

  return 0;
}
