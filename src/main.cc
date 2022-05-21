#include "fixed.h"

#define BYTE_COUNT 4

#define TEST_SUB(a,b) do{\
    std::cout << a << " - \n" << b << " = \n" << a - b << '\n';\
    std::cout << a.to_double() << " - " << b.to_double() << " = " << (a - b).to_double() << '\n';\
} while(0)

int main(){
    printf("Hello World!\n");
    fixed<BYTE_COUNT> x(0.7);
    fixed<BYTE_COUNT> y(0.3);
    fixed<BYTE_COUNT> z(0.5);

    TEST_SUB(x, y); //x-y
    TEST_SUB(x, z); //x-z
    TEST_SUB(y, x); //y-x
    TEST_SUB(y, z); //y-z
    TEST_SUB(z, x); //z-x
    TEST_SUB(z, y); //z-y
    return 0;
}
