#ifndef AES
#define AES

#include "conversions.h"
#include "xor.h"
#include "assert.h"

byte_string* aes_ecb_encrypt(byte_string*, byte_string*, size_t);
byte_string* aes_ecb_decrypt(byte_string*, byte_string*, size_t);
byte_string* aes_cbc_encrypt(byte_string*, byte_string*, byte_string*, size_t);
byte_string* aes_cbc_decrypt(byte_string*, byte_string*, byte_string*, size_t);

#endif
