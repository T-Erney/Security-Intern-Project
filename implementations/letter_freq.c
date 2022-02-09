#include "../headers/letter_freq.h"

float letter_freq_analysis(byte_string* bytes) {
    float score = 0.0;

    for (size_t i = 0; i < bytes->size; i += 1) {
        int j = 0;
        for (; j < 27; j += 1) {
            if (bytes->data[i] == letter_freq_order[j]) break;
        }

        score += (j < 27) ? letter_freq_value[j] : 0;
    }

    return score;
}
