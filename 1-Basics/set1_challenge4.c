#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "../headers/conversions.h"
#include "../headers/xor.h"

// strcmp from string.h keeps segfaulting. 
int strcmp(const char* c1, const char* c2) {
  if (strlen(c1) != strlen(c2)) return 0;
  for (size_t i = 0; i < strlen(c1); i += 1) {
    if (c1[i] != c2[i]) return -1;
  }
  return 0;
}

typedef struct list_t {
  char* data;
  struct list_t* next;
} List;

void list_append(List** l, char* d) {
  if (*l != NULL) {
    List* ptr = *l;
    while (ptr->next != NULL) {
      ptr = ptr->next;
    }

    ptr->next = malloc(sizeof(List));
    ptr = ptr->next;
    ptr->data = d;
    ptr->next = NULL;
  } else if (*l == NULL) {
    *l = malloc(sizeof(List));
    (*l)->data = d;
    (*l)->next = NULL;
  }
}

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
  // setup
  FILE* file = fopen("4.txt", "r");
  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* hex = malloc(sizeof(char) * file_size + 1);
  fread(hex, 1, file_size, file);
  hex[file_size] = 0;

  // read string line-by-line
  List* hexs = NULL;
  {
    char* ptr = hex;
    while (ptr) {
      char* line = strchr(ptr, '\n');
      if (line) *line = '\0';
      list_append(&hexs, ptr);
      if (line) *line = '\n';
      ptr = (line) ? line + 1 : NULL;
    }
  }

  char* overall_best_text = NULL;
  float overall_best_score = 0.0;

  List* ptr = hexs;
  while (ptr != NULL) {
    byte_string* bytes = hex_to_bytes(ptr->data);
  
    char*   best_text = NULL;
    float   best_score = 0.0;

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
      } else {
        free(text);
      }
      
    }

    bytes_free(bytes);
    // printf("Key %d :: Score %f :: %s\n", best_key, best_score, best_text);
    ptr = ptr->next;

    if (best_score > overall_best_score) {
      overall_best_score = best_score;
      if (overall_best_text != NULL) free(overall_best_text);
      overall_best_text = best_text;
    }
  }

  printf("Best Text :: %s\nBest Score :: %f\n", overall_best_text, overall_best_score);
  
  return 0; 
}
