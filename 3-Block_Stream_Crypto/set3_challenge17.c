#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/pkcs7.h"
#include "../headers/memory_output.h"
#include "../headers/pair.h"

#define BLOCK_SIZE 16

// needed to modify to change unpadding at end of function
byte_string* aes_cbc_decrypt_mod(byte_string* c_bytes, byte_string* k_bytes, byte_string* iv_bytes, size_t block_size) {
  byte_string* p_bytes = bytes_init(c_bytes->size);
  assertm(iv_bytes->size == 16, "~ iv_bytes should be 16 bytes in length");
  byte_string* prev_iv_bytes = bytes_clone(iv_bytes);

  for (size_t i = 0; i < c_bytes->size; i += block_size) {
    byte_string* tmp_c_bytes       = bytes_from((char*)c_bytes->data + i, block_size);
    byte_string* ebc_block_decrypt = aes_ecb_decrypt(tmp_c_bytes, k_bytes, block_size);
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

pair* get_secret(byte_string* key) {
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

int oracle(byte_string* ciphertext, byte_string* iv, byte_string* key) {
  byte_string* plaintext = aes_cbc_decrypt_mod(ciphertext, key, iv, BLOCK_SIZE);
  mem_output(plaintext);
  return pkcs7_pad_validator(plaintext);
}

int main() {

  byte_string* iv,
             * secret,
             * key = gen_rand(16);

  // get iv and our ciphertext from get_secret
  struct pair* p_ = get_secret(key);
  secret = p_->a;
  iv = p_->b;

  if (oracle(secret, iv, key)) {
    printf("secret decrypted is valid!\n");
  } else {
    printf("secret decrypted doesn't have valid padding\n");
  }

  free(p_);
  bytes_free(key);
  bytes_free(iv);
  bytes_free(secret);
  return 0;
}
