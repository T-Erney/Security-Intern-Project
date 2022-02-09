#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/assert.h"
#include "../headers/memory_output.h"
#include "../headers/letter_freq.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define debug fprintf(stderr, "... HIT %d ...\n", __LINE__)


byte_string* gen_rand(size_t size) {
  byte_string* k = bytes_init(size);

  time_t t;
  srand(time(&t) + size);
  for (uint8_t i = 0; i < size; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}

static byte_string* k_bytes;

unsigned char get_keystream_byte(byte_string* bytes) {
  unsigned char byte = 0;
  float score = 0.0;

  for (size_t key = 0; key < 256; key += 1) {
    float _score = letter_freq_analysis(single_xor(bytes, key));

    if (_score > score) {
      score = _score;
      byte = key;
    }
  }

  return byte;
}

byte_string** aes_ctr_nonce_crack(byte_string** bytes_arr, size_t count) {
  byte_string** p_bytes_arr = malloc(sizeof(byte_string*) * count);
  byte_string* keystream = bytes_init(count);

  // find the largest of the byte_strings
  size_t size = 0xffffffff;
  for (size_t i = 0; i < count; i += 1) if (size > bytes_arr[i]->size) size = bytes_arr[i]->size;

  // form a column of the i-th character for each ciphertext
  for (size_t i = 0; i < size; i += 1) {
    byte_string* col_bytes = bytes_init(count);
    for (size_t j = 0; j < count; j += 1) {
      bytes_append(col_bytes, (j < bytes_arr[j]->size) ? bytes_arr[j]->data[i] : '\0');
    }
    bytes_append(keystream, get_keystream_byte(col_bytes));
  }

  printf("keystream (%ld):\n", keystream->size);
  mem_output(keystream);

  // create plaintexts with our generated keystream
  for (size_t i = 0; i < count; i += 1) {
    p_bytes_arr[i] = bytes_from((const char*)_xor_repeated_bytes(bytes_arr[i], keystream)->data, keystream->size);
  }

  return p_bytes_arr;
}


int main() {
    FILE* file = fopen("3-Block_Stream_Crypto/res/set3_challenge20.txt", "r");
    assert(file != NULL);
    k_bytes = gen_rand(16);
    size_t nonce = 0;

    byte_string** c_bytes_arr = malloc(sizeof(byte_string*) * 60); // 60 lines in the file
    size_t index = 0;

    // create array of ctr ciphertexts
    size_t size = 100;
    char* buffer = malloc(size);
    for (size_t i = 0; i < 60; i += 1) {
      getline(&buffer, &size, file);
      buffer[strlen(buffer) - 1] = 0;
      c_bytes_arr[index++] = aes_ctr_mode(base64_to_bytes(buffer), k_bytes, nonce);
      for (size_t i = 0; i < 100; i += 1) buffer[i] = 0;
    }

    // crack the ciphers
    byte_string** p_bytes_arr = aes_ctr_nonce_crack(c_bytes_arr, 60);
    for (size_t i = 0; i < 60; i += 1) {
      char* str = bytes_to_string(p_bytes_arr[i]);
      printf("%s", str);
      // mem_output(p_bytes_arr[i]);
    }
    printf("\n");


    return 0;
}
