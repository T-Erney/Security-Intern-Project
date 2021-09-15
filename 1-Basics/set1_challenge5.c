#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"

int main() {
  char* key = "ICE";
  char* plaintext = 
    "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
  char* answer = 
    "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";

  char* hex = _xor_repeated(plaintext, key);
  printf("Expected: %s\n", answer);
  printf("Result:   %s\n", hex);

  mem_output(answer);
  mem_output(hex);

  return 0;
}
