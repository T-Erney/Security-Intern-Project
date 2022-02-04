#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/pkcs7.h"
#include "../headers/memory_output.h"
#include "../headers/pair.h"
#include "../headers/xor.h"
#include "../headers/assert.h"

#define BLOCK_SIZE 16
static byte_string* key;

// needed to modify to change unpadding at end of function
byte_string* aes_cbc_decrypt__(byte_string* c_bytes, byte_string* k_bytes, byte_string* iv_bytes, size_t block_size) {
  byte_string* p_bytes = bytes_init(c_bytes->size);
  assertm(iv_bytes->size == 16, "~ iv_bytes should be 16 bytes in length");
  byte_string* prev_iv_bytes = bytes_clone(iv_bytes);

  for (size_t i = 0; i < c_bytes->size; i += block_size) {
    byte_string* tmp_c_bytes       = bytes_from((char*)c_bytes->data + i, block_size);
    byte_string* ebc_block_decrypt = aes_ecb_decrypt(tmp_c_bytes, k_bytes, block_size);
    assertm(ebc_block_decrypt->size == 16, "~ decryption should be 16 bytes");
    byte_string* xor_bytes         = bytes_xor(prev_iv_bytes, ebc_block_decrypt);

    for (size_t j = 0; j < block_size; j += 1) bytes_append(p_bytes, xor_bytes->data[j]);

    bytes_free(prev_iv_bytes);
    prev_iv_bytes = bytes_clone(tmp_c_bytes);

    bytes_free(xor_bytes);
    bytes_free(tmp_c_bytes);
    bytes_free(ebc_block_decrypt);
  }

  // byte_string* unpadded_p_bytes = pkcs7_unpad_bytes(p_bytes, block_size);
  // bytes_free(p_bytes);
  bytes_free(prev_iv_bytes);
  // return unpadded_p_bytes;
  return p_bytes;
}

byte_string* gen_rand(size_t size) {
  byte_string* k = bytes_init(size);

  time_t t;
  srand(time(&t) + size);
  for (uint8_t i = 0; i < size; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}

pair* get_secret() {
  char* secrets[] = {
    "MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=",
    "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic=",
    "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==",
    "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==",
    "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl",
    "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==",
    "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==",
    "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=",
    "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=",
    "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93"
  };

  srand(time(0));
  byte_string* iv     = gen_rand(16);
  byte_string* secret = aes_cbc_encrypt(pkcs7_pad_bytes(base64_to_bytes(secrets[rand() % 10]), BLOCK_SIZE), key, iv, BLOCK_SIZE);
  pair* p_     = create_pair((void*)secret, (void*)iv);
  return p_;
}

int oracle(byte_string* ciphertext, byte_string* iv) {
  byte_string* plaintext = aes_cbc_decrypt__(ciphertext, key, iv, BLOCK_SIZE);
  return pkcs7_pad_validator(plaintext);
}

byte_string* crack_block(byte_string* block1, byte_string* block2) {
  byte_string* cracked_bytes = bytes_init(AES_BLOCK_SIZE);
  size_t padding = 1, start_guess = 0;
  while (cracked_bytes->size < AES_BLOCK_SIZE) {
    size_t guess;
    for (guess = start_guess; guess <= 255; guess += 1) {
      byte_string* corrupted_bytes = bytes_clone(block1);
      for (size_t byte = 1; byte < padding + 1; byte += 1) {
        if (byte < padding) {
          corrupted_bytes->data[16 - byte] =
            _xor(_xor(block1->data[16 - byte], cracked_bytes->data[cracked_bytes->size - byte]), padding);
        } else {
          corrupted_bytes->data[16 - byte] =
            _xor(_xor(block1->data[16 - byte], guess), padding);
        }
      }


      if (oracle(block2, corrupted_bytes)) {
        bytes_prepend(cracked_bytes, guess);
        padding += 1;
        mem_output(cracked_bytes);
        break;
      }

      bytes_free(corrupted_bytes);
    }

    if (guess > 255) {
      start_guess = cracked_bytes->data[0] + 1;
      bytes_pop_front(cracked_bytes);
    }
  }

  return cracked_bytes;
}

int main() {
  byte_string* iv,
            * secret;
  key = gen_rand(16);

  // get iv and our ciphertext from get_secret
  pair* p_ = get_secret();
  secret = (byte_string*)p_->a;
  iv = (byte_string*)p_->b;

  mem_output(iv);
  mem_output(secret);

  byte_string* p_bytes = bytes_init(secret->size);
  byte_string* c_bytes = bytes_init(secret->size + iv->size);
  for (size_t i = 0; i < iv->size; i += 1)     bytes_append(c_bytes, iv->data[i]);
  for (size_t i = 0; i < secret->size; i += 1) bytes_append(c_bytes, secret->data[i]);
  for (size_t i = 0; i < (secret->size / AES_BLOCK_SIZE); i += 1) {
    byte_string* block1 = bytes_from((const char*)c_bytes->data + (i * 16), 16);
    byte_string* block2 = bytes_from((const char*)c_bytes->data + ((i + 1) * 16), 16);

    mem_output(block1);
    mem_output(block2);

    byte_string* crack = crack_block(block1, block2);
    for (size_t i = 0; i < crack->size; i += 1) bytes_append(p_bytes, crack->data[i]);
  }
  // byte_string* bytes = pkcs7_unpad_bytes(p_bytes, 16);
  mem_output(p_bytes);

  free(p_);
  bytes_free(key);
  bytes_free(iv);
  bytes_free(secret);
  return 0;
}
