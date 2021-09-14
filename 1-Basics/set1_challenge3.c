#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "conversions.h"
#include "xor.h"

// brought to you by our good friend Wikipedia
// #spacesarecitizenstoo
static char letter_freq_order[] = {
  ' ', 'e', 't', 'a', 'o', 'i', 'n', 's', 'h', 
  'r', 'd', 'l', 'c', 'u', 'm', 'w', 'f', 'g', 
  'y', 'p', 'b', 'v', 'k', 'j', 'x', 'q', 'z'
};

// also brought to you by our good friend Wikipedia
static float letter_freq_value[] = {
  13.000, 12.702, 9.056, 8.167, 7.507, 6.966, 6.749, 6.327, 6.094,
   5.987,  4.253, 4.025, 2.782, 2.758, 2.406, 2.360, 2.228, 2.015,
   1.974,  1.929, 1.492, 0.978, 0.772, 0.153, 0.150, 0.095, 0.074
};

int main() {
  char* hex = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
  byte_string* bytes = hex_to_bytes(hex);
  
  char*   best_text = NULL;
  float   best_score = 0.0;
  uint8_t best_key = -1;

  for (uint16_t key = 1; key < 256; key += 1) {
    char* text = NULL;
    text = malloc(sizeof(char) * bytes->size + 1);
    text[bytes->size] = 0;
    for (size_t i = 0; i < bytes->size; i += 1) {
      unsigned char xor = _xor(bytes->data[i], (uint8_t)key);
      text[i] = xor;
    }
    
    float score = 0;
    for (size_t i = 0; i < bytes->size; i += 1) {
      int j = 0;
      for (; j < 27; j += 1) {
        if (text[i] == letter_freq_order[j]) break;
      }

      score += letter_freq_value[j];
    }

    if (score > best_score) {
      best_score = score;
      if (best_text != NULL) free(best_text);
      best_text = text;
      best_key = key;
    }

    if (strcmp(best_text, text) != 0) {
      free(text);
    }
  }

  printf("Key %d :: Score %f :: %s\n", best_key, best_score, best_text);

  return 0; 
}
