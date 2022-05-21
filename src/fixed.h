#ifndef __FIXED_H__
#define __FIXED_H__

#include <iostream>
#include <ostream>
#include <bitset>
#include <vector>

/* function to add 2 streams of bytes, a and b, and store them in a given
   buffer, res.  All buffers must have at least size N.  It is safe to have
   res be equal to either A or B (or both).  */
void add_streams (unsigned char* a, unsigned char* b, unsigned char* res,
                  unsigned int n){
    int x = 0, y = 0;
    for(int i = n-1; i >= 0; i--){
        x += a[i];
        y = b[i];
        x += y;
        res[i] = x % 256;
        x /= 256;
    }
    /* overflows are discarded on purpose. */
}

/* Function to multiply 2 streams of bytes, a and b, and store them in a
   given buffer, res.  A and B must have size N, and C must have size 2*n.
   It is not safe to have res be equal to either A or B.  */
void multiply_streams (unsigned char* a, unsigned char* b, unsigned char* res,
                       unsigned int n){
    /* 1 byte is easy to multiply. Use an integer to deal with overflow.
       Do not recurse here.  */
    if (n == 1){
        unsigned int x = *a;
        x *= *b;
        res[0] = x / 256;
        res[1] = x % 256;
        return;
    }
    /* The first 2 steps can be done directly to RES, as multyplying the
       2 first halfs and the 2 second halfs will never interact with each
       other.  */
    multiply_streams (a, b, res, n/2);
    multiply_streams (a + n/2, b + n/2, res + n, n/2);

    /* Now do the cross multiplication.  */
    unsigned char x[2*n], y[2*n];
    for(size_t i = 0; i<2*n; i++) x[i] = y[i] = 0;
    multiply_streams (a, b + n/2, x+n/2, n/2);
    multiply_streams (a + n/2, b, y+n/2, n/2);
    add_streams(x, y, x, 2*n);

    /* Finally, add the result of the cross multiplication with the
       easy multiplication above.  */
    add_streams(res, x, res, 2*n);
    return;
}

template <unsigned int n>
class fixed{
private:
    unsigned char bytes[n];
public:
    fixed(double d = 0);
    fixed(unsigned char * c);

    fixed<n> operator+= (fixed<n> other);
    fixed<n> operator+ (fixed<n> other)
    {
        fixed<n> f(other);
        return f += *this;
    }
    fixed<n> operator *= (fixed<n> other);
    fixed<n> operator * (fixed<n> other)
    {
        fixed<n> f(this);
        return f *= other;
    }

    /* Used for printing stuff */
    double to_double();
    template<unsigned int s>
    friend std::ostream& operator<<(std::ostream& os, const fixed<s>& f);
};

template <unsigned int n>
fixed<n>::fixed(double d){
    for(size_t i=0; i < n; i++){
        unsigned char c = 0;
        for(int j = 0; j<8; j++){
            c <<= 1;
            d *= 2;
            if(d >= 1){
                c |= 1;
                d -= 1;
            }
        }
        bytes [i] = c;
    }
}

template <unsigned int n>
fixed<n> fixed<n>::operator+=(fixed<n> other) {
    unsigned char full_answer[2*n];
    add_streams(bytes, other.bytes, full_answer, n);
    for (size_t i = 0; i < n; i++) bytes[i] = full_answer[i];
    return *this;
}

template <unsigned int n>
fixed<n> fixed<n>::operator*= (fixed<n> other){
    unsigned char full_precision[n*2];
    multiply_streams(bytes, other.bytes, full_precision, n);
    for(size_t i=0; i<n; i++) bytes[i] = full_precision[i];
    return *this;
}

template <unsigned int n>
double fixed<n>::to_double(){
    double ret = 0;
    for(size_t i = 0; i < n; i++) {
        double d = 0;
        unsigned char c = bytes[i];
        for(int j = 0; j<8; j++){
            if(c & 1){
                d += 1;
            }
            c>>=1;
            d /=2;
        }
        for(size_t j = 0; j<i; j++) d /= 256;
        ret += d;
    }
    return ret;
}

template<unsigned int n>
std::ostream& operator<< (std::ostream& os, const fixed<n>& f){
    for(size_t i=0; i<n; i++){
        std::bitset<8> bits(f.bytes[i]);
        if (i > 0) os << ' ';
        os << bits;
    }
    return os;
}

#endif
