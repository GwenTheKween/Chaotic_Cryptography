#ifndef __FIXED_H__
#define __FIXED_H__

#include <iostream>
#include <ostream>
#include <bit>
#include <bitset>
#include <vector>

/* function to add 2 streams of bytes, a and b, and store them in a given
   buffer, res.  All buffers must have at least size N.  It is safe to have
   res be equal to either A or B (or both).  */
static void add_streams (unsigned char* a, unsigned char* b, unsigned char* res,
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
static void multiply_streams (unsigned char* a, unsigned char* b, unsigned char* res,
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

template <unsigned int n> class fixed
{
private:
    unsigned char bytes[n];
    enum {power_of_2 = n && !(n&(n-1))};
    static_assert(power_of_2, "you must have a byte count that is a power of 2");
public:
    fixed(double d = 0);
    fixed(unsigned char * c);

    fixed<n> operator+= (fixed<n> other);
    fixed<n> operator+ (fixed<n> other)
    {
        fixed<n> f(other);
        return f += *this;
    }
    fixed<n> operator -=(fixed<n> other);
    fixed<n> operator - (fixed<n> other)
    {
        fixed<n> f(*this);
        return f-= other;
    }
    fixed<n> operator *= (fixed<n> other);
    fixed<n> operator * (fixed<n> other)
    {
        fixed<n> f(*this);
        return f *= other;
    }
    bool operator!= (fixed<n> other);
    bool operator!= (double d){
        fixed<n> f(d);
        return *this != f;
    }
    bool operator> (fixed<n> other){
        return (other < *this) && (other != *this);
    }
    bool operator> (double d){
        fixed<n> f(d);
        return *this > f;
    }
    bool operator< (fixed<n> other);
    bool operator< (double d){
        fixed<n> f(d);
        return *this < f;
    }

    /* This function pulls double duty.  Not only does it generate the
       two's complement of the number, this is also equivalent of doing
       1 - x, since all numbers are stored mod 1, and 
       (1 - x) mod 1 == -x mod 1.  This method is, unfortunately, O(n),
       but it can not be avoided, as all bytes must be negated.  */
    fixed<n> negate();

    /* This method multiplies THIS by three.  It is used to help with
       the logistic map, as alpha will be 3.999..., which can be broken
       into (3 + 0.999...) turning into fixed * fixed + fixed.times_three() */
    void times_three();

    /* This method is used to check how many bits are different between
       THIS and OTHER, as a percentage between 0 and 1.  */
    double bit_diff(fixed<n> other);

    /* shift left by n bits, then return the next 8 bits.  */
    unsigned char get_byte_after(size_t i);

    /* Used for printing stuff */
    double to_double();
    template<unsigned int s>
    friend std::ostream& operator<<(std::ostream& os, const fixed<s>& f);
};

template <unsigned int n> fixed<n>::fixed(double d)
{
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

template <unsigned int n> fixed<n>::fixed(unsigned char* c)
{
    for(size_t i=0; i<n; i++) bytes[i] = c[i];
}

template <unsigned int n> fixed<n> fixed<n>::operator+=(fixed<n> other)
{
    unsigned char full_answer[2*n];
    add_streams(bytes, other.bytes, full_answer, n);
    for (size_t i = 0; i < n; i++) bytes[i] = full_answer[i];
    return *this;
}

template <unsigned int n> fixed<n> fixed<n>::operator-= (fixed<n> other)
{
    fixed<n> f(other.negate());
    return f + *this;
}

template <unsigned int n> fixed<n> fixed<n>::operator*= (fixed<n> other)
{
    unsigned char full_precision[n*2];
    multiply_streams(bytes, other.bytes, full_precision, n);
    for(size_t i=0; i<n; i++) bytes[i] = full_precision[i];
    return *this;
}

template <unsigned int n> bool fixed<n>::operator !=(fixed<n> other)
{
    for(size_t i = 0; i<n; i++)
        if(bytes[i] != other.bytes[i])
            return true;
    return false;
}

template <unsigned int n> bool fixed<n>::operator <(fixed<n> other)
{
    for(size_t i = 0; i < n; i++){
        if(bytes[i] != other.bytes[i])
            return bytes[i] < other.bytes[i];
    }
    return false;
}

template <unsigned int n> fixed<n> fixed<n>::negate()
{
    unsigned char byt[n], c=1;
    for(size_t i=n; i>0; i --) {
        /* We invert and add 1 to the byte here to turn the byte into its
           two's complement.  */
        byt[i-1] = (~bytes[i-1]) + c;
        /* Then we check if there was an overflow in the current byte.
          
           If the first bit was ON in the original byte, there is no way
           to have an overflow, as we are adding just 1 bit at the end.

           If the first bit was OFF in the original byte, and it is ON in the
           resulting byte, there is no overflow, as the  bit was just
           inverted.

           If the first bit was OFF, and the resulting bit is OFF, an
           overflow has happened.

           Therefore we can use the negated bitwise-or of the first bit
           to determine overflow.  */
        c = !((byt[i-1] | bytes[i-1])>>7);
    }
    /* Generate a new object that we just calculated.  */
    return fixed<n>(byt);
}

template <unsigned int n> void fixed<n>::times_three()
{
    unsigned int x = 0;
    for(int i = n - 1; i >= 0; i--) {
        x += (bytes[i] << 1) + bytes[i];
        bytes[i] = x;
        x >>= 8;
    }
}

template <unsigned int n> double fixed<n>::bit_diff(fixed<n> other)
{
    double count = 0;
    for(size_t i = 0; i<n; i++){
        unsigned char x = bytes[i] ^ other.bytes[i];
        count += std::popcount(x);
    }
    return count / (8*n);
}

template <unsigned int n> unsigned char fixed<n>::get_byte_after(size_t i)
{
    int index = n/8;
    unsigned char c = bytes[index] << (n % 8);
    if(n % 8 == 0) return c;
    c += bytes[index + 1] >> (8 - (n % 8));
    return c;
}

template <unsigned int n> double fixed<n>::to_double()
{
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

template<unsigned int n> std::ostream& operator<<
        (std::ostream& os, const fixed<n>& f)
{
    for(size_t i=0; i<n; i++){
        std::bitset<8> bits(f.bytes[i]);
        if (i > 0) os << ' ';
        os << bits;
    }
    return os;
}

#endif
