#include "../headers/pkcs7.h"
#include "../headers/assert.h"
#include "../headers/mod.h"

byte_string* pkcs7_pad_bytes(byte_string* x_bytes, size_t block_size) {
  size_t pad_size = mod((block_size - x_bytes->size), block_size);
  if (x_bytes->size == block_size) return bytes_clone(x_bytes);

  byte_string* bytes = bytes_init(x_bytes->size + pad_size);

  for (size_t i = 0; i < x_bytes->size + pad_size; i += 1) {
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

// validate padding on byte_string
int pkcs7_pad_validator(byte_string* bytes) {
  size_t pad_size = (bytes->data[bytes->size - 1] <= 0xf) ? bytes->data[bytes->size - 1] : 0;
  if (pad_size == 0 || pad_size > 0xf) return 0;
  size_t current_pad = 0;

  for (size_t i = bytes->size - 1; (int64_t)i >= 0; i -= 1) {
    if (bytes->data[i] != pad_size) break;
    current_pad += 1;
  }

  return (pad_size == current_pad) ? 1 : 0;
}
