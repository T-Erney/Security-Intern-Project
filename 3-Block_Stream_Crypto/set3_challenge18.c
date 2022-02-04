#include "../headers/conversions.h"
#include "../headers/aes.h"
#include "../headers/xor.h"
#include "../headers/memory_output.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    char* input = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";
    byte_string* c_bytes = base64_to_bytes(input);
    mem_output(c_bytes);

    byte_string* k_bytes = string_to_bytes("YELLOW SUBMARINE");
    size_t nonce = 0;
    byte_string* p_bytes = aes_ctr_mode(c_bytes, k_bytes, nonce);
    mem_output(p_bytes);

    return 0;
}
