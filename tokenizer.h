#ifndef tokenizer_h
#define tokenizer_h 


#include <string>
#include <vector>

enum TokenType
{
    TOKEN_COMMAND, //0   
    TOKEN_ARGUMENT,   //1
    TOKEN_REDIRECT,  //2
    TOKEN_APPEND,   //3
    TOKEN_INPUT,     //4
    TOKEN_ERROR,    //5
    TOKEN_PIPE,       //6
    TOKEN_BACKGROUND, //7
    TOKEN_REDIRECT_AND_ERROR, //8
    TOKEN_EOF,       //9
};

struct Token
{
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string &input);

#endif
