#include "lexer.h"
#include <iostream>

std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"let", TokenType::LET},
    {"const", TokenType::CONST},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"function", TokenType::FUNCTION},
    {"return", TokenType::RETURN},
    {"print", TokenType::PRINT},
    {"class", TokenType::CLASS},
    {"extends", TokenType::EXTENDS},
    {"this", TokenType::THIS},
    {"super", TokenType::SUPER},
    {"new", TokenType::NEW},
    {"null", TokenType::NULL_TOKEN},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"BigInt", TokenType::BIGINT}
};

Lexer::Lexer(const std::string& source) 
    : source(source), position(0), line(1), column(1) {}

char Lexer::currentChar() {
    if (position >= source.length()) return '\0';
    return source[position];
}

char Lexer::nextChar() {
    if (position >= source.length()) return '\0';
    position++;
    column++;
    return currentChar();
}

void Lexer::skipWhitespace() {
    while (isspace(currentChar())) {
        if (currentChar() == '\n') {
            line++;
            column = 0;
        }
        nextChar();
    }
}

void Lexer::skipComment() {
    if (currentChar() == '#') {
        while (currentChar() != '\n' && currentChar() != '\0') {
            nextChar();
        }
    }
    if (currentChar() == '/' && position + 1 < source.length() && source[position + 1] == '/') {
        while (currentChar() != '\n' && currentChar() != '\0') {
            nextChar();
        }
    }
}

Token Lexer::readNumber() {
    std::string value;
    int startColumn = column;
    bool isFloat = false;
    
    while (isdigit(currentChar()) || currentChar() == '.') {
        if (currentChar() == '.') {
            if (isFloat) break; // Две точки - ошибка
            isFloat = true;
        }
        value += currentChar();
        nextChar();
    }
    
    // Проверяем, не является ли это BigInt
    if (currentChar() == 'n' && !isFloat) {
        nextChar();
        return {TokenType::BIGINT, value, line, startColumn};
    }
    
    return {TokenType::NUMBER, value, line, startColumn};
}

Token Lexer::readBigInt() {
    std::string value;
    int startColumn = column;
    
    while (isdigit(currentChar())) {
        value += currentChar();
        nextChar();
    }
    
    if (currentChar() == 'n') {
        nextChar();
    }
    
    return {TokenType::BIGINT, value, line, startColumn};
}

Token Lexer::readString() {
    std::string value;
    char delimiter = currentChar();
    int startColumn = column;
    nextChar(); // Пропускаем открывающую кавычку
    
    while (currentChar() != delimiter && currentChar() != '\0') {
        if (currentChar() == '\\') {
            nextChar(); // Пропускаем обратный слеш
            switch (currentChar()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += currentChar(); break;
            }
        } else {
            value += currentChar();
        }
        nextChar();
    }
    
    if (currentChar() == delimiter) {
        nextChar(); // Пропускаем закрывающую кавычку
    }
    
    return {TokenType::STRING, value, line, startColumn};
}

Token Lexer::readIdentifier() {
    std::string value;
    int startColumn = column;
    
    while (isalnum(currentChar()) || currentChar() == '_') {
        value += currentChar();
        nextChar();
    }
    
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return {it->second, value, line, startColumn};
    }
    
    return {TokenType::IDENTIFIER, value, line, startColumn};
}

Token Lexer::nextToken() {
    skipWhitespace();
    skipComment();
    skipWhitespace();
    
    if (currentChar() == '\0') {
        return {TokenType::END_OF_FILE, "", line, column};
    }
    
    char c = currentChar();
    int startColumn = column;
    
    // Числа
    if (isdigit(c)) {
        return readNumber();
    }
    
    // Строки
    if (c == '"' || c == '\'') {
        return readString();
    }
    
    // Идентификаторы и ключевые слова
    if (isalpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // Операторы и разделители
    switch (c) {
        case '+': nextChar(); return {TokenType::PLUS, "+", line, startColumn};
        case '-': nextChar(); return {TokenType::MINUS, "-", line, startColumn};
        case '*': nextChar(); return {TokenType::MULTIPLY, "*", line, startColumn};
        case '/': nextChar(); return {TokenType::DIVIDE, "/", line, startColumn};
        case '=': 
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return {TokenType::EQUALS, "==", line, startColumn};
            }
            return {TokenType::ASSIGN, "=", line, startColumn};
        case '!':
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return {TokenType::NOT_EQUALS, "!=", line, startColumn};
            }
            break;
        case '<':
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return {TokenType::LESS_EQUAL, "<=", line, startColumn};
            }
            return {TokenType::LESS, "<", line, startColumn};
        case '>':
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return {TokenType::GREATER_EQUAL, ">=", line, startColumn};
            }
            return {TokenType::GREATER, ">", line, startColumn};
        case '(': nextChar(); return {TokenType::LPAREN, "(", line, startColumn};
        case ')': nextChar(); return {TokenType::RPAREN, ")", line, startColumn};
        case '{': nextChar(); return {TokenType::LBRACE, "{", line, startColumn};
        case '}': nextChar(); return {TokenType::RBRACE, "}", line, startColumn};
        case '[': nextChar(); return {TokenType::LBRACKET, "[", line, startColumn};
        case ']': nextChar(); return {TokenType::RBRACKET, "]", line, startColumn};
        case ';': nextChar(); return {TokenType::SEMICOLON, ";", line, startColumn};
        case ',': nextChar(); return {TokenType::COMMA, ",", line, startColumn};
        case '.': nextChar(); return {TokenType::DOT, ".", line, startColumn};
        case ':': nextChar(); return {TokenType::COLON, ":", line, startColumn};
    }
    
    // Неизвестный символ
    std::string unknown(1, c);
    nextChar();
    return {TokenType::UNKNOWN, unknown, line, startColumn};
}

Token Lexer::peekToken() {
    size_t savedPosition = position;
    int savedLine = line;
    int savedColumn = column;
    
    Token token = nextToken();
    
    position = savedPosition;
    line = savedLine;
    column = savedColumn;
    
    return token;
}
