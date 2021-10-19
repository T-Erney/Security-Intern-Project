#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/mod.h"
#include "../headers/memory_output.h"

byte_string* gen_key() {
  byte_string* k = bytes_init(16);

  srand(time(NULL));
  for (uint8_t i = 0; i < 16; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}

byte_string* aes_encrypt_oracle(byte_string* p_bytes, byte_string* k_bytes, size_t block_size) {
  // randomly choose a number between 5 - 10
  // this number becomes your padding before and after your plaintext?
  // padding = 'a' 'b' 'y' 'z' + (number of bytes for padding) on our append
  // our prepend will be just random
  srand(time(null));
  uint8_t pad = mod(rand(), 5) + 5; // get number between 5 - 10

  // now that we have our pad size, its time to prepend and append
  byte_string* prepend = bytes_init(pad);
  byte_string* append  = bytes_init(pad);

  for (uint8_t i = 0; i < pad; i += 1) {
    bytes_append(append,  (uint8_t)rand(128));
    bytes_append(prepend, (uint8_t)rand(128));
  }

  // get count of repeated blocks. If there are > 0 repeated blocks, we can assume ebc, else we assume cbc
  

  bytes_free(append);
  bytes_free(prepend);
}

int main() {

  printf("k := \n");
  byte_string* k = gen_key();
  mem_output(k);

  return 0;
}
