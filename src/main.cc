#include "log_map.h"
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv){
    double d;
    if(argc > 1){
        d = atof (argv[1]);
    }else{
        d = 0.4;
    }
    d = d - floor(d);
    logistic_map m(d);
    for(int i = 0; i < 100000; i++){
        unsigned char c = m.get_random();
        std::cout << (int) c << '\n';
    }
    return 0;
}
