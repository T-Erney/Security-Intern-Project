#include "../headers/conversions.h"
#include "../headers/xor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  {
    char hex[]    = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    char base64[] = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    char* result = hex_to_base64(hex);
    printf("expected: %s\nresult  : %s\n", base64, result);
 
    if (strcmp(base64, result) == 0) {
      printf("MATCHING\n");
    } else {
      printf("NOT MATCHING\n");
    } 
  }

  {
    char* hex1 = "1c0111001f010100061a024b53535009181c";
    char* hex2 = "686974207468652062756c6c277320657965";
    char* hex3 = "746865206b696420646f6e277420706c6179";

    char* res  = _xor_hex(hex1, hex2);
    printf("expected: %s\nresult  : %s\n", hex3, res);
    if (strcmp(hex3, res) == 0) {
      printf("_xor(hex1, hex2) := true\n");
    } else {
      printf("_xor(hex1, hex2) := false\n");
    }
  }

  

  return 0;
}
