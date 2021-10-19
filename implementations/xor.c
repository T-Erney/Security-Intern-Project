#include "../headers/conversions.h"
#include "../headers/assert.h"

unsigned char _xor (unsigned char c1, unsigned char c2) {
  return c1 ^ c2;
}

char* _xor_hex(char* h1, char* h2) {
  byte_string* bytes1 = hex_to_bytes(h1);
  byte_string* bytes2 = hex_to_bytes(h2);
  byte_string* bytes3 = bytes_init(bytes1->size);

  if (bytes1->size != bytes2->size) return NULL;

  for (size_t i = 0; i < bytes1->size; i += 1) {
    bytes_append(bytes3, _xor(bytes1->data[i], bytes2->data[i]));
  }

  char* hex = bytes_to_hex(bytes3);

  bytes_free(bytes1);
  bytes_free(bytes2);
  bytes_free(bytes3);
  return hex;
}

char* _xor_repeated(char* plaintext, char* key) {
  byte_string* k_bytes = string_to_bytes(key);
  byte_string* p_bytes = string_to_bytes(plaintext);
  byte_string* c_bytes = bytes_init(p_bytes->size);

  for (size_t i = 0; i < p_bytes->size; i += 1) {
    bytes_append(c_bytes, _xor(p_bytes->data[i], k_bytes->data[i % k_bytes->size]));
  }

  char* hex = bytes_to_hex(c_bytes);
  bytes_free(k_bytes);
  bytes_free(p_bytes);
  bytes_free(c_bytes);
  return hex;
}

byte_string* bytes_xor(byte_string* x_bytes, byte_string* y_bytes) {
  assert(x_bytes->size == y_bytes->size);
  byte_string* bytes = bytes_init(x_bytes->size);

  for (size_t i = 0; i < x_bytes->size; i += 1) {
    bytes_append(bytes, _xor(x_bytes->data[i], y_bytes->data[i]));
  }

  return bytes;
}
