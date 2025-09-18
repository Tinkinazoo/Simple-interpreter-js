#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>

class Parser {
private:
    Lexer& lexer;
    Token currentToken{TokenType::END_OF_FILE, "", 0, 0};

    void advance();
    Token expect(TokenType expectedType, const std::string& errorMessage);
    Token peek();
    
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseLogicalOr();
    std::unique_ptr<Expression> parseLogicalAnd();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();
    std::unique_ptr<FunctionCall> parseFunctionCall(const std::string& functionName);
    std::unique_ptr<Expression> parseArrayLiteral();
    std::unique_ptr<Expression> parseObjectLiteral();
    
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseAssignmentOrExpression(); // НОВЫЙ МЕТОД
    std::unique_ptr<VariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<Assignment> parseAssignment(const std::string& variableName);
    std::unique_ptr<Assignment> parseArrayAssignment(const std::string& arrayName, std::unique_ptr<Expression> index); // НОВЫЙ МЕТОД
    std::unique_ptr<IfStatement> parseIfStatement();
    std::unique_ptr<WhileStatement> parseWhileStatement();
    std::unique_ptr<Statement> parseForStatement();
    std::unique_ptr<FunctionDeclaration> parseFunctionDeclaration();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<PrintStatement> parsePrintStatement();
    std::unique_ptr<Block> parseBlock();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();

public:
    Parser(Lexer& lexer);
    std::unique_ptr<Program> parse();
};

#endif // PARSER_H