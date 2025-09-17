#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>
#include <stdexcept>

class ParserError : public std::runtime_error {
public:
    ParserError(const std::string& message) : std::runtime_error(message) {}
};

class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer) {
        currentToken = lexer.nextToken();
    }
    
    std::unique_ptr<ASTNode> parse();
    
private:
    Lexer& lexer;
    Token currentToken;
    
    void eat(TokenType expectedType);
    Token expect(TokenType expectedType, const std::string& errorMessage);
    
    // Правила грамматики
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseMultiplicative();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parseVariableDeclaration();
    std::unique_ptr<ASTNode> parsePrintStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseFunctionExpression();
    std::unique_ptr<ASTNode> parseCallExpression(std::unique_ptr<ASTNode> callee);
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseClassDeclaration();
    std::unique_ptr<ASTNode> parseNewExpression();
    std::unique_ptr<ASTNode> parsePropertyAccess(std::unique_ptr<ASTNode> object);
};

#endif
