#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"
#include "hamming_dist.h"

typedef struct chunk_t {
  char*   start;
  char*   end;
  struct chunk_t* next;
} Chunk;

void chunk_append(Chunk* chunk, char* s, char* e) {
  if (chunk != NULL) {
    Chunk* ptr = chunk;
    while (ptr->next != NULL) {
      ptr = ptr->next;
    }

    ptr->next = malloc(sizeof(Chunk));
    ptr->next->start = s;
    ptr->next->end = e;
    ptr->next->next = NULL;
  } else {
    chunk = malloc(sizeof(Chunk));
    chunk->start = s;
    chunk->end = e;
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
    char* s = str;
    char* e = (str + k_size);

    chunk_append(chunks, s, e);

    str += k_size;
  }

  return chunks;
}

int main() {

  { // TESTING HAMMING DISTANCE FUNCTION
    char* test1 = "this is a test";
    char* test2 = "wokka wokka!!!";

    int h_dist = hamming_dist(test1, test2);
    printf("h_dist = %d\n", h_dist);
  }

  {

    for (size_t k_size = 2; k_size <= 40; i += 1) {
      Chunks* chunks = create_chunks(str, k_size);
    
      

    }

  }


  return 0;
}
