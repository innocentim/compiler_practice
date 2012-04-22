#ifndef __LEX_HPP__
#define __LEX_HPP__

enum {
    identifier,
    constant_int,
    kwd_return,
    lparen, // (
    rparen, // )
    lbrace, // {
    rbrace, // }
    equ, // =
    plus, // +
    star, // *
    comma, // ,
    eof,
};

class Token {
    int _token;
public:
    void * data;
    Token(int _token = -1, void * data = NULL) : _token(_token), data(data) {};
    operator int() {
        return _token;
    };
};
#endif
