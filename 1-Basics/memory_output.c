#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/memory_output.h"

/* | 00 00 00 00 00 00 00 00 | | ........ |
 * | 01 02 03 04 05 06 07 08 | | ........ | 
 * ...
 * ...
 * | 09 0a 0b 0c 0d 0e 0f 10 | | ........ |
 */
void mem_output(byte_string* bytes) {

  printf("+-------------------------+ +----------+\n");
  for (size_t i = 0; i < bytes->size;) {
    char line[8] = {32, 32, 32, 32, 32, 32, 32, 32};
    printf("| ");
    for (size_t j = 0; j < 8; j += 1) {
      if (i < bytes->size) {
        line[i % 8] = bytes->data[i];
        printf("%2x ", bytes->data[i]);
      } else if (i >= bytes->size) {
        line[i % 8] = 0;
        printf("%2x ", 0);
      }
      i += 1;
    }

    printf("| | ");
    for (size_t j = 0; j < 8; j += 1) {
      if (line[j] > 31 && line[j] < 127) {
        printf("%c", line[j]);
      } else {
        printf(".");
      }
    }
    printf(" |\n");
  }
  printf("+-------------------------+ +----------+\n\n");
}


