#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

enum class TokenType {
    // Литералы
    NUMBER,
    BIGINT,
    STRING,
    IDENTIFIER,
    
    // Ключевые слова
    LET,
    CONST,
    IF,
    ELSE,
    WHILE,
    FOR,
    FUNCTION,
    RETURN,
    PRINT,
    CLASS,
    EXTENDS,
    THIS,
    SUPER,
    NEW,
    NULL_TOKEN,
    TRUE,
    FALSE,
    
    // Операторы
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    EQUALS,
    NOT_EQUALS,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    
    // Разделители
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    SEMICOLON,
    COMMA,
    DOT,
    COLON,
    
    // Специальные
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
};

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();
    Token peekToken();
    
private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    char currentChar();
    char nextChar();
    void skipWhitespace();
    void skipComment();
    
    Token readNumber();
    Token readBigInt();
    Token readString();
    Token readIdentifier();
    
    static std::unordered_map<std::string, TokenType> keywords;
};

#endif
