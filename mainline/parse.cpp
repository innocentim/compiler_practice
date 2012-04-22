#include <string>
#include <cstdio>
#include "lex.hpp"

extern Token tokens[];
extern int token_n;

class AST {
};

int parse() {
    Token token;
    for (int i = 0; i < token_n; i++) {
        token = tokens[i];
        printf("%d: ", (int)token);
        if (token == constant_int) {
            printf("%lld", *((long long *)token.data));
        } else if (token == kwd_return) {
            printf("return");
        } else if (token == identifier) {
            printf("%s", ((std::string*)token.data)->c_str());
        }
        printf("\n");
    }
};
