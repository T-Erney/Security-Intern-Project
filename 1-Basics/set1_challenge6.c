#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "conversions.h"
#include "xor.h"
#include "memory_output.h"
#include "hamming_dist.h"

static char letter_freq_order[] = {
  ' ', 'e', 't', 'a', 'o', 'i', 'n', 's', 'h', 
  'r', 'd', 'l', 'c', 'u', 'm', 'w', 'f', 'g', 
  'y', 'p', 'b', 'v', 'k', 'j', 'x', 'q', 'z'
};

static float letter_freq_value[] = {
  13.000, 12.702, 9.056, 8.167, 7.507, 6.966, 6.749, 6.327, 6.094,
   5.987,  4.253, 4.025, 2.782, 2.758, 2.406, 2.360, 2.228, 2.015,
   1.974,  1.929, 1.492, 0.978, 0.772, 0.153, 0.150, 0.095, 0.074
};


// ---

char* substr(char* str, size_t a, size_t b) {
  char* s = malloc(sizeof(char) * b + 1);
  memcpy(s, str + a, b);
  s[b] = 0;

  return s;
}

// ---



int main() {

  /*
  { // TESTING HAMMING DISTANCE FUNCTION
    char* test1 = "this is a test";
    char* test2 = "wokka wokka!!!";

    int h_dist = hamming_dist(test1, test2);
    printf("[TEST] h_dist = %d\n\n", h_dist);
  }
  */

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
    bytes_free(c_bytes); // free since we don't need anymore

    float smallest_normalized_dist = 0x0000ffff;
    int   smallest_dist = 0;
    int   dists[38] = { 0 };

    for (size_t k_size = 2; k_size <= 40; k_size += 1) {
      char* sec1 = substr(hex, 0, k_size);
      char* sec2 = substr(hex, k_size, k_size);

      int h_dist = hamming_dist(sec1, sec2);
      float normalized_dist = (float)h_dist / (float)k_size;

      /*
      if (normalized_dist < smallest_normalized_dist) {
        smallest_dist = h_dist;
        smallest_normalized_dist = normalized_dist;
      }
      */

      dists[40 - k_size] = h_dist;
      printf("%d ", h_dist);

      free(sec1);
      free(sec2);
    }
    printf("\n");

    // orginize from least to greatest
    int smallest_dists[3] = { 0 };
    for (size_t i = 0; i < 3; i += 1) {
      int s = 999;
      int s_val = 999999;
      for (size_t j = i; j < 38; j += 1) {
        if (s_val >= dists[j]) { s = j; s_val = dists[j]; }
      }

      int tmp = dists[i];
      dists[i] = dists[s];
      dists[s] = tmp;
      smallest_dists[i] = dists[i];
    }

    printf("smallest distances            :: %d, %d, %d\n", smallest_dists[0], smallest_dists[1], smallest_dists[2]);

    /*
    printf("Smallest Normalized Distance  :: %f\n", smallest_normalized_dist);
    printf("Smallest Hamming Distance     :: %d\n", smallest_dist);
    */

    for (int k = 0; k < 3; k += 1) {

      smallest_dist = smallest_dists[k];
    
    size_t chunk_count = strlen(hex) / smallest_dist;
    char** chunks = malloc(sizeof(char*) * chunk_count);
    {
      size_t i = 0;
      for (size_t j = 0; j < chunk_count; j += 1) {
        chunks[j] = malloc(sizeof(char) * smallest_dist);
        memcpy(chunks[j], hex + i, smallest_dist);
        i += smallest_dist;
      }
    }

    char** transposed_chunks = malloc(sizeof(char*) * smallest_dist);
    {
      size_t i = 0;
      for (size_t j = 0; j < smallest_dist; j += 1) {
        transposed_chunks[j] = malloc(sizeof(char*) * chunk_count + 1);
        transposed_chunks[j][chunk_count] = 0;
        for (size_t k = 0; k < chunk_count; k += 1) {
          transposed_chunks[j][k] = chunks[k][j];
        }
      }
    }

    // single xor on all chunks
    char* keys = malloc(sizeof(char) * smallest_dist);
    memset(keys, 0, smallest_dist);
    {
      for (size_t i = 0; i < smallest_dist; i += 1) {
        char* best_text = NULL;
        float best_score = 0.0;
        unsigned char best_key = 0;
        byte_string* bytes = hex_to_bytes(transposed_chunks[i]);

        for (int key = 1; key < 256; key += 1) {
          char* text = NULL;
          text = malloc(sizeof(char) * bytes->size);
          text[bytes->size] = 0;
          for (size_t i = 0; i < bytes->size; i += 1) {
            unsigned char xor = _xor(bytes->data[i], key);
            text[i] = xor;
          }

          float score = 0.0;
          for (size_t i = 0; i < bytes->size; i += 1) {
            int j = 0;
            int other = 1;
            for (; j < 27; j += 1) {
              if (text[i] == letter_freq_order[j]) {
                other = 0;
                break;
              }
            }

            score += (other) ? 0.50 : letter_freq_value[j];
          }

          if (score > best_score) {
            best_score = score;
            if (best_text != NULL) free(best_text);
            best_text = text;
            best_key = key;
          } else {
            free(text);
          }
        }

        keys[i] = best_key;
        bytes_free(bytes);
        free(best_text);
      }
    }

    printf("Keys:\n\t[ ");
    for (size_t i = 0; i < smallest_dist; i += 1) {
      printf("%d ", keys[i]);
    }
    printf("]\n");

    char* output = NULL;
    {
      byte_string* h_bytes = hex_to_bytes(hex);
      byte_string* k_bytes = string_to_bytes(keys);
      byte_string* o_bytes = bytes_init(h_bytes->size);

      for (size_t i = 0; i < h_bytes->size; i += 1) {
        bytes_append(o_bytes, _xor(h_bytes->data[i], k_bytes->data[i % k_bytes->size]));
      }

      output = bytes_to_string(o_bytes);
    }

    printf("\n%s\n", output);

    // cleanup
    for (size_t i = 0; i < chunk_count; i += 1) free(chunks[i]);
    for (size_t i = 0; i < smallest_dist; i += 1) free(transposed_chunks[i]);
    free(chunks);
    free(transposed_chunks);
  }

    free(buffer);
  }


  return 0;
}
