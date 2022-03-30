/*
** For this challenge we need to invert the temper function.
 */


#include "../headers/mt19937.h"
#include <stdio.h>
#include <time.h>

int main() {
    for (unsigned int i = 0; i < 10; i += 1) {
        unsigned int rng = mt_temper(i);
        printf("i: %d == untemper: %d\n", i, mt_untemper(rng));
    }

    mt19937 seed = mt_seed(time(NULL));
    unsigned int state[624] = {0};
    for (unsigned int i = 0; i < 624; i += 1) {
        state[i] = mt_untemper(mt_extract_number(&seed));
    }

    mt19937 cloned = mt_clone_state(state);

    for (int i = 0; i < 624; i += 1) {
        unsigned int a = mt_extract_number(&seed),
            b = mt_extract_number(&cloned);
        if (a != b) {
            printf("%d != %d\n", a, b);
            return -1;
        }
    }

    printf("mt19937 seed == mt19937 clone\n");


    return 0;
}
