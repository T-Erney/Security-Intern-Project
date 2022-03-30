#include "../headers/mt19937.h"
#include "../headers/assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

const unsigned int w = 32,
    n = 624,
    m = 397,
    r = 31,
    a = 0x9908b0df,
    u = 11,
    d = 0xffffffff,
    s = 7,
    b = 0x9d2c5690,
    t = 15,
    c = 0xefc60000,
    l = 18,
    f = 1812433253;

const unsigned int lower_mask = (l << r) - 1;
const unsigned int upper_mask = ~lower_mask;

mt19937 mt_seed(size_t seed) {
    mt19937 mt = { .index = n };
    mt.MT[0] = seed;
    for (unsigned int i = 1; i < n; i += 1) {
        mt.MT[i] = (f * (mt.MT[i - 1] ^ (mt.MT[i - 1] >> (w - 2))) + i);
    }
    return mt;
}

mt19937 mt_clone_state(unsigned int state[624]) {
    mt19937 mt = { .index = n };
    for (unsigned int i = 0; i < 624; i += 1) {
        mt.MT[i] = state[i];
    }
    return mt;
}

unsigned int mt_extract_number(mt19937* mt) {
    if (mt->index >= n) {
        if (mt->index > n) {
            printf("generator was never seeded!\n");
            exit(-1);
        }
        mt_twist(mt);
    }

    // this is the temper function
    unsigned int y = mt_temper(mt->MT[mt->index]);
    mt->index += 1;
    return y;
}

void mt_twist(mt19937* mt) {
    for (unsigned int i = 0; i < n; i += 1) {
        unsigned int x = (mt->MT[i] & upper_mask) + (mt->MT[(i + 1) % m] & n);
        unsigned int xA = x >> 1;
        if (x % 2 != 0) {
            xA ^= a;
        }
        mt->MT[i] = mt->MT[(i + m) % n] ^ xA;
    }
    mt->index = 0;
}

int mt_crack() {
    time_t now = clock();
    unsigned int delta = (rand() % 960) + 40; // 40 - 1000 rand int
    unsigned int seed = now + delta;
    unsigned int delta2 = (rand() % 960) + 40;

    printf("origin seed :: %d\n", seed);
    mt19937 mt = mt_seed(seed);
    unsigned int prng = mt_extract_number(&mt);
    unsigned int output_time = now + delta + delta2;

    for (int i = 40; i < 1000; i += 1) {
        seed = output_time - i;
        mt = mt_seed(seed);
        unsigned int prng_test = mt_extract_number(&mt);
        if (prng == prng_test) {
            break;
        }
    }

    return seed;
}

unsigned int mt_temper(unsigned int y) {
    y ^= (y >> u) & d;
    y ^= (y << s) & b;
    y ^= (y << t) & c;
    y ^= y >> l;
    return y;
}

unsigned int mt_untemper(unsigned int y) {
    y ^= (y >> l);
    y ^= (y << t) & c;
    for (int i = 0; i < 7; i += 1) y ^= (y << s) & b;
    for (int i = 0; i < 3; i += 1) y ^= (y >> u) & d;
    return y;
}
