#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"

// brought to you by our good friend Wikipedia
// #spacesarecitizenstoo
static char letter_freq_order[] = {
                                   ' ', 'e', 't', 'a', 'o', 'i', 'n', 's', 'h', 
                                   'r', 'd', 'l', 'c', 'u', 'm', 'w', 'f', 'g', 
                                   'y', 'p', 'b', 'v', 'k', 'j', 'x', 'q', 'z'
};

// also brought to you by our good friend Wikipedia
static double letter_freq_value[] = {
                                    13.000, 12.702, 9.056, 8.167, 7.507, 6.966, 6.749, 6.327, 6.094,
                                    5.987,  4.253, 4.025, 2.782, 2.758, 2.406, 2.360, 2.228, 2.015,
                                    1.974,  1.929, 1.492, 0.978, 0.772, 0.153, 0.150, 0.095, 0.074
};

double get_letter_freq(char c) {
  double val = 0.0;
  for (size_t i = 0; i < 27; i += 1) {
    if (c == letter_freq_order[i]) val = letter_freq_value[i];
  }

  return val;
}

double get_score(byte_string* bytes) {
  double score = 0.0;
  for (size_t i = 0; i < bytes->size; i += 1) {
    score += get_letter_freq(bytes->data[i]);
  }

  return score;
}

int main() {

  char* hex = NULL;
  char* key = "YELLOW SUBMARINE";
  {
    FILE* file = fopen("8.txt", "r");

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    hex = malloc(sizeof(char) * size + 1);
    hex[size] = 0;

    fread(hex, 1, size, file);
    fclose(file);
  }

  byte_string* c_bytes = hex_to_bytes(hex);
  byte_string* k_bytes = base64_to_bytes(key);

  // set AES decrypt key
  AES_KEY aes_decrypt_key;
  AES_set_decrypt_key(k_bytes->data, 128, &aes_decrypt_key);

  // create chunks of c_bytes divided by newline
  //   count number of newlines in c_bytes
  size_t lines = 0;
  size_t cols  = 0;
  for (size_t i = 0; i < c_bytes->size; i += 1) {
    if (c_bytes->data[i] == '\n') {
      lines += 1;
    }
  }
  for (size_t i = 0; i < c_bytes->size; i += 1) {
    cols += 1;
    if (c_bytes->data[i] == '\n') break;
  }

  //   create chunks by memcpy the column size of the lines
  byte_string** chunks = malloc(sizeof(byte_string) * lines);
  for (size_t i = 0; i < lines; i += 1) {
    chunks[i] = bytes_init(cols);
    memcpy(chunks[i]->data, c_bytes->data + (cols * i), cols);
    chunks[i]->size = cols;
  }

  // score out each chunk to determine which one was decrypted with ECB
  double best_score = 0;
  size_t best_chunk = 0;
  byte_string* p_bytes = NULL;
  for (size_t i = 0; i < lines; i += 1) {
    byte_string* bytes = bytes_init(chunks[i]->size);

    size_t j = 0;
    while (j < chunks[i]->size) {
      AES_ecb_encrypt(chunks[i]->data + j, bytes->data + j, &aes_decrypt_key, AES_DECRYPT);
      bytes->size += 16;
      j += 16;
    }

    double score = get_score(bytes);
    printf("\t\tscore = %lf\n", score);

    if (score > best_score) {
      if (p_bytes != NULL) bytes_free(p_bytes);
      p_bytes = bytes;
      best_score = score;
      best_chunk = i;
    } else {
      bytes_free(bytes);
    }
  }

  printf("*** best_chunk = %ld *** best_score = %lf ***\n%s\n", best_chunk, best_score, bytes_to_string(p_bytes));

  bytes_free(c_bytes);
  bytes_free(k_bytes);
}
