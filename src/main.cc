#include "fixed.h"

int main(){
    printf("Hello World!\n");
    fixed<2> x(0.7);
    fixed<2> y(0.3);
    fixed<2> z(0.5);
    x *= y;
    std::cout << x.to_double() << std::endl;
    return 0;
}
