#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../headers/conversions.h"
#include "../headers/mod.h"

byte_string* pkcs7_pad(byte_string* m_bytes, size_t block_size) {
  size_t pad_size = mod((block_size - m_bytes->size), block_size);
  if (m_bytes->size == block_size) pad_size = block_size;

  byte_string* bytes = bytes_init(m_bytes->size + pad_size);

  for (int i = 0; i < m_bytes->size + pad_size + 1; i += 1) {
    bytes_append(bytes, (i < m_bytes->size) ? m_bytes->data[i]: (unsigned char)pad_size);
  }

  return bytes;
}

int main() {
  size_t block_size = 20;
  char* msg = "YELLOW SUBMARINE1234";
  byte_string* m_bytes = string_to_bytes(msg);
  byte_string* p_bytes = pkcs7_pad(m_bytes, block_size);
  char* padded_msg = bytes_to_string(p_bytes);

  printf("msg:        %s\n", msg);
  printf("padded_msg: ");
  for (size_t i = 0; i < p_bytes->size; i += 1) {
    if (i > m_bytes->size) {
      printf("\\x%2x", padded_msg[i]);
    } else {
      printf("%c", padded_msg[i]);
    }
  }
  printf("\n");

  bytes_free(m_bytes);
  bytes_free(p_bytes);
  free(padded_msg);

  return 0;
}
