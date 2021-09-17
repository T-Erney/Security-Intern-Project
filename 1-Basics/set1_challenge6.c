#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"
#include "hamming_dist.h"

// ---

char* substr(char* str, size_t size) {
  char* s = malloc(sizeof(char) * size + 1);
  s[size] = 0;

  for (size_t i = 0; i < size; i += 1) {
    s[i] = str[i];
  }

  return s;
}

// ---

typedef struct chunk_t {
  char*   start;
  struct chunk_t* next;
} Chunk;

void chunk_append(Chunk* chunk, char* s) {
  if (chunk != NULL) {
    Chunk* ptr = chunk;
    while (ptr->next != NULL) {
      ptr = ptr->next;
    }

    ptr->next = malloc(sizeof(Chunk));
    ptr->next->start = s;
    ptr->next->next = NULL;
  } else {
    chunk = malloc(sizeof(Chunk));
    chunk->start = s;
    chunk->next = NULL;
  }
}

void chunk_free(Chunk* chunk) {
  Chunk* tmp = chunk->next;
  free(chunk);
  chunk_free(tmp);
}

Chunk* create_chunks(char* str, size_t k_size) {
  Chunk* chunks = NULL;

  while (str) {
    char* s = substr(str, k_size);
    chunk_append(chunks, s);
    str += k_size;
  }

  return chunks;
}

int main() {

  { // TESTING HAMMING DISTANCE FUNCTION
    char* test1 = "this is a test";
    char* test2 = "wokka wokka!!!";

    int h_dist = hamming_dist(test1, test2);
    printf("[TEST] h_dist = %d\n\n", h_dist);
  }

  {
    // get the contents of 6.txt
    char* buffer = NULL;
    {
      FILE* file = fopen("6.txt", "r");
      size_t size = 0;
    
      fseek(file, 0, SEEK_END);
      size = ftell(file);
      fseek(file, 0, SEEK_SET);
      
      buffer = malloc(sizeof(char) * size + 1);
      fread(buffer, 1, size, file);
      buffer[size] = 0;

      fclose(file);
    }

    // un-base64 the file
    byte_string* c_bytes = base64_to_bytes(buffer);
    char* hex = bytes_to_hex(c_bytes);

    float smallest_normalized_dist = 0x0fffffff;
    int   smallest_dist = 0;

    printf("%s\n\n", hex);
    
    for (size_t k_size = 2; k_size <= 40; k_size += 1) {
      char* sec1 = substr(hex, k_size);
      char* sec2 = substr(hex + k_size, k_size);

      int h_dist = hamming_dist(sec1, sec2);
      float normalized_dist = (float)h_dist / (float)k_size;
      
      if (normalized_dist < smallest_normalized_dist) {
        smallest_dist = h_dist;
        smallest_normalized_dist = normalized_dist;
      }
    }

    printf("Smallest Normalized Distance  :: %f\n", smallest_normalized_dist);
    printf("Smallest Hamming Distance     :: %d\n", smallest_dist);
    Chunk* chunks = create_chunks(hex, smallest_dist);
    
    

  }


  return 0;
}
