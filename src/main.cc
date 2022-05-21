#include "log_map.h"

void test(double d){
    logistic_map m(d);
    int count = m.remove_transient();
    std::cout << count << '\n';
}

int main(){
    for (double d = 0.01; d < 1; d+=0.01)
        test(d);
    return 0;
}
