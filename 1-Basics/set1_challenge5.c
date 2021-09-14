#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"

int main() {
  char* key = "ICE";
  char* plaintext = 
    "Burning 'em, if you ain't quick and nimble I go crazy when I hear a cymbal";
  char* answer = 
    "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272"
    "a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";

  size_t k_size = strlen(key);
  byte_string* k_bytes = string_to_bytes(key);
  byte_string* p_bytes = string_to_bytes(plaintext);
  byte_string* c_bytes = bytes_init();

  for (size_t i = 0; i < p_bytes->size; i += 1) {
    bytes_append(c_bytes, _xor(p_bytes->data[i], k_bytes->data[i % k_size]));
  }

  char* hex = bytes_to_hex(c_bytes);
  char* plain = bytes_to_string(c_bytes);
  printf("Expected: %s\n", answer);
  printf("Result:   %s\n", hex);

  mem_output(plain);

  return 0;
}
