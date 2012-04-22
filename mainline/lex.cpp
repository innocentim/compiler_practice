#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "lex.hpp"

class Trie {
    Token data;
    Trie * children[256]; 
    void _print() {
        printf("%d [label=\"%d\"]\n", this, data);
        for (int i = 0; i < 256; i++) {
            if (children[i]) {
                printf("%d -> %d\n [label=\"%c\"]", this, children[i], i);
                children[i]->_print();
            }
        }
    };
public:
    Trie() {
        data = -1;
        memset(children, 0, 256 * sizeof(Trie*));
    };

    void insert(const char * str, Token token) {
        Trie * temp = this;
        int len = strlen(str);
        for (int i = 0; i < len; i++) {
            char ch = str[i];
            if (temp->children[(unsigned char)ch] == NULL) {
                temp->children[(unsigned char)ch] = new Trie();
            }
            temp = temp->children[(unsigned char)ch];
        }
        temp->data = token;
    };

    Token getData() {
        return data;
    };

    void print() {
        printf("digraph a {\n");
        _print();
        printf("}\n");
    };

    Trie * next(char last) {
        return children[last];
    };

    Token get_data() {
        return data;
    };
};

static Trie punc_map;
Token tokens[1024];
int token_n = 0;

inline
bool isalpha(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
};

inline
bool isdigit(char ch) {
    return '0' <= ch && ch <= '9';
};

Token lex() {
    static char last = ' ';
    while (last == '\n' || last == '\r' || last == ' ' || last == '\t') {
        last = getchar();
    }
    if (isalpha(last) || last == '_') { // identifier := [_a-zA-Z][_a-zA-Z0-9]*
        std::string & iden_str = *(new std::string);
        iden_str += last;
        while (isalpha(last = getchar()) || last == '_' || isdigit(last)) {
            iden_str += last;
        }
        if (iden_str == "return") {
            delete &iden_str;
            return Token(kwd_return);
        }
        return Token(identifier, &iden_str);
    } else if (isdigit(last)) { // constant_number := [0-9]+
        long long & num_int = *(new long long);
        num_int = last - '0';
        while (isdigit(last = getchar())) {
            num_int = num_int * 10 + last - '0';
        }
        return Token(constant_int, &num_int);
    } else if (last == EOF) {
        return Token(eof);
    }
    Trie * temp = &punc_map;
    Trie * last_trie;
    static int count = 0;
    while (1) {
        last_trie = temp;
        temp = temp->next(last);
        if (temp == NULL) {
            break;
        }
        last = getchar();
    }
    return Token(last_trie->get_data());
};

void init() {
    punc_map.insert("(", lparen);
    punc_map.insert(")", rparen);
    punc_map.insert("{", lbrace);
    punc_map.insert("}", rbrace);
    punc_map.insert("=", equ);
    punc_map.insert("+", plus);
    punc_map.insert("*", star);
    punc_map.insert(",", comma);
};

extern void parse();

int main(int argc, char ** argv) {
    if (argc < 2) {
        exit(1);
    }
    freopen(argv[1], "r", stdin);
    init();
    while (1) {
        Token token = lex();
        if (token < 0 || token == eof) {
            break;
        }
        tokens[token_n++] = token;
    }
    parse();

    return 0;
}
