#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../headers/conversions.h"
#include "../headers/pkcs7.h"
#include "../headers/memory_output.h"



int main() {

  // byte_string* bytes = string_to_bytes("Yolo Swaggins");
  // byte_string* pad_bytes = pkcs7_pad_bytes(bytes, 16);

  byte_string* pad_bytes = string_to_bytes("Yolo Swaggins\003\003\003");

  if (pkcs7_pad_validator(pad_bytes)) {
    mem_output(pad_bytes);
  } else {
    printf("error: the byte string is not valid.\n");
  }

  return 0;
}
