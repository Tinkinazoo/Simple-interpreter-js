#include "parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();
}

void Parser::advance() {
    currentToken = lexer.getNextToken();
}

Token Parser::expect(TokenType expectedType, const std::string& errorMessage) {
    if (currentToken.type != expectedType) {
        throw std::runtime_error(errorMessage + ". Got: " + tokenTypeToString(currentToken.type) + 
                                ", expected: " + tokenTypeToString(expectedType));
    }
    Token token = currentToken;
    advance();
    return token;
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    
    while (currentToken.type != TokenType::END_OF_FILE) {
        try {
            program->addStatement(parseStatement());
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse error at line " << currentToken.line << ": " << e.what() << std::endl;
            // Попытка восстановления - пропускаем до следующего оператора
            while (currentToken.type != TokenType::SEMICOLON && 
                   currentToken.type != TokenType::END_OF_FILE) {
                advance();
            }
            if (currentToken.type == TokenType::SEMICOLON) {
                advance();
            }
        }
    }
    
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::LET:
            return parseVariableDeclaration();
        case TokenType::IF:
            return parseIfStatement();
        case TokenType::WHILE:
            return parseWhileStatement();
        case TokenType::FOR:
            return parseForStatement();
        case TokenType::FUN:
            return parseFunctionDeclaration();
        case TokenType::RETURN:
            return parseReturnStatement();
        case TokenType::PRINT:
            return parsePrintStatement();
        case TokenType::LEFT_BRACE:
            return parseBlock();
        default:
            return parseExpressionStatement();
    }
}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration() {
    expect(TokenType::LET, "Expected 'let'");
    
    Token nameToken = expect(TokenType::IDENTIFIER, "Expected variable name after 'let'");
    
    expect(TokenType::ASSIGN, "Expected '=' after variable name");
    
    auto initializer = parseExpression();
    
    expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VariableDeclaration>(nameToken.lexeme, std::move(initializer));
}

std::unique_ptr<Assignment> Parser::parseAssignment() {
    Token nameToken = expect(TokenType::IDENTIFIER, "Expected variable name");
    
    expect(TokenType::ASSIGN, "Expected '=' after variable name");
    
    auto value = parseExpression();
    
    expect(TokenType::SEMICOLON, "Expected ';' after assignment");
    
    return std::make_unique<Assignment>(nameToken.lexeme, std::move(value));
}

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
    expect(TokenType::IF, "Expected 'if'");
    
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    
    auto thenBlock = parseBlock();
    
    std::unique_ptr<Block> elseBlock = nullptr;
    if (currentToken.type == TokenType::ELSE) {
        advance();
        elseBlock = parseBlock();
    }
    
    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

std::unique_ptr<WhileStatement> Parser::parseWhileStatement() {
    expect(TokenType::WHILE, "Expected 'while'");
    
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    
    auto body = parseBlock();
    
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::parseForStatement() {
    expect(TokenType::FOR, "Expected 'for'");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    
    // Парсим инициализатор
    std::unique_ptr<Statement> initializer;
    if (currentToken.type == TokenType::LET) {
        initializer = parseVariableDeclaration();
    } else if (currentToken.type == TokenType::SEMICOLON) {
        advance(); // Пропускаем точку с запятой
        initializer = nullptr;
    } else {
        initializer = parseExpressionStatement();
    }
    
    // Парсим условие
    std::unique_ptr<Expression> condition;
    if (currentToken.type != TokenType::SEMICOLON) {
        condition = parseExpression();
    }
    expect(TokenType::SEMICOLON, "Expected ';' after for condition");
    
    // Парсим инкремент
    std::unique_ptr<Expression> increment;
    if (currentToken.type != TokenType::RIGHT_PAREN) {
        increment = parseExpression();
    }
    expect(TokenType::RIGHT_PAREN, "Expected ')' after for clauses");
    
    auto body = parseBlock();
    
    // Преобразуем for в while
    auto whileBody = std::make_unique<Block>();
    if (body) {
        whileBody->addStatement(std::move(body));
    }
    if (increment) {
        whileBody->addStatement(std::make_unique<ExpressionStatement>(std::move(increment)));
    }
    
    auto whileLoop = std::make_unique<WhileStatement>(
        condition ? std::move(condition) : std::make_unique<BooleanLiteral>(true),
        std::move(whileBody)
    );
    
    auto resultBlock = std::make_unique<Block>();
    if (initializer) {
        resultBlock->addStatement(std::move(initializer));
    }
    resultBlock->addStatement(std::move(whileLoop));
    
    return resultBlock;
}

std::unique_ptr<FunctionDeclaration> Parser::parseFunctionDeclaration() {
    expect(TokenType::FUN, "Expected 'fun'");
    
    Token nameToken = expect(TokenType::IDENTIFIER, "Expected function name");
    
    expect(TokenType::LEFT_PAREN, "Expected '(' after function name");
    
    std::vector<std::string> parameters;
    if (currentToken.type != TokenType::RIGHT_PAREN) {
        do {
            Token paramToken = expect(TokenType::IDENTIFIER, "Expected parameter name");
            parameters.push_back(paramToken.lexeme);
            
            if (currentToken.type != TokenType::COMMA) {
                break;
            }
            advance();
        } while (true);
    }
    
    expect(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    
    auto body = parseBlock();
    
    return std::make_unique<FunctionDeclaration>(nameToken.lexeme, parameters, std::move(body));
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    expect(TokenType::RETURN, "Expected 'return'");
    
    std::unique_ptr<Expression> value = nullptr;
    if (currentToken.type != TokenType::SEMICOLON) {
        value = parseExpression();
    }
    
    expect(TokenType::SEMICOLON, "Expected ';' after return statement");
    
    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<PrintStatement> Parser::parsePrintStatement() {
    expect(TokenType::PRINT, "Expected 'print'");
    
    auto expression = parseExpression();
    
    expect(TokenType::SEMICOLON, "Expected ';' after print statement");
    
    return std::make_unique<PrintStatement>(std::move(expression));
}

std::unique_ptr<Block> Parser::parseBlock() {
    expect(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto block = std::make_unique<Block>();
    
    while (currentToken.type != TokenType::RIGHT_BRACE && 
           currentToken.type != TokenType::END_OF_FILE) {
        block->addStatement(parseStatement());
    }
    
    expect(TokenType::RIGHT_BRACE, "Expected '}' after block");
    
    return block;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto expression = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExpressionStatement>(std::move(expression));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseLogicalOr();
}

std::unique_ptr<Expression> Parser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    while (currentToken.type == TokenType::OR) {
        Token op = currentToken;
        advance();
        auto right = parseLogicalAnd();
        left = std::make_unique<BinaryOperation>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expression> Parser::parseLogicalAnd() {
    auto left = parseEquality();
    
    while (currentToken.type == TokenType::AND) {
        Token op = currentToken;
        advance();
        auto right = parseEquality();
        left = std::make_unique<BinaryOperation>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto left = parseComparison();
    
    while (currentToken.type == TokenType::EQUALS || currentToken.type == TokenType::NOT_EQUALS) {
        Token op = currentToken;
        advance();
        auto right = parseComparison();
        left = std::make_unique<BinaryOperation>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto left = parseTerm();
    
    while (currentToken.type == TokenType::LESS || currentToken.type == TokenType::LESS_EQUAL ||
           currentToken.type == TokenType::GREATER || currentToken.type == TokenType::GREATER_EQUAL) {
        Token op = currentToken;
        advance();
        auto right = parseTerm();
        left = std::make_unique<BinaryOperation>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto left = parseFactor();
    
    while (currentToken.type == TokenType::PLUS || currentToken.type == TokenType::MINUS) {
        Token op = currentToken;
        advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryOperation>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto left = parseUnary();
    
    while (currentToken.type == TokenType::MULTIPLY || currentToken.type == TokenType::DIVIDE) {
        Token op = currentToken;
        advance();
        auto right = parseUnary();
        left = std::make_unique<BinaryOperation>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (currentToken.type == TokenType::NOT || currentToken.type == TokenType::MINUS) {
        Token op = currentToken;
        advance();
        auto operand = parseUnary();
        return std::make_unique<UnaryOperation>(op.lexeme, std::move(operand));
    }
    
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    switch (currentToken.type) {
        case TokenType::NUMBER: {
            double value = std::stod(currentToken.lexeme);
            advance();
            return std::make_unique<NumberLiteral>(value);
        }
        
        case TokenType::STRING: {
            std::string value = currentToken.lexeme;
            advance();
            return std::make_unique<StringLiteral>(value);
        }
        
        case TokenType::TRUE: {
            advance();
            return std::make_unique<BooleanLiteral>(true);
        }
        
        case TokenType::FALSE: {
            advance();
            return std::make_unique<BooleanLiteral>(false);
        }
        
        case TokenType::IDENTIFIER: {
            std::string name = currentToken.lexeme;
            advance();
            
            // Проверяем, является ли это вызовом функции
            if (currentToken.type == TokenType::LEFT_PAREN) {
                return parseFunctionCall(name);
            }
            
            return std::make_unique<Identifier>(name);
        }
        
        case TokenType::LEFT_PAREN: {
            advance();
            auto expression = parseExpression();
            expect(TokenType::RIGHT_PAREN, "Expected ')' after expression");
            return expression;
        }
        
        default:
            throw std::runtime_error("Expected expression, got: " + tokenTypeToString(currentToken.type));
    }
}

std::unique_ptr<FunctionCall> Parser::parseFunctionCall(const std::string& functionName) {
    expect(TokenType::LEFT_PAREN, "Expected '(' after function name");
    
    std::vector<std::unique_ptr<Expression>> arguments;
    if (currentToken.type != TokenType::RIGHT_PAREN) {
        do {
            arguments.push_back(parseExpression());
            
            if (currentToken.type != TokenType::COMMA) {
                break;
            }
            advance();
        } while (true);
    }
    
    expect(TokenType::RIGHT_PAREN, "Expected ')' after function arguments");
    
    return std::make_unique<FunctionCall>(functionName, std::move(arguments));
}
