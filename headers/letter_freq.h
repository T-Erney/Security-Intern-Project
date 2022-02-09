#ifndef LETTER_FREQ_H
#define LETTER_FREQ_H

#include "conversions.h"
#include "xor.h"

// brought to you by our good friend Wikipedia
// #spacesarecitizenstoo
static char letter_freq_order[] = {
  ' ', 'e', 't', 'a', 'o', 'i', 'n', 's', 'h',
  'r', 'd', 'l', 'c', 'u', 'm', 'w', 'f', 'g',
  'y', 'p', 'b', 'v', 'k', 'j', 'x', 'q', 'z'
};

// also brought to you by our good friend Wikipedia
static float letter_freq_value[] = {
  13.000, 12.702, 9.056, 8.167, 7.507, 6.966, 6.749, 6.327, 6.094,
   5.987,  4.253, 4.025, 2.782, 2.758, 2.406, 2.360, 2.228, 2.015,
   1.974,  1.929, 1.492, 0.978, 0.772, 0.153, 0.150, 0.095, 0.074
};

float letter_freq_analysis(byte_string*);

#endif // LETTER_FREQ_H
