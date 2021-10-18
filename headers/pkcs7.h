#ifndef PKCS7
#define PKCS7

#include "conversions.h"

byte_string* pkcs7_pad_bytes(byte_string*, size_t);
byte_string* pkcs7_unpad_bytes(byte_string*, size_t);

#endif
