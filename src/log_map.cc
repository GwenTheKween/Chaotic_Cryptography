#include "log_map.h"

#include <cstring>
#include <openssl/sha.h>

/* Function to calculate the SHA512 of the given password, or from the current
   time, which should only be used to speed up testing, not for actual
   encrypting.  Not that this library should be used to encrypt anything, but
   still.
   
   Resulting hash will be stored in PWD, which must have 512 bytes.  */

static void generate_start(unsigned char* pwd, size_t len){
    if(len == 0) {
	/* Generate a random seed and print it out.  */
	/* TBA DO NOT USE THIS YET.  */
	memset(pwd,0x00, 512);
	len = 511;
    }
    SHA512_CTX ctx;

    SHA512_Init(&ctx);
    SHA512_Update(&ctx, pwd, len);
    SHA512_Final(pwd, &ctx);

    for(size_t i=0; i<len; i++) std::cout<<(int)pwd[i]<<' ';
    std::cout<<'\n';
}

logistic_map::logistic_map(double d, int ii):
    state(d), alpha(0.9996), i(ii) {
    remove_transient();
}

logistic_map::logistic_map(const unsigned char* c, bool number, int ii):
    alpha(0.9996), i(ii) {
    if(number)
    {
	double d;
	d = atof((const char*)c);
	state.set(d);
    } else
	state.set(c);
    max_numbers = BYTE_COUNT - (i/8 + (i%8 != 0));
    used_numbers = max_numbers;
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

void logistic_map::refresh_numbers () {
    used_numbers = 0;

    iterate();
    int shift_bits = i;
    int index = 0;
    while (shift_bits/8 < BYTE_COUNT - (shift_bits % 8 != 0)) {
	numbers[index] = state.get_byte_after (shift_bits);
	index ++;
	shift_bits += 8;
    }
}

uint8_t logistic_map::get_random() {
    if (used_numbers >= max_numbers)
	refresh_numbers ();
    return numbers[used_numbers++];
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

