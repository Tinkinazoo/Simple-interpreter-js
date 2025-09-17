#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

enum class TokenType {
    // Ключевые слова
    LET, IF, ELSE, WHILE, FOR, FUN, RETURN, PRINT,
    TRUE, FALSE, AND, OR, NOT,
    
    // Идентификаторы и литералы
    IDENTIFIER, NUMBER, STRING,
    
    // Операторы
    PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN, EQUALS, NOT_EQUALS,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
    
    // Разделители
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, SEMICOLON,
    
    // Специальные токены
    END_OF_FILE, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    
    Token(TokenType type, const std::string& lexeme, int line);
};

std::ostream& operator<<(std::ostream& os, const Token& token);
std::string tokenTypeToString(TokenType type);

#endif // TOKEN_H
