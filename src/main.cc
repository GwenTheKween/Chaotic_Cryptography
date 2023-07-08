#include "log_map.h"
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <string>
#include <vector>
#include <assert.h>

void usage(const char *prog_name) {
    std::cerr << "Usage: " << prog_name << "\n";

    std::cerr << "\t-h, --help\t\tPrint this help and exit\n";
    std::cerr << "\t-c, --count[=COUNT]\t\tGenerate COUNT random numbers. Defaults to 1000.\n";
    std::cerr << "\t-p, --password[=pwd]\t\tUse the give a password to generate the starting seed.\n";
    std::cerr << "-p and -s are mutually exclusive\n";
}

enum OPTION_INDEXES {
    HELP,
    SEED,
    COUNT,
    PASSWORD,
    PERF,
    TEST,
    OPTION_SIZE
};

/* Function to parse arguments. Return true if there was a problem.  */

bool parse_arguments(int argc, char** argv,
		    std::string *args) {
    struct option longopts[] ={
	{ "help", no_argument, NULL, 'h' },
	{ "test" , no_argument, NULL, 't' },
	{ "count" , required_argument, NULL, 'c' },
	{ "password" , required_argument, NULL, 'p' },
	{ "perf" , no_argument, NULL, 'P' },
	{ 0 }
    };

    /* Set the defaults here. Empty string means no default.  */
    args[HELP] = "";
    args[SEED] = "password";
    args[COUNT] = "1000";
    args[PASSWORD] = "1";
    args[PERF] = "";
    args[TEST] = "";

    while(1) {
	int opt = getopt_long(argc, argv, "htc:p:P", longopts, 0);

	/* We finished getting options.  */
	if(opt == -1) break;

	switch(opt) {
	    case '?':
	    case 'h':
		usage(argv[0]);
		return opt == '?';
	    case 't':
		args[TEST]="1";
		break;
	    case 'c':
		args[COUNT] = optarg;
		break;
	    case 'p':
		args[SEED] = optarg;
		args[PASSWORD] = "1";
		break;
	    case 'P':
		args[PERF] = "1";
		break;
	}
    }
    return false;
}

int main(int argc, char** argv){
    std::string args[OPTION_SIZE];
    int count;

    if(parse_arguments(argc, argv, args)){
	return 1;
    }

    count = atoi(args[COUNT].c_str());
    logistic_map m((const unsigned char*)args[SEED].c_str(),
		    args[PASSWORD]=="");
    if(args[TEST]!="") {
	std::cout<<"#================================================="<<std::endl;
	std::cout<<"#Logistic map; password: "<<args[SEED]<<std::endl;
	std::cout<<"#================================================="<<std::endl;
	std::cout<<" type: d"<<std::endl;
	std::cout<<" count: "<<args[COUNT]<<std::endl;
	std::cout<<" numbit: 8"<<std::endl;
    }
    for(int i = 0; i < count; i++){
	uint8_t c = m.get_random();
	if(args[PERF]=="")
	    std::cout << (int) c << std::endl;
    }
    return 0;
}
