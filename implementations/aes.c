#include "../headers/aes.h"
#include "../headers/pkcs7.h"

#include <stdio.h>
#include <openssl/aes.h>

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
  size_t c_size = (p_bytes->size % block_size) == 0 ? p_bytes->size : p_bytes->size + (p_bytes->size - (p_bytes->size % block_size));
  byte_string* c_bytes = bytes_init(c_size);
  byte_string* prev_iv_bytes = bytes_clone(iv_bytes);

  for (size_t i = 0; i < p_bytes->size; i += block_size) {
    byte_string* tmp_p_bytes     = bytes_from(p_bytes->data + i, block_size);
    byte_string* plaintext_bytes = pkcs7_pad_bytes(tmp_p_bytes, block_size);
    byte_string* xor_bytes       = bytes_xor(plaintext_bytes, prev_iv_bytes);
    byte_string* ebc_block_bytes = aes_ecb_encrypt(xor_bytes, k_bytes, block_size);

    for (size_t j = 0; j < block_size; j += 1) bytes_append(c_bytes, ebc_block_bytes->data[j]);

    if (prev_iv_bytes) bytes_free(prev_iv_bytes);
    prev_iv_bytes = bytes_clone(ebc_block_bytes);
    bytes_free(tmp_p_bytes);
    bytes_free(xor_bytes);
    bytes_free(ebc_block_bytes);
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
