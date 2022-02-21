#include "../headers/mt19937.h"
#include "../headers/assert.h"
#include <stdio.h>

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

unsigned int MT[624];
unsigned int _index = 0;

void mt_seed(size_t seed) {
    _index = n;
    MT[0] = seed;
    for (unsigned int i = 1; i < n; i += 1) {
        MT[i] = (f * (MT[i - 1] ^ (MT[i - 1] >> (w - 2))) + i);
    }
}

unsigned int mt_extract_number() {
    if (_index >= n) {
        if (_index > n) {
            printf("generator was never seeded!\n");
            exit(-1);
        }
        mt_twist();
    }

    unsigned int y = MT[_index];
    y ^= (y >> u) & d;
    y ^= (y << s) & b;
    y ^= (y << t) & c;
    y ^= y >> l;

    _index += 1;
    return y;
}

void mt_twist() {
    for (unsigned int i = 0; i < n; i += 1) {
        unsigned int x = (MT[i] & upper_mask) + (MT[(i + 1) % m] & n);
        unsigned int xA = x >> 1;
        if (x % 2 != 0) {
            xA ^= a;
        }
        MT[i] = MT[(i + m) % n] ^ xA;
    }
    _index = 0;
}
