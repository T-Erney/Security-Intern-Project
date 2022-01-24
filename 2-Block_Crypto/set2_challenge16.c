#include "../headers/aes.h"
#include "../headers/pkcs7.h"
#include "../headers/conversions.h"
#include "../headers/memory_output.h"
#include "../headers/xor.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

byte_string* gen_rand(size_t size) {
  byte_string* k = bytes_init(size);

  time_t t;
  srand(time(&t) + size);
  for (uint8_t i = 0; i < size; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}


byte_string* replace(char* src) {
    byte_string* bytes = bytes_init(100);
    while (*src != 0) {
        char c = *src;
        src += 1;
        if (c == ';') {
            bytes_append(bytes, '%');
            bytes_append(bytes, '3');
            bytes_append(bytes, 'b');
        }
        else if (c == '=') {
            bytes_append(bytes, '%');
            bytes_append(bytes, '3');
            bytes_append(bytes, 'd');
        }
        else {
            bytes_append(bytes, c);
        }
    }

    bytes_resize(bytes);
    return bytes;
}

byte_string* oracle(byte_string* bytes, byte_string* k_bytes, byte_string* iv_bytes) {
    byte_string* r_bytes = replace(bytes_to_string(bytes));
    byte_string* prepend_bytes = string_to_bytes("comment1=cooking%20MCs;userdata=");
    byte_string* append_bytes  = string_to_bytes(";comment2=%20like%20a%20pound%20of%20bacon");
    byte_string* new_bytes = bytes_init(bytes->size + prepend_bytes->size + append_bytes->size);

    for (size_t i = 0; i < prepend_bytes->size; i += 1) bytes_append(new_bytes, prepend_bytes->data[i]);
    for (size_t i = 0; i < r_bytes->size; i += 1)       bytes_append(new_bytes, r_bytes->data[i]);
    for (size_t i = 0; i < append_bytes->size; i += 1)  bytes_append(new_bytes, append_bytes->data[i]);

    byte_string* c_bytes = aes_cbc_encrypt(new_bytes, k_bytes, iv_bytes, AES_BLOCK_SIZE);

    bytes_free(new_bytes);
    bytes_free(r_bytes);
    bytes_free(prepend_bytes);
    bytes_free(append_bytes);
    return c_bytes;
}

int check_admin(byte_string* c_bytes, byte_string* k_bytes, byte_string* iv_bytes) {
    byte_string* p_bytes = aes_cbc_decrypt(c_bytes, k_bytes, iv_bytes, AES_BLOCK_SIZE);
    #define CMP_STR ";admin=true;"
    char* p_bytes_str = bytes_to_string(p_bytes);
    while (*p_bytes_str != 0) {
        if (strlen(p_bytes_str) < 13) break;
        if (memcmp(p_bytes_str, CMP_STR, strlen(CMP_STR)) == 0) return 1;

        p_bytes_str += 1;
    }
    #undef CMP_STR
    return 0;
}

int main() {

    byte_string* k_bytes = gen_rand(AES_BLOCK_SIZE);
    byte_string* iv_bytes = string_to_bytes("0000000000000000");

    byte_string* injection_bytes = string_to_bytes("AAAAAAAAAAAAAAAAAadminAtrueA");
    byte_string* c_bytes = oracle(injection_bytes, k_bytes, iv_bytes);

    /*
      * Using the first block from our 2 block long injection_bytes we
      * can manipulate the data which we want to be changed from the second block.
      * So for the first block we want to manipulate the 'A' from the
      * ciphertext at the first byte of the block and we xor that with an xor
      * result of 'A' and the character that we wish to replace the second
      * block location with. We then do the same for the next 2 'A's we
      * wish to replace!
      */
    c_bytes->data[32] = _xor(c_bytes->data[32], _xor('A', ';'));
    c_bytes->data[38] = _xor(c_bytes->data[38], _xor('A', '='));
    c_bytes->data[43] = _xor(c_bytes->data[43], _xor('A', ';'));

    int is_admin = check_admin(c_bytes, k_bytes, iv_bytes);
    if (is_admin) {
        printf("';admin=true;' was found!\n");
    } else {
        printf("';admin=true;' was not found!\n");
    }

    mem_output(aes_cbc_decrypt(c_bytes, k_bytes, iv_bytes, AES_BLOCK_SIZE));

    bytes_free(injection_bytes);
    return 0;
}
