#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/memory_output.h"

byte_string* aes_encrypt_oracle(byte_string* p_bytes, byte_string* k_bytes) {
  char unk_string[] = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
  byte_string* unk_bytes = base64_to_bytes(unk_string);
  mem_output(unk_bytes);


  return unk_bytes;
}


int main() {

  byte_string* tmp = aes_encrypt_oracle(NULL, NULL);

  return 0;
}
