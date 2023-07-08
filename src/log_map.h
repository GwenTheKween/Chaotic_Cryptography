#ifndef __LOG_MAP_H__
#define __LOG_MAP_H__

#include "fixed.h"

/* As the README.md says, I'll only need 32 bytes for
   cryptographical security, so I'm "hardcoding" it here
   in a way that can easily be changed later - or as a
   compilation argument.  */
#ifndef BYTE_COUNT
#define BYTE_COUNT 256
#endif /* BYTE_COUNT */

class logistic_map {
private:
    fixed<BYTE_COUNT> state;
    fixed<BYTE_COUNT> alpha;

    int i;

    /* The random numbers available in this iteration.  */
    uint8_t numbers[BYTE_COUNT];
    /* how many of those numbers we've used.  */
    unsigned int used_numbers;
    /* How many numbers we can get from each iteration.  */
    unsigned int max_numbers;
public:
    /* This constructor should mostly be used to test
       the logistic map.  For actual cryptography, this
       class should be initialized with the result of a
       SHA512.  */
    logistic_map(double d, int ii = 8);

    /* This is going to be the finally useful constructor,
       but it shouldn't allow NUMBER == true in the final
       version of the crypto.  */
    logistic_map(const unsigned char* c, bool number, int ii = 8);

    int remove_transient();
    /* get_random will iterate once over the map and return the
       first 8 bits after the given 'i'.  */
    uint8_t get_random();

    void iterate(size_t n = 1);
    void print_state();

private:
    void _iterate(fixed<BYTE_COUNT>& s, size_t n);
    void refresh_numbers();
};
#endif /* __LOG_MAP_H__ */
