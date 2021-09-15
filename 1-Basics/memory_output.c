#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* | 00 00 00 00 00 00 00 00 | | ........ |
 * | 01 02 03 04 05 06 07 08 | | ........ | 
 * ...
 * ...
 * | 09 0a 0b 0c 0d 0e 0f 10 | | ........ |
 */
void mem_output(char* mem) {
  size_t size = strlen(mem);

  for (size_t i = 0; i < size;) {
    char line[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    printf("| ");
    for (size_t j = 0; j < 8; j += 1) {
      line[i % 8] = mem[i];
      printf("%2x ", mem[i]);
      i += 1;
    }

    printf("| | ");
    for (size_t j = 0; j < 8; j += 1) {
      if (line[j] <= 31 && line[j] >= 127) {
        printf(".");
      } else {
        printf("%c", line[j]);
      }
    }
    printf("|\n");
  }
}


