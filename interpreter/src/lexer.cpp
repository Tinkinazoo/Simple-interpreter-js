#include "lexer.h"
#include <cctype>
#include <map>
#include <stdexcept>

Lexer::Lexer(const std::string& source) 
    : source(source), start(0), current(0), line(1), column(1) {}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source[current++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

char Lexer::peekChar() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

Token Lexer::string() {
    while (peekChar() != '"' && !isAtEnd()) {
        if (peekChar() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string");
    }

    advance(); // Закрывающая кавычка

    std::string value = source.substr(start + 1, current - start - 2);
    return Token(TokenType::STRING, value, line, column);
}

Token Lexer::number() {
    while (isDigit(peekChar())) advance();

    if (peekChar() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peekChar())) advance();
    }

    std::string value = source.substr(start, current - start);
    return Token(TokenType::NUMBER, value, line, column);
}

Token Lexer::identifier() {
    while (isAlphaNumeric(peekChar())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;

    // Проверка ключевых слов
    if (text == "let") type = TokenType::LET;
    else if (text == "if") type = TokenType::IF;
    else if (text == "else") type = TokenType::ELSE;
    else if (text == "while") type = TokenType::WHILE;
    else if (text == "for") type = TokenType::FOR;
    else if (text == "fun") type = TokenType::FUN;
    else if (text == "return") type = TokenType::RETURN;
    else if (text == "print") type = TokenType::PRINT;
    else if (text == "true") type = TokenType::TRUE;
    else if (text == "false") type = TokenType::FALSE;
    else if (text == "and") type = TokenType::AND;
    else if (text == "or") type = TokenType::OR;
    else if (text == "not") type = TokenType::NOT;

    return Token(type, text, line, column);
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peekChar();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    // Комментарий до конца строки
                    while (peekChar() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return Token(TokenType::END_OF_FILE, "", line, column);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return Token(TokenType::LEFT_PAREN, "(", line, column);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", line, column);
        case '{': return Token(TokenType::LEFT_BRACE, "{", line, column);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", line, column);
        case ',': return Token(TokenType::COMMA, ",", line, column);
        case ';': return Token(TokenType::SEMICOLON, ";", line, column);
        case '+': return Token(TokenType::PLUS, "+", line, column);
        case '-': return Token(TokenType::MINUS, "-", line, column);
        case '*': return Token(TokenType::MULTIPLY, "*", line, column);
        case '/': return Token(TokenType::DIVIDE, "/", line, column);
        case '=':
            if (peekChar() == '=') {
                advance();
                return Token(TokenType::EQUALS, "==", line, column);
            }
            return Token(TokenType::ASSIGN, "=", line, column);
        case '!':
            if (peekChar() == '=') {
                advance();
                return Token(TokenType::NOT_EQUALS, "!=", line, column);
            }
            break;
        case '<':
            if (peekChar() == '=') {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", line, column);
            }
            return Token(TokenType::LESS, "<", line, column);
        case '>':
            if (peekChar() == '=') {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", line, column);
            }
            return Token(TokenType::GREATER, ">", line, column);
        case '"': return string();
    }

    return Token(TokenType::ERROR, std::string(1, c), line, column);
}

Token Lexer::getNextToken() {
    return scanToken();
}

Token Lexer::peek() {
    // Сохраняем текущее состояние
    int savedStart = start;
    int savedCurrent = current;
    int savedLine = line;
    int savedColumn = column;
    
    // Получаем следующий токен
    Token token = getNextToken();
    
    // Восстанавливаем состояние
    start = savedStart;
    current = savedCurrent;
    line = savedLine;
    column = savedColumn;
    
    return token;
}