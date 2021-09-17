#include "hamming_dist.h"
#include "conversions.h"

int bit_masks[] = {
  0x80,
  0x40,
  0x20, 
  0x10,
  0x08, 
  0x04,
  0x02,
  0x01
};

int hamming_dist(char* s1, char* s2) {
  byte_string* s1_bytes = string_to_bytes(s1);
  byte_string* s2_bytes = string_to_bytes(s2);

  if (s1_bytes->size != s2_bytes->size) return -1;

  int h_dist = 0;
  
  for (size_t i = 0; i < s1_bytes->size; i += 1) {
    unsigned char byte1 = s1_bytes->data[i],
                  byte2 = s2_bytes->data[i];
    unsigned char byte3 = byte1 ^ byte2;
    for (size_t j = 0; j < 8; j += 1) {
      if (bit_masks[j] & byte3) {
        h_dist += 1;
      }
    }
  }

  return h_dist;
}