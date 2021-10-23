#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>

#include "../headers/conversions.h"
#include "../headers/xor.h"
#include "../headers/mod.h"
#include "../headers/memory_output.h"
#include "../headers/assert.h"

byte_string* bytes_xor(byte_string* x_bytes, byte_string* y_bytes) {
  assert(x_bytes->size == y_bytes->size);
  byte_string* bytes = bytes_init(x_bytes->size);

  for (size_t i = 0; i < x_bytes->size; i += 1) {
    bytes_append(bytes, _xor(x_bytes->data[i], y_bytes->data[i]));
  }

  return bytes;
}

// pad bytes
byte_string* pkcs7_pad_bytes(byte_string* x_bytes, size_t block_size) {
  size_t pad_size = mod((block_size - x_bytes->size), block_size);
  if (x_bytes->size == block_size) return x_bytes;

  byte_string* bytes = bytes_init(x_bytes->size + pad_size);

  for (int i = 0; i < x_bytes->size + pad_size + 1; i += 1) {
    bytes_append(bytes, (i < x_bytes->size) ? x_bytes->data[i]: (unsigned char)pad_size);
  }

  return bytes;
}

// unpad bytes
byte_string* pkcs7_unpad_bytes(byte_string* bytes, size_t block_size) {
  // read last element and remove as many bytes as the value of that element
  if (bytes->size == block_size) return bytes_clone(bytes);

  size_t unpad = bytes->data[bytes->size - 1];
  size_t new_size = bytes->size - unpad;

  byte_string* new_bytes = bytes_init(new_size);

  for (size_t i = 0; i < new_size; i += 1) {
    bytes_append(new_bytes, bytes->data[i]);
  }

  return new_bytes;
}

byte_string* aes_ecb_encrypt(byte_string* p_bytes, byte_string* k_bytes, size_t block_size) {
  byte_string* c_bytes = bytes_init(p_bytes->size);
  byte_string* padded_p_bytes = pkcs7_pad_bytes(p_bytes, block_size);

  AES_KEY k;
  AES_set_encrypt_key(k_bytes->data, 128, &k);
  AES_ecb_encrypt(padded_p_bytes->data, c_bytes->data, &k, AES_ENCRYPT);
  c_bytes->size += block_size;

  return c_bytes;
}

byte_string* aes_ecb_decrypt(byte_string* c_bytes, byte_string* k_bytes, size_t block_size) {
  byte_string* p_bytes = bytes_init(c_bytes->size);

  AES_KEY k;
  AES_set_decrypt_key(k_bytes->data, 128, &k);
  AES_ecb_encrypt(c_bytes->data, p_bytes->data, &k, AES_DECRYPT);
  p_bytes->size += block_size;

  byte_string* unpadded_p_bytes = pkcs7_unpad_bytes(p_bytes, block_size);
  bytes_free(p_bytes);
  return unpadded_p_bytes;
}

byte_string* aes_cbc_encrypt(byte_string* p_bytes, byte_string* k_bytes, byte_string* iv_bytes, size_t block_size) {
  byte_string* c_bytes = bytes_init(p_bytes);
  byte_string* prev_iv_bytes = iv_bytes;

  for (size_t i = 0; i < p_bytes->size; i += block_size) {
    byte_string* tmp_p_bytes = bytes_from(p_bytes->data + i, block_size);
    byte_string* plaintext_bytes = pkcs7_pad_bytes(tmp_p_bytes, block_size);
    byte_string* xor_bytes = bytes_xor(plaintext_bytes, prev_iv_bytes);
    byte_string* ebc_block_bytes = aes_ecb_encrypt(xor_bytes, k_bytes, block_size);

    for (size_t j = 0; j < block_size; j += 1) bytes_append(c_bytes, ebc_block_bytes->data[j]);

    if (prev_iv_bytes) bytes_free(prev_iv_bytes);
    prev_iv_bytes = ebc_block_bytes;

    bytes_free(tmp_p_bytes);
    bytes_free(plaintext_bytes);
    bytes_free(xor_bytes);
  }

  return c_bytes;
}

byte_string* aes_cbc_decrypt(byte_string* c_bytes, byte_string* k_bytes, byte_string* iv_bytes, size_t block_size) {
  byte_string* p_bytes = bytes_init(c_bytes->size);
  assertm(iv_bytes->size == 16, "~ iv_bytes should be 16 bytes in length");
  byte_string* prev_iv_bytes = bytes_clone(iv_bytes);

  for (size_t i = 0; i < c_bytes->size; i += block_size) {
    byte_string* tmp_c_bytes       = bytes_from(c_bytes->data + i, block_size);
    byte_string* ebc_block_decrypt = aes_ecb_decrypt(tmp_c_bytes, k_bytes, block_size);
    byte_string* xor_bytes         = bytes_xor(prev_iv_bytes, ebc_block_decrypt);

    for (size_t j = 0; j < block_size; j += 1) bytes_append(p_bytes, xor_bytes->data[j]);

    bytes_free(prev_iv_bytes);
    prev_iv_bytes = bytes_clone(tmp_c_bytes);

    bytes_free(xor_bytes);
    bytes_free(tmp_c_bytes);
  }

  byte_string* unpadded_p_bytes = pkcs7_unpad_bytes(p_bytes, block_size);
  bytes_free(p_bytes);
  bytes_free(prev_iv_bytes);
  return unpadded_p_bytes;
}

int main() {

  char* buffer = NULL;
  {
    FILE* file = fopen("res/10.txt", "r");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(size + 1);
    buffer[size] = 0;

    fread(buffer, 1, size, file);
    fclose(file);
  }

  byte_string* c_bytes = base64_to_bytes(buffer);
  byte_string* k_bytes = string_to_bytes("YELLOW SUBMARINE");
  size_t block_size    = 16;

  // set iv to 0 bytes of size block_size
  byte_string* iv_bytes = bytes_init(block_size);
  for (size_t i = 0; i < block_size; i += 1) {
    bytes_append(iv_bytes, (unsigned char)0);
  }

  byte_string* p_bytes = aes_cbc_decrypt(c_bytes, k_bytes, iv_bytes, block_size);
  mem_output(p_bytes);

  free(buffer);
  return 0;
}
