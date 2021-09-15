#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conversions.h"

/* | 00 00 00 00 00 00 00 00 | | ........ |
 * | 01 02 03 04 05 06 07 08 | | ........ | 
 * ...
 * ...
 * | 09 0a 0b 0c 0d 0e 0f 10 | | ........ |
 */
void mem_output(char* mem) {
  byte_string* bytes = string_to_bytes(mem);

  printf("+-------------------------+ +----------+\n");
  for (size_t i = 0; i < bytes->size;) {
    char line[8] = {32, 32, 32, 32, 32, 32, 32, 32};
    printf("| ");
    for (size_t j = 0; j < 8; j += 1) {
      line[i % 8] = bytes->data[i];
      printf("%2x ", bytes->data[i]);
      i += 1;
    }

    printf("| | ");
    for (size_t j = 0; j < 8; j += 1) {
      if (line[j] == 0) break;
      if (line[j] <= 31 && line[j] >= 127) {
        printf(".");
      } else {
        printf("%c", line[j]);
      }
    }
    printf(" |\n");
  }
  printf("+-------------------------+ +----------+\n\n");
}


