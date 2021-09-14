#include "conversions.h"

unsigned char _xor (unsigned char c1, unsigned char c2) {
  return c1 ^ c2;
}

char* _xor_hex(char* h1, char* h2) {
  byte_string* bytes1 = hex_to_bytes(h1);
  byte_string* bytes2 = hex_to_bytes(h2);
  byte_string* bytes3 = bytes_init();

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
