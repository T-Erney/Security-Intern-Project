#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/pkcs7.h"
#include "../headers/memory_output.h"

// a globally kept key... I know... BLASPHAMY!!!
static byte_string* key = NULL;

byte_string* gen_rand(size_t size) {
  byte_string* k = bytes_init(size);

  srand(time(NULL));
  for (uint8_t i = 0; i < size; i += 1) {
    bytes_append(k, (uint8_t)(rand() % 128));
  }

  return k;
}

byte_string** parse_profile(byte_string* bytes) {
  byte_string** parse_bytes = malloc(sizeof(byte_string*) * 3);
  parse_bytes[0] = bytes_init(1);
  parse_bytes[1] = bytes_init(1);
  parse_bytes[2] = bytes_init(1);
  size_t k = 0;

  for (size_t i = 0; i < bytes->size;) {
    while (bytes->data[i] != '&' && bytes->size > i) {
      bytes_append(parse_bytes[k], bytes->data[i++]);
    }
    k += 1;
    i += 1;
  }

  return parse_bytes;
}

byte_string* profile_for(char* email) {
  byte_string* enc = NULL;
  size_t block_size = 16;

  char buffer[100] = {0};
  sprintf(buffer, "email=%s&uid=10&role=user", email);
  byte_string* bytes = string_to_bytes(buffer);

  // get size and find padding of buffer
  size_t enc_size = bytes->size + (block_size - (bytes->size % block_size));
  enc = bytes_init(enc_size);

  for (size_t i = 0; i < bytes->size; i += 16) {
    byte_string* tmp_bytes = bytes_from((char*)bytes->data + i, block_size);
    byte_string* ecb_block_encrypt = aes_ecb_encrypt(tmp_bytes, key, block_size);
    for (size_t j = 0; j < block_size; j += 1) bytes_append(enc, ecb_block_encrypt->data[j]);
    bytes_free(ecb_block_encrypt);
    // bytes_free(tmp_bytes); // this causes an error, double free?
  }

  bytes_free(bytes);
  return enc;
}

byte_string* get_profile(byte_string* c_bytes) {
  byte_string* p_bytes = bytes_init(c_bytes->size);
  size_t block_size = 16;

  for (size_t i = 0; i < c_bytes->size; i += block_size) {
    byte_string* tmp_bytes = bytes_from((char*)c_bytes->data + i, block_size);
    byte_string* ecb_block_decrypt = aes_ecb_decrypt(tmp_bytes, key, block_size);
    for (size_t j = 0; j < block_size; j += 1) bytes_append(p_bytes, ecb_block_decrypt->data[j]);
    bytes_free(tmp_bytes);
    bytes_free(ecb_block_decrypt);
  }

  bytes_resize(p_bytes);
  return p_bytes;
}

byte_string* create_admin_profile() {
  byte_string* bytes = NULL;
  size_t block_size = 16; // change this so we get a calculation using profile_for function

  byte_string* prebytes = string_to_bytes("email=");
  byte_string* postbytes = string_to_bytes("&uid=10&role=");
  byte_string* rolebytes = string_to_bytes("admin");
  byte_string* emailbytes = NULL;

  {
    // get remaining bytes to block_size w/o the role
    size_t remaining_size = block_size - ((prebytes->size + postbytes->size) % block_size);
    emailbytes = bytes_init(remaining_size + 1);
    for (size_t i = 0; i < remaining_size; i += 1) bytes_append(emailbytes, 'A');
    bytes_append(emailbytes, 0);
  }

  // create a profile for our email
  byte_string* profile = profile_for((char*)emailbytes->data);

  {
    // create a new profile where admin is padded into its own space where email usually goes
    size_t pre  = block_size - prebytes->size;
    size_t post = block_size - rolebytes->size;
    byte_string* fauxemailbytes = bytes_init(block_size);
    for (size_t i = 0; i < pre; i += 1) bytes_append(fauxemailbytes, 'A');
    byte_string* padded_rolebytes = pkcs7_pad_bytes(rolebytes, block_size);
    for (size_t i = 0; i < padded_rolebytes->size; i += 1) bytes_append(fauxemailbytes, padded_rolebytes->data[i]);

    byte_string* alteredprofilebytes = profile_for((char*)fauxemailbytes->data);

    // append second block of alteredprofilebytes to first 2 blocks of profile
    byte_string* new_profile = bytes_init(profile->size);
    for (size_t i = 0; i < block_size * 2; i += 1) bytes_append(new_profile, profile->data[i]);
    for (size_t i = block_size; i < block_size * 2; i += 1) bytes_append(new_profile, alteredprofilebytes->data[i]);


    bytes = pkcs7_unpad_bytes(get_profile(new_profile), block_size);

    bytes_free(fauxemailbytes);
    bytes_free(alteredprofilebytes);
  }

  bytes_free(prebytes);
  bytes_free(postbytes);
  bytes_free(rolebytes);
  bytes_free(emailbytes);
  return bytes;
}

int main() {

  key = gen_rand(16);

  char* profile = "email=terney@email.com&uid=10&role=user";
  byte_string* profile_bytes = string_to_bytes(profile);
  byte_string* enc_profile_bytes = profile_for("terney@email.com");
  byte_string* decrypted_bytes = get_profile(enc_profile_bytes);
  byte_string* admin = create_admin_profile();

  mem_output(admin);


  bytes_free(profile_bytes);
  bytes_free(enc_profile_bytes);
  bytes_free(decrypted_bytes);
  bytes_free(admin);
  return 0;
}
