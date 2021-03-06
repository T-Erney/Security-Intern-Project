#include "../headers/conversions.h"
#include "../headers/assert.h"
#include <string.h>
#include <stdio.h>

byte_string* bytes_init (size_t size) {
  byte_string* bytes = malloc(sizeof(byte_string));
  assert(bytes != NULL);
  *bytes = (byte_string) {
    .size = 0,
    .capacity = size,
    .data = malloc(sizeof(unsigned char) * size) };
  return bytes;
}

void bytes_append(byte_string* bytes, unsigned char byte) {
  assert(bytes->capacity > 0);

  if (bytes->capacity == bytes->size) {
    bytes->data = realloc(bytes->data, sizeof(unsigned char) * bytes->capacity * 2);
    bytes->capacity *= 2;
  }

  bytes->data[bytes->size] = byte;
  bytes->size += 1;
}

void bytes_free(byte_string* bytes) {
  free(bytes->data);
  free(bytes);
}

void bytes_print(byte_string* bytes) {
  for (size_t i = 0; i < bytes->size; i += 1) {
    printf("%c", bytes->data[i]);
  }
}

byte_string* bytes_clone(byte_string* bytes) {
  byte_string* new_bytes = bytes_init(bytes->size);
  for (size_t i = 0; i < bytes->size; i += 1) bytes_append(new_bytes, bytes->data[i]);
  return new_bytes;
}

byte_string* bytes_from(const char* str, size_t size) {
  byte_string* bytes = bytes_init(size);
  for (size_t i = 0; i < size; i += 1) bytes_append(bytes, str[i]);
  return bytes;
}

int bytes_cmp(byte_string* x_bytes, byte_string* y_bytes) {
  for (size_t i = 0; i < x_bytes->size; i += 1) {
    if (x_bytes->data[i] < y_bytes->data[i]) return -1;
    if (x_bytes->data[i] > y_bytes->data[i]) return 1;
  }
  return 0;
}

void bytes_resize(byte_string* bytes) {
  bytes->data = realloc(bytes->data, bytes->size);
  bytes->capacity = bytes->size;
}

// find the first non-zero byte from the back and resize to that byte
void bytes_truncate(byte_string* bytes) {
  size_t i = bytes->size;
  while (bytes->data[i - 1] == 0) i -= 1;
  bytes->data = realloc(bytes->data, i);
  bytes->size = i;
  bytes->capacity = i;
}

void bytes_prepend(byte_string* bytes, unsigned char byte) {
 assert(bytes->capacity > 0);

  if (bytes->capacity == bytes->size) {
    bytes->data = realloc(bytes->data, sizeof(unsigned char) * bytes->capacity * 2);
    bytes->capacity *= 2;
  }

  for (size_t i = 0; i < bytes->size; i += 1) {
    bytes->data[i + 1] = bytes->data[i];
  }

  bytes->data[0] = byte;
  bytes->size += 1;
}

void bytes_pop_front(byte_string* bytes) {
  if (bytes->size == 0) return;
  for (size_t i = 0; i < bytes->size - 1; i += 1) {
    bytes->data[i] = bytes->data[i + 1];
    // fprintf(stderr, "\t\tbytes->size :: %ld, i :: %ld\n", bytes->size, i);
  }
  bytes->data[bytes->size - 1] = 0;
  bytes->size -= 1;
}

// ---

byte_string* string_to_bytes (char* string) {
  byte_string* bytes = bytes_init(strlen(string));

  for (int i = 0; i < (int)strlen(string); i += 1) {
    bytes_append(bytes, string[i]);
  }

  return bytes;
}

// hex_to_bytes (char* hex) -> byte_string*
// - convert a hex string into an array of byte values
//
// WARN: This code, as is, does not implement any sort of error checking when
//       running the hex string.
byte_string* hex_to_bytes (char* hex) {
  byte_string* bytes = bytes_init(strlen(hex) / 2);

  for (int i = 0; i < (int)strlen(hex); i += 2) {
    uint8_t byte = 0;
    uint8_t x = hex[i],
            y = hex[i + 1];
    if (x >= 'a' && x <= 'f') x = x - 'a' + 10;
    else                      x = x - '0';

    if (y >= 'a' && y <= 'f') y = y - 'a' + 10;
    else                      y = y - '0';

    byte = (x << 4) + y;
    bytes_append(bytes, byte);
  }

  return bytes;
}

// base64_to_bytes (char* base64) -> byte_string*
// - converts a string of base 64 to an array of byte values
//
// WARN: This code, as is, does not do any error checking when 
//       running the base64 string.
byte_string* base64_to_bytes (char* base64) {
  int input_size  = strlen(base64);
  if (input_size % 4 != 0) {
    printf("*** [ERROR] base64 %% 4 != 0 [FOUND] base64 %% 4 == %d ***\n", input_size % 4);
    return NULL;
  }

  int output_size = (input_size / 4) * 3;
  if (base64[input_size - 1] == '=') output_size -= 1;
  if (base64[input_size - 2] == '=') output_size -= 1;

  byte_string* bytes = bytes_init(output_size);

  /* The goal here is to convert 4 chars into 3 bytes
   *
   * "ABCD" => 'A' 'B' 'C' 'D' => 1 2 3 4 => 
   * 00000001 00000010 00000011 00000100 =>
   *   000001   000010   000011   000100 =>
   * 00000100 00100000 11000100 <- this is our byte stream
   *
   */
  for (int i = 0; i < input_size;) {
    uint8_t x = 0,
            y = 0,
            z = 0;
    uint8_t a = base64[i] == '=' ? 0 & i++ : base64_decoding_table[(int)base64[i++]],
            b = base64[i] == '=' ? 0 & i++ : base64_decoding_table[(int)base64[i++]],
            c = base64[i] == '=' ? 0 & i++ : base64_decoding_table[(int)base64[i++]],
            d = base64[i] == '=' ? 0 & i++ : base64_decoding_table[(int)base64[i++]];

    x = (a << 2) | ((0x30 & b) >> 4);
    y = ((0x0f & b) << 4) | ((0x3c & c) >> 2);
    z = ((0x03 & c) << 6) | d;

    bytes_append(bytes, x);
    bytes_append(bytes, y);
    bytes_append(bytes, z);

    while (base64[i] == '\n') i += 1;
  }

  bytes_truncate(bytes);
  return bytes;
}

// ---

char* bytes_to_string(byte_string* bytes) {
  char* string = malloc(sizeof(char) * bytes->size + 1);
  string[bytes->size] = 0;

  for (size_t i = 0; i < bytes->size; i += 1) {
    string[i] = bytes->data[i];
  }

  return string;
}

char* bytes_to_hex(byte_string* bytes) {
  char* hex = malloc(sizeof(char) * ((bytes->size * 2) + 1));
  hex[(bytes->size * 2)] = '\0';
  int   hex_i = 0;

  for (size_t i = 0; i < bytes->size; i += 1) {
    unsigned char byte = bytes->data[i];
    hex[hex_i++] = hex_encoding[byte >> 4];
    hex[hex_i++] = hex_encoding[0x0f & byte];
  }

  return hex;
}


char* bytes_to_base64(byte_string* bytes) {
  int output_size = 4 * ((bytes->size + 2) / 3);
  char* base64 = malloc(sizeof(char) * (output_size + 1));
  base64[output_size] = '\0';

  /* The goal here is to convert 3 bytes into 4 chars
   *
   * 0 1 2 => 00000000 00000001 00000010 =>
   * 000000 000000 000100 000010 =>
   * 'A' 'A' 'E' 'C'
   *
   */
  size_t j = 0;
  for (size_t i = 0; i < bytes->size; i += 3) {
    uint8_t x = i < bytes->size ? bytes->data[i]     : 0,
            y = i < bytes->size ? bytes->data[i + 1] : 0,
            z = i < bytes->size ? bytes->data[i + 2] : 0;
    /*
    a = base64_encoding_table[(0xfc & x) >> 2];
    b = base64_encoding_table[((0x03 & x) << 4) + ((0xf0 & y) >> 4)];
    c = base64_encoding_table[((0x0f & y) << 2) + ((0xc0 & z) >> 6)];
    d = base64_encoding_table[(0x3f & z)];
    */

    switch (bytes->size - i) {
      case 1:
        base64[j++] = base64_encoding_table[(0xfc & x) >> 2];
        base64[j++] = base64_encoding_table[((0x03 & x) << 4)];
        base64[j++] = '=';
        base64[j++] = '=';
        break;
      case 2:
        base64[j++] = base64_encoding_table[(0xfc & x) >> 2];
        base64[j++] = base64_encoding_table[((0x03 & x) << 4) + ((0xf0 & y) >> 4)];
        base64[j++] = base64_encoding_table[((0x0f & y) << 2)];
        base64[j++] = '=';
        break;
      default:
        base64[j++] = base64_encoding_table[(0xfc & x) >> 2];
        base64[j++] = base64_encoding_table[((0x03 & x) << 4) + ((0xf0 & y) >> 4)];
        base64[j++] = base64_encoding_table[((0x0f & y) << 2) + ((0xc0 & z) >> 6)];
        base64[j++] = base64_encoding_table[(0x3f & z)];
    }
  }

  return base64;
}

// ---

char* hex_to_base64(char* hex) {
  byte_string* bytes  = hex_to_bytes(hex);
  char* base64        = bytes_to_base64(bytes);
  bytes_free(bytes);

  return base64;
}

char* base64_to_hex(char* base64) {
  byte_string* bytes  = base64_to_bytes(base64);
  char* hex           = bytes_to_hex(bytes);
  bytes_free(bytes);

  return hex;
}
