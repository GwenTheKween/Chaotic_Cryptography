#ifndef __LOG_MAP_H__
#define __LOG_MAP_H__

#include "fixed.h"

/* As the README.md says, I'll only need 32 bytes for
   cryptographical security, so I'm "hardcoding" it here
   in a way that can easily be changed later - or as a
   compilation argument.  */
#ifndef BYTE_COUNT
#define BYTE_COUNT 32
#endif /* BYTE_COUNT */

class logistic_map {
private:
    fixed<BYTE_COUNT> state;
    fixed<BYTE_COUNT> alpha;

    int i;
public:
    /* This constructor should mostly be used to test
       the logistic map.  For actual cryptography, this
       class should be initialized with the result of a
       SHA512.  */
    logistic_map(double d, int ii = 8);

    int remove_transient();
    /* get_random will iterate once over the map and return the
       first 8 bits after the given 'i'.  */
    unsigned char get_random();

    void iterate(size_t n = 1);
    void print_state();

private:
    void _iterate(fixed<BYTE_COUNT>& s, size_t n);
};

logistic_map::logistic_map(double d, int ii):
    state(d), alpha(0.9996), i(ii) {
    remove_transient();
}

int logistic_map::remove_transient() {
    unsigned char c[BYTE_COUNT];
    int count = 0;
    for(int i=0; i<BYTE_COUNT; i++) c[i] = (i == (BYTE_COUNT/2-1));
    fixed<BYTE_COUNT> tmp_state(c);
    tmp_state += state;
    while((state - tmp_state) < 0.10 ||
          (state - tmp_state) > 0.90) {
        _iterate(state, 1);
        _iterate(tmp_state, 1);
        count++;
    }
    return count;
}

unsigned char logistic_map::get_random() {
    iterate();
    return state.get_byte_after(i);
}

void logistic_map::iterate(size_t n){
    _iterate(state, n);
}

void logistic_map::_iterate(fixed<BYTE_COUNT>& s, size_t n){
    while(n--){
        s *= s.negate();
        fixed<BYTE_COUNT> f(s);
        s *= alpha;
        f.times_three();
        s += f;
    }
}

void logistic_map::print_state(){
    std::cout << state.to_double() << "\n";
}

#endif /* __LOG_MAP_H__ */
