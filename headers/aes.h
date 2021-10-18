#ifndef AES
#define AES

#include "conversions.h"
#include "xor.h"
#include "assert.h"

byte_string* aes_ebc_encrypt(byte_string*, byte_string*, size_t);
byte_string* aes_ebc_decrypt(byte_string*, byte_string*, size_t);
byte_string* aes_cbc_encrypt(byte_string*, byte_string*, byte_string*, size_t);
byte_string* aes_cbc_decrypt(byte_string*, byte_string*, byte_string*, size_t);

#endif
