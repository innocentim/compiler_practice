#include "lex.hpp"
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>

class Trie {
    Token data;
    Trie * children[256]; 
    void printImpl() {
        printf("%lu [label=\"%d\"]\n", (unsigned long)this, (int)data);
        for (int i = 0; i < 256; i++) {
            if (children[i]) {
                printf("%lu -> %lu\n [label=\"%c\"]", (unsigned long)this, (unsigned long)children[i], i);
                children[i]->printImpl();
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
        printImpl();
        printf("}\n");
    };

    Trie * next(char last) {
        return children[(unsigned char)last];
    };

    Token getData() {
        return data;
    };
};

static Trie puncMap;
Token tokens[1024];
int tokenN = 0;

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
            return Token(KWD_RETURN);
        }
        return Token(IDENTIFIER, &idenStr);
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
        numInt = sign * numInt;
        return Token(CONSTANT_INT, &numInt);
    } else if (last == EOF) {
        return Token(_EOF);
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
    if (lastTrie->getData() == SLASHSLASH) {
        while (last != '\n') {
            last = getchar();
        }
        last = getchar();
        return lex();
    }
    return Token(lastTrie->getData());
};

void init() {
    puncMap.insert("(", LPAREN);
    puncMap.insert(")", RPAREN);
    puncMap.insert("{", LBRACE);
    puncMap.insert("}", RBRACE);
    puncMap.insert("=", EQU);
    puncMap.insert("+", PLUS);
    puncMap.insert("*", STAR);
    puncMap.insert(",", COMMA);
    puncMap.insert("//", SLASHSLASH);
};

extern void parseInit();

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
        if (token == _EOF) {
            break;
        }
        tokens[tokenN++] = token;
    }
    parseInit();

    return 0;
}
