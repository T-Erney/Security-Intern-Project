#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mod.h"

char* pkcs7_pad(char* msg, size_t block_size) {
  size_t msg_size = strlen(msg);
  size_t pad_size = mod((block_size - msg_size), block_size);
  if (pad_size == 0) pad_size = block_size;

  char* str = malloc(sizeof(char) * (msg_size + pad_size) + 1);
  str[msg_size + pad_size] = 0;

  memcpy(str, msg, strlen(msg));

  for (int i = msg_size; i < block_size; i += 1) {
    str[i] = (char)pad_size;
  }

  return str;
}

int main() {
  size_t block_size = 20;
  char* msg = "YELLOW SUBMARINE1234";
  char* padded_msg = pkcs7_pad(msg, block_size);

  printf("msg:        %s\n", msg);
  printf("padded_msg: ");
  for (size_t i = 0; i < block_size; i += 1) {
    if (padded_msg[i] < ' ') {
      printf("\\x%x", padded_msg[i]);
    } else {
      printf("%c", padded_msg[i]);
    }
  }
  printf("\n");

  return 0;
}
