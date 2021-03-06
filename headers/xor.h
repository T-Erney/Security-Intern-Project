#ifndef XOR_H
#define XOR_H

#include "conversions.h"

unsigned char _xor (unsigned char, unsigned char);
char* _xor_hex(char*, char*);
char* _xor_repeated(char*, char*);

byte_string* _xor_repeated_bytes(byte_string*, byte_string*);
byte_string* single_xor(byte_string*, unsigned char);
byte_string* bytes_xor(byte_string*, byte_string*);

#endif
