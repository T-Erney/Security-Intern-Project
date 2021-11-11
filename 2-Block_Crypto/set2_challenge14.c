#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/pkcs7.h"
#include "../headers/memory_output.h"

byte_string* gen_rand(size_t size) {
  byte_string* k = bytes_init(size);

  time_t t;
  srand(time(&t) + size);
  for (uint8_t i = 0; i < size; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}

byte_string* aes_encrypt_oracle(byte_string* p_bytes, byte_string* k_bytes) {
  char unk_string[] = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
  byte_string* unk_bytes = base64_to_bytes(unk_string);

  size_t p_size = (p_bytes != NULL) ? p_bytes->size : 0;

  byte_string* p_unk_bytes = bytes_init(p_size + unk_bytes->size);
  for (size_t i = 0; i < p_size; i += 1)          bytes_append(p_unk_bytes, p_bytes->data[i]);
  for (size_t i = 0; i < unk_bytes->size; i += 1) bytes_append(p_unk_bytes, unk_bytes->data[i]);

  byte_string* c_bytes = bytes_init(p_unk_bytes->size);

  for (size_t i = 0; i < p_unk_bytes->size; i += 16) {
    byte_string* tmp_bytes = bytes_from((char*)p_unk_bytes->data + i, 16);
    byte_string* enc_bytes = aes_ecb_encrypt(tmp_bytes, k_bytes, 16);
    for (size_t j = 0; j < 16; j += 1) bytes_append(c_bytes, enc_bytes->data[j]);

    // bytes_free(tmp_bytes); // this causes a double free error for some reason
    bytes_free(enc_bytes);
  }

  bytes_free(p_unk_bytes);
  bytes_free(unk_bytes);
  return c_bytes;
}

byte_string* break_aes_ecb(byte_string* k_bytes) {
  size_t block_size = 0;
  size_t block_count = 0;
  {
    // get block size of cipher
    // to do this we need to just keep appending to our oracle until a new block appears
    byte_string* c_bytes = aes_encrypt_oracle(NULL, k_bytes);
    byte_string* tmp_bytes = bytes_init(16);
    byte_string* p_bytes = NULL;

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

    block_size  = new_size - origin_size;
    block_count = origin_size / block_size;

    bytes_free(p_bytes);
    bytes_free(c_bytes);
    bytes_free(tmp_bytes);
  }

  printf("Block Size  :: %ld\n", block_size);
  printf("Block Count :: %ld\n", block_count);

  {
    // now that we have the block size, we need to check that this is an aes_ecb cipher
    // good for us, we have a handy function for this
    byte_string* p_bytes = string_to_bytes("YELLOW SUBMARINEYELLOW SUBMARINE");
    byte_string* c_bytes = aes_encrypt_oracle(p_bytes, k_bytes);
    size_t is_ecb = aes_ecb_detect(c_bytes, block_size);

    assertm(is_ecb > 0, "Well, something is wrong here!");
  }

  // now that we have ensured we are in fact working with aes_ecb encryption, we now craft an attack
  // the attact will use a block insert where we insert new text that is size block_size - 1 into the oracle
  // we then brute-force to find the first byte, then the second, and so on until we solve the cipher
  byte_string* p_bytes = bytes_init(block_count * block_size);
  size_t p_size = block_size * block_count;

  for (size_t i = 0; i < p_size; i += 1) {

    // DIFFERENT FROM CHALLENGE 12!
    // create a random byte_string with a random size.
    time_t t;
    srand(i + time(&t));
    size_t random_bytes_size = rand() % p_size;
    byte_string* random_bytes = gen_rand(random_bytes_size);

    byte_string* pad = bytes_init(p_size - p_bytes->size - 1);
    for (size_t j = 0; j < p_size - p_bytes->size - 1; j += 1)
      bytes_append(pad, (j < random_bytes_size) ? random_bytes->data[j] : 'A');

    byte_string* c_bytes = aes_encrypt_oracle(pad, k_bytes);

    for (uint16_t c = 0; c < 256; c += 1) {
      uint8_t found = 0;
      byte_string* guess_bytes = bytes_init(p_size);
      for (size_t k = 0; k < pad->size; k += 1) bytes_append(guess_bytes, pad->data[k]);
      for (size_t k = 0; k < p_bytes->size; k += 1) bytes_append(guess_bytes, p_bytes->data[k]);
      bytes_append(guess_bytes, c);

      //mem_output(guess_bytes);

      byte_string* enc_guess_bytes = aes_encrypt_oracle(guess_bytes, k_bytes);


      // get subsection of enc_guess_bytes and c_bytes to compare
      {
        byte_string* sub_enc_guess = bytes_from((char*)enc_guess_bytes->data, p_size);
        byte_string* sub_c_bytes   = bytes_from((char*)c_bytes->data, p_size);

        if (bytes_cmp(sub_enc_guess, sub_c_bytes) == 0) {
          bytes_append(p_bytes, c);
          found = 1;
        }

        bytes_free(sub_enc_guess);
        bytes_free(sub_c_bytes);
      }

      bytes_free(guess_bytes);
      bytes_free(enc_guess_bytes);

      if (found > 0) break;
    }

    bytes_free(pad);
    bytes_free(c_bytes);
  }

  return p_bytes;
}

int main() {

  byte_string* k_bytes = gen_rand(16);
  byte_string* p_bytes = break_aes_ecb(k_bytes);
  mem_output(p_bytes);


  bytes_free(k_bytes);
  bytes_free(p_bytes);
  return 0;
}
