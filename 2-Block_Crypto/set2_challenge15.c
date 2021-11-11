#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../headers/conversions.h"
#include "../headers/pkcs7.h"
#include "../headers/memory_output.h"



int main() {

  // byte_string* bytes = string_to_bytes("Yolo Swaggins");
  // byte_string* pad_bytes = pkcs7_pad_bytes(bytes, 16);

  byte_string* pad_bytes = string_to_bytes("Yolo Swaggins\001\002\003");

  byte_string* validated_bytes = pkcs7_pad_validator(pad_bytes);

  if (validated_bytes != NULL) {
    mem_output(validated_bytes);
  } else {
    printf("error: the byte string is not valid.\n");
  }

  return 0;
}
