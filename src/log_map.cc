#include "log_map.h"

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

