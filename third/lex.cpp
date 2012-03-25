#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>

enum Token {
    identifier,
    constant_int,
    kwd_return,
    lparen, // (
    rparen, // )
    lbrace, // {
    rbrace, // }
    equ, // =
    equequ, // ==
    plus, // +
    eof,
};

class Trie {
public:
    int data;
    Trie * children[256]; 
    Trie() {
        for (int i = 0; i < 256; i++) {
            children[i] = NULL;
        }
    };

    void insert(const char * str, int token) {
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

    int operator[](const char * str) {
        Trie * temp = this;
        int len = strlen(str);
        for (int i = 0; i < len; i++) {
            char ch = str[i];
            if (temp->children[(unsigned char)ch] == NULL) {
                return -1;
            }
            temp = temp->children[(unsigned char)ch];
        }
        return temp->data;
    };
};

static Trie punc_map;
std::string iden_str;
long long num_int;

inline
bool isalpha(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
};

inline
bool isdigit(char ch) {
    return '0' <= ch && ch <= '9';
};

int lex() {
    static char last = ' ';
    while (last == '\n' || last == '\r' || last == ' ' || last == '\t') {
        last = getchar();
    }
    if (isalpha(last) || last == '_') { // identifier := [_a-zA-Z][_a-zA-Z0-9]*
        iden_str = "";
        iden_str += last;
        last = getchar();
        while (isalpha(last) || last == '_' || isdigit(last)) {
            iden_str += last;
            last = getchar();
        }
        if (iden_str == "return") {
            return kwd_return;
        }
        return identifier;
    } else if (isdigit(last)) { // [0-9]+
        num_int = last - '0';
        while (isdigit(last = getchar())) {
            num_int = num_int * 10 + last - '0';
        }
        return constant_int;
    } 
    return -1;
};

void init() {
    punc_map.insert("(", lparen);
    punc_map.insert(")", rparen);
    punc_map.insert("{", lbrace);
    punc_map.insert("}", rbrace);
    punc_map.insert("=", equ);
    punc_map.insert("==", equequ);
    punc_map.insert("+", plus);
};

int main(int argc, char ** argv) {
    if (argc < 2) {
        exit(1);
    }
    freopen(argv[1], "r", stdin);
    init();
    while (1) {
        int token = lex();
        if (token < 0) {
            break;
        }
        printf("%d", token);
        if (token == constant_int) {
            printf(" %lld", num_int);
        }
        printf("\n");
    }

    return 0;
};
