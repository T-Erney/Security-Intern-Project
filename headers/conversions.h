#ifndef HEX_TO_BASE64_H
#define HEX_TO_BASE64_H

#include <stdint.h>
#include <stdlib.h>

typedef struct byte_string_t {
  size_t          size, capacity;
  unsigned char*  data;
} byte_string;

byte_string*  bytes_init();
void          bytes_append(byte_string*, unsigned char);
void          bytes_free(byte_string*);
void          bytes_print(byte_string*);
byte_string*  bytes_clone(byte_string*);
byte_string*  bytes_from(char*, size_t);

static char base64_encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int8_t base64_decoding_table[] = {
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 62, -1, -1, -1, 63,
  52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, -1, -1, -1, -1, -1, -1,
  -1,  0,  1,  2,  3,  4,  5,  6,
   7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, -1, -1, -1, -1, -1,
  -1, 26, 27, 28, 29, 30, 31, 32,
  33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 51, -1, -1, -1, -1, -1

};
// static char base64_padding = '=';
static char hex_encoding[] = "0123456789abcdef";

// conversion to bytes
byte_string* string_to_bytes (char*);
byte_string* hex_to_bytes (char*);
byte_string* base64_to_bytes (char*);

char* bytes_to_string (byte_string*);
char* bytes_to_hex (byte_string*);
char* bytes_to_base64 (byte_string*);

char* hex_to_base64 (char*);
char* base64_to_hex (char*);

#endif
