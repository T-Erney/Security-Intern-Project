#ifndef MT19937_H
#define MT19937_H

#include "conversions.h"

void         mt_seed(size_t);
unsigned int mt_extract_number();
void         mt_twist();

#endif // MT19937_H
