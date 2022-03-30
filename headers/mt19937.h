#ifndef MT19937_H
#define MT19937_H

#include "conversions.h"

typedef struct mt19937_t {
    unsigned int MT[624];
    unsigned int index;
} mt19937;

mt19937      mt_seed(size_t);
mt19937      mt_clone_state(unsigned int[624]);
unsigned int mt_extract_number(mt19937*);
void         mt_twist(mt19937*);

int          mt_crack();
unsigned int mt_temper(unsigned int);
unsigned int mt_untemper(unsigned int);

#endif // MT19937_H
