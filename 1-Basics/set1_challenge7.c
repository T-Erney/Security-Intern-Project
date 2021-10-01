#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>

#include "../headers/conversions.h"

int main() {

  char* b64 = NULL;
  char* key = "YELLOW SUBMARINE";
  {
    FILE* file = fopen("7.txt", "r");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    b64 = malloc(sizeof(char) * size + 1);
    b64[size] = 0;

    fread(b64, 1, size, file);
    fclose(file);
  }

  byte_string* c_bytes = base64_to_bytes(b64);
  byte_string* k_bytes = string_to_bytes(key);
  byte_string* p_bytes = bytes_init(c_bytes->size);

  // AES decrypt key
  AES_KEY aes_decrypt_key;
  AES_set_decrypt_key(k_bytes->data, 128, &aes_decrypt_key);

  size_t i = 0;
  while (i < c_bytes->size) {
    AES_ecb_encrypt(c_bytes->data + i, p_bytes->data + i, &aes_decrypt_key, AES_DECRYPT);
    p_bytes->size += 16;
    i += 16;
  }

  printf("%s\n", bytes_to_string(p_bytes));

  bytes_free(c_bytes);
  bytes_free(k_bytes);
  bytes_free(p_bytes);
  return 0;
}
