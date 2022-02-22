// This is a version where the amount of time passed is simulated.
// Maybe we should come back and do a realtime version of this.

#include "../headers/mt19937.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

unsigned int crack_mt19937() {
    time_t now = clock();
    unsigned int delta = (rand() % 960) + 40; // 40 - 1000 rand int
    unsigned int seed = now + delta;
    unsigned int delta2 = (rand() % 960) + 40;

    printf("origin seed :: %d\n", seed);
    mt_seed(seed);
    unsigned int prng = mt_extract_number();
    unsigned int output_time = now + delta + delta2;

    for (int i = 40; i < 1000; i += 1) {
        seed = output_time - i;
        mt_seed(seed);
        unsigned int prng_test = mt_extract_number();
        if (prng == prng_test) {
            break;
        }
    }

    return seed;
}

int main() {

    unsigned int seed = crack_mt19937();
    printf("found seed :: %d\n", seed);

    return 0;
}
