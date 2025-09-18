#ifndef LEXER_H
#define LEXER_H

#include <string>
#include "token.h"  // Используем Token из token.h

class Lexer {
private:
    std::string source;
    int start;
    int current;
    int line;
    int column;

    char advance();
    char peekChar();
    char peekNext();
    bool isAtEnd();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    
    Token string();
    Token number();
    Token identifier();
    TokenType checkKeyword(int start, int length, const std::string& rest, TokenType type);
    void skipWhitespace();
    Token scanToken();
    
public:
    Lexer(const std::string& source);
    Token getNextToken();
    Token peek();
};

#endif // LEXER_H