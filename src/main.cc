#include "log_map.h"
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <string>
#include <vector>

void usage(const char *prog_name) {
    std::cerr << "Usage: " << prog_name << "\n";

    std::cerr << "\t-h, --help\t\tPrint this help and exit\n";
    std::cerr << "\t-s, --seed[=VALUE]\t\tUse VALUE as the starting state for the map.  VALUE must be in (0,1) exclusive.\n";
    std::cerr << "\t-c, --count[=COUNT]\t\tGenerate COUNT random numbers. Defaults to 1000.\n";
}

enum OPTION_INDEXES {
    HELP,
    SEED,
    COUNT,
    OPTION_SIZE
};

/* Function to parse arguments. Return true if there was a problem.  */

bool parse_arguments(int argc, char** argv,
                    std::string *args) {
    struct option longopts[] ={
        { "help", no_argument, NULL, 'h' },
        { "seed", required_argument, NULL, 's' },
        { "count" , required_argument, NULL, 'c' },
        { 0 }
    };

    /* Set the defaults here. Empty string means no default.  */
    args[HELP] = "";
    args[SEED] = "0.4";
    args[COUNT] = "1000";

    while(1) {
        int opt = getopt_long(argc, argv, "hs:c:", longopts, 0);

        /* We finished getting options.  */
        if(opt == -1) break;

        switch(opt) {
            case '?':
            case 'h':
                usage(argv[0]);
                return opt == '?';
            case 's':
                args[SEED] = optarg;
                break;
            case 'c':
                args[COUNT] = optarg;
                break;
        }
    }
    return false;
}

int main(int argc, char** argv){
    double d;
    std::string args[OPTION_SIZE];
    int count;

    if(parse_arguments(argc, argv, args)){
        return 1;
    }

    d = atof(args[SEED].c_str());
    d = d - floor(d);
    count = atoi(args[COUNT].c_str());
    logistic_map m(d);
    for(int i = 0; i < count; i++){
        unsigned char c = m.get_random();
        std::cout << (int) c << '\n';
    }
    return 0;
}
