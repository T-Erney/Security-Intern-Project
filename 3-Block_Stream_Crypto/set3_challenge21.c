#include "../headers/mt19937.h"
#include <stdio.h>

int main() {
    mt_seed(1);
    for (int i = 0; i < 10; i += 1) {
        printf("%u\n", mt_extract_number());
    }

    return 0;
}
