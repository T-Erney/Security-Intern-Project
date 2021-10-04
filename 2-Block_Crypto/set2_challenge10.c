#include <stdio.h>
#include <stdlib.h>
#include <openssl/aes>

#include "../headers/conversions.h"
#include "../headers/xor.h"
#include "../headers/mod.h"

// pad bytes
byte_string* pkcs7_pad_bytes(byte_string* bytes, size_t block_size) {
  size_t pad_size = mod((block_size - m_bytes->size), block_size);
  if (m_bytes->size == block_size) return bytes;

  byte_string* bytes = bytes_init(m_bytes->size + pad_size);

  for (int i = 0; i < m_bytes->size + pad_size + 1; i += 1) {
    bytes_append(bytes, (i < m_bytes->size) ? m_bytes->data[i]: (unsigned char)pad_size);
  }

  return bytes;
}

// unpad bytes
byte_string* pkcs7_unpad_bytes(byte_string* bytes) {
  // read last element and remove as many bytes as the value of that element
  size_t unpad = bytes->data[bytes->size - 1];
  size_t new_size = bytes->size - unpad;

  byte_string* new_bytes = bytes_init(new_size);

  for (size_t i = 0; i < new_size; i += 1) {
    bytes_append(new_bytes, bytes->data[i]);
  }

  return new_bytes;
}

byte_string* aes_ebc_encrypt(byte_string* p_bytes, byte_string* k_bytes, size_t block_size) {
  byte_string* c_bytes = bytes_init(p_bytes->size);
  byte_string* padded_p_bytes = pkcs7_pad_bytes(p_bytes, block_size);

  AES_KEY k;
  AES_set_encrypt_key(k_bytes->data, block_size * 8, %k);
  AES_ecb_encrypt(padded_p_bytes->data, c_bytes->data, &k, AES_ENCRYPT);
  c_bytes->size += block_size * 8;

  bytes_free(padded_p_bytes);
  return c_bytes;
}

byte_string* aes_ebc_decrypt(byte_string* c_bytes, byte_string* k_bytes, size_t block_size) {
  byte_string* p_bytes = bytes_init(c_bytes->size);
}

int main() {

  char* buffer = NULL;
  {
    FILE* file = fopen("res/10.txt");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(size + 1);
    buffer[size] = 0;

    fread(buffer, 1, size, file);
    fclose(file);
  }

  byte_string* p_bytes = base64_to_bytes(buffer);
  byte_string* k_bytes = string_to_bytes("YELLOW SUBMARINE");
  size_t block_size    = k_bytes->size;

  // set iv to 0 bytes of size block_size
  byte_string* iv_bytes = bytes_init(block_size);
  for (size_t i = 0; i < block_size; i += 1) {
    bytes_append(iv_bytes, (unsigned char)0);
  }

  

  free(buffer);
  return 0;
}
