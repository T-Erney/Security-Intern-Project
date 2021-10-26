#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/pkcs7.h"
#include "../headers/memory_output.h"

byte_string* gen_rand(size_t size) {
  byte_string* k = bytes_init(size);

  srand(time(NULL));
  for (uint8_t i = 0; i < size; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}

byte_string* aes_encrypt_oracle(byte_string* p_bytes, byte_string* k_bytes) {
  char unk_string[] = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
  byte_string* unk_bytes = base64_to_bytes(unk_string);

  byte_string* p_unk_bytes    = bytes_init(p_bytes->size + unk_bytes->size);
  for (size_t i = 0; i < p_bytes->size; i += 1)   bytes_append(p_unk_bytes, p_bytes->data[i]);
  for (size_t i = 0; i < unk_bytes->size; i += 1) bytes_append(p_unk_bytes, unk_bytes->data[i]);

  byte_string* c_bytes = bytes_init(p_unk_bytes->size);

  for (size_t i = 0; i < p_unk_bytes->size; i += 16) {
    byte_string* tmp_bytes = bytes_from(p_unk_bytes->data + i, 16);
    byte_string* enc_bytes = aes_ecb_encrypt(tmp_bytes, k_bytes, 16);
    for (size_t j = 0; j < 16; j += 1) bytes_append(c_bytes, enc_bytes->data[j]);

    bytes_free(tmp_bytes);
    bytes_free(enc_bytes);
  }

  return c_bytes;
}

byte_string* break_aes_ecb(byte_string* k_bytes) {
  size_t block_size = 0;
  {
    // get block size of cipher
    // to do this we need to just keep appending to our oracle until a new block appears
    byte_string* p_bytes = bytes_init(1);
    byte_string* c_bytes = aes_encrypt_oracle(p_bytes, k_bytes);
    byte_string* tmp_bytes = bytes_init(16);

    size_t origin_size = c_bytes->size;
    size_t new_size    = 0;

    while (new_size <= origin_size) {
      bytes_free(p_bytes);
      bytes_free(c_bytes);

      bytes_append(tmp_bytes, (unsigned char)'A');
      p_bytes = bytes_clone(tmp_bytes);
      c_bytes = aes_encrypt_oracle(p_bytes, k_bytes);

      new_size = c_bytes->size;
    }

    block_size = new_size - origin_size;

    bytes_free(p_bytes);
    bytes_free(c_bytes);
    bytes_free(tmp_bytes);
  }

  {
    // now that we have the block size, we need to check that this is an aes_ecb cipher
    // good for us, we have a handy function for this
    byte_string* p_bytes = bytes_init(1);
    byte_string* c_bytes = aes_encrypt_oracle(p_bytes, k_bytes);
    size_t is_ecb = aes_ecb_detect(c_bytes, block_size);

    assertm(is_ecb > 0, "Well, something is wrong here!");
  }

  {
    // now that we have ensured we are in fact working with aes_ecb encryption, we now craft an attack
    // the attact will use a block insert where we insert new text that is size block_size - 1 into the oracle
    // we then brute-force to find the first byte, then the second, and so on until we solve the cipher
    byte_string* p_bytes = bytes_init(block_size - 1);
    for (size_t i = 0; i < block_size - 1; i += 1) bytes_append(p_bytes, 'A');
    byte_string* c_bytes = aes_encrypt_oracle(p_bytes, k_bytes);
    byte_string* tmp_bytes = bytes_from((char*)c_bytes->data, block_size);

    for (uint8_t c = 0; c <= 255; c += 1) {
      char* str = malloc(block_size + 1);
      for (size_t i = 0; i < block_size - 1; i += 1) str[i] = 'A';
      str[block_size - 1] = c;
      str[block_size] = 0;

      byte_string* comp_bytes = string_to_bytes(str);

      if (memcmp(comp_bytes->data, c_bytes->data, block_size) == 0) {

      }
    }
  }
}

int main() {

  byte_string* k_bytes = gen_rand(16);



  return 0;
}
