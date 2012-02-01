#include "misc.hpp"
#include <cstdio>
#include <cstdlib>

void warn(const char * str){
	fprintf(stderr, "warning: %s\n", str);
};

void error(const char * str){
	fprintf(stderr, "error: %s\n", str);
	exit(1);
};

void print_iden(unsigned int iden){
	while (iden--){
		printf("    ");
	}
};
