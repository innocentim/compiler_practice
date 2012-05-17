#include "lex.hpp"
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>

class Trie {
    Token data;
    Trie * children[256]; 
    void _print() {
        printf("%lu [label=\"%d\"]\n", (unsigned long)this, (int)data);
        for (int i = 0; i < 256; i++) {
            if (children[i]) {
                printf("%lu -> %lu\n [label=\"%c\"]", (unsigned long)this, (unsigned long)children[i], i);
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

    void print() {
        printf("digraph a {\n");
        _print();
        printf("}\n");
    };

    Trie * next(char last) {
        return children[(unsigned char)last];
    };

    Token get_data() {
        return data;
    };
};

static Trie puncMap;
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
        std::string & idenStr = *(new std::string);
        idenStr += last;
        while (isalpha(last = getchar()) || last == '_' || isdigit(last)) {
            idenStr += last;
        }
        if (idenStr == "return") {
            delete &idenStr;
            return Token(kwd_return);
        }
        return Token(identifier, &idenStr);
    } else if (isdigit(last) || last == '-') { // constant_number := [0-9]+
        long long & numInt = *(new long long);
        int sign = 1;
        if (last == '-') {
            sign = -1;
            last = getchar();
        }
        numInt = last - '0';
        while (isdigit(last = getchar())) {
            numInt = numInt * 10 + last - '0';
        }
        return Token(constant_int, &numInt);
    } else if (last == EOF) {
        return Token(eof);
    }
    Trie * temp = &puncMap;
    Trie * lastTrie;
    while (1) {
        lastTrie = temp;
        temp = temp->next(last);
        if (temp == NULL) {
            break;
        }
        last = getchar();
    }
    if (lastTrie->get_data() == slashslash) {
        while (last != '\n') {
            last = getchar();
        }
        last = getchar();
        return lex();
    }
    return Token(lastTrie->get_data());
};

void init() {
    puncMap.insert("(", lparen);
    puncMap.insert(")", rparen);
    puncMap.insert("{", lbrace);
    puncMap.insert("}", rbrace);
    puncMap.insert("=", equ);
    puncMap.insert("+", plus);
    puncMap.insert("*", star);
    puncMap.insert(",", comma);
    puncMap.insert("//", slashslash);
};

extern void parse_init();

int main(int argc, char ** argv) {
    if (argc < 2) {
        exit(1);
    }
    freopen(argv[1], "r", stdin);
    init();
    Token token;
    while (1) {
        token = lex();
        if (token < 0) {
            exit(1);
        }
        if (token == eof) {
            break;
        }
        tokens[token_n++] = token;
    }
    parse_init();

    return 0;
}
