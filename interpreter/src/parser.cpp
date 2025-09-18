#include "parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer), currentToken(TokenType::END_OF_FILE, "", 0, 0) {
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
        case TokenType::IDENTIFIER:
            return parseAssignmentOrExpression();
        default:
            return parseExpressionStatement();
    }
}

std::unique_ptr<Assignment> Parser::parseArrayAssignment(const std::string& arrayName, std::unique_ptr<Expression> index) {
    expect(TokenType::ASSIGN, "Expected '=' after array index");
    
    auto value = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after array assignment");
    
    // Создаем IndexExpression для целевого элемента
    auto target = std::make_unique<IndexExpression>(
        std::make_unique<Identifier>(arrayName),
        std::move(index)  // Используем std::move
    );
    
    return std::make_unique<Assignment>(arrayName, std::move(value), std::move(target));
}

std::unique_ptr<Assignment> Parser::parseAssignment(const std::string& variableName) {
    // Текущий токен уже '='
    expect(TokenType::ASSIGN, "Expected '=' after variable name");
    
    auto value = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after assignment");
    
    return std::make_unique<Assignment>(variableName, std::move(value));
}

std::unique_ptr<Statement> Parser::parseAssignmentOrExpression() {
    // Сохраняем позицию для отката
    Token identifierToken = currentToken;
    advance();
    
    // Проверяем, является ли это присваиванием
    if (currentToken.type == TokenType::ASSIGN) {
        // Это присваивание: variable = expression
        return parseAssignment(identifierToken.lexeme);
    }
    else if (currentToken.type == TokenType::LEFT_BRACKET) {
        // Это может быть присваивание элемента массива: arr[index] = value
        advance();
        auto index = parseExpression();
        expect(TokenType::RIGHT_BRACKET, "Expected ']' after index");
        
        if (currentToken.type == TokenType::ASSIGN) {
            // Присваивание элементу массива
            return parseArrayAssignment(identifierToken.lexeme, std::move(index));
        } else {
            // Доступ к элементу массива в выражении - откатываемся
            currentToken = identifierToken;
            return parseExpressionStatement();
        }
    }
    else {
        // Это выражение - откатываемся
        currentToken = identifierToken;
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

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
    expect(TokenType::IF, "Expected 'if'");
    
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    expect(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    
    auto thenBlock = parseBlock();  // Должен парсить блок { }
    
    std::unique_ptr<Block> elseBlock = nullptr;
    if (currentToken.type == TokenType::ELSE) {
        advance();
        elseBlock = parseBlock();  // Должен парсить блок { }
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
    
    // Убедимся, что мы действительно на точке с запятой
    if (currentToken.type != TokenType::SEMICOLON) {
        std::cout << "DEBUG: Before SEMICOLON error - current token: " 
                  << tokenTypeToString(currentToken.type) 
                  << " '" << currentToken.lexeme << "'" << std::endl;
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
    expect(TokenType::LEFT_BRACE, "Expected '{'");  // Должен ожидать {
    
    auto block = std::make_unique<Block>();
    
    while (currentToken.type != TokenType::RIGHT_BRACE && 
           currentToken.type != TokenType::END_OF_FILE) {
        block->addStatement(parseStatement());
    }
    
    expect(TokenType::RIGHT_BRACE, "Expected '}' after block");  // Должен ожидать }
    
    return block;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto expression = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExpressionStatement>(std::move(expression));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    auto expr = parseLogicalOr();
    
    // Обработка индексов и свойств
    while (currentToken.type == TokenType::LEFT_BRACKET || 
           currentToken.type == TokenType::DOT) {
        if (currentToken.type == TokenType::LEFT_BRACKET) {
            advance(); // Пропускаем '['
            auto index = parseExpression();
            expect(TokenType::RIGHT_BRACKET, "Expected ']' after index");
            expr = std::make_unique<IndexExpression>(std::move(expr), std::move(index));
        } else if (currentToken.type == TokenType::DOT) {
            advance(); // Пропускаем '.'
            Token property = expect(TokenType::IDENTIFIER, "Expected property name after '.'");
            expr = std::make_unique<PropertyAccess>(std::move(expr), property.lexeme);
        }
    }
    
    return expr;
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
    
    while (currentToken.type == TokenType::MULTIPLY || currentToken.type == TokenType::DIVIDE) {
        Token op = currentToken;
        advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryOperation>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto left = parseUnary();
    
    while (currentToken.type == TokenType::PLUS || currentToken.type == TokenType::MINUS) {
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
        
        case TokenType::LEFT_BRACKET:
            return parseArrayLiteral();
            
        case TokenType::LEFT_BRACE:
            return parseObjectLiteral();
            
        case TokenType::NULL_TOKEN:
            advance();
            return std::make_unique<NullLiteral>();

        default:
            throw std::runtime_error("Expected expression, got: " + tokenTypeToString(currentToken.type));
    }
}

Token Parser::peek() {
    return lexer.peek();  // Предполагая, что в Lexer есть метод peek()
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

// Новые методы парсинга
std::unique_ptr<Expression> Parser::parseArrayLiteral() {
    expect(TokenType::LEFT_BRACKET, "Expected '['");
    
    std::vector<std::unique_ptr<Expression>> elements;
    if (currentToken.type != TokenType::RIGHT_BRACKET) {
        do {
            elements.push_back(parseExpression());
            if (currentToken.type != TokenType::COMMA) {
                break;
            }
            advance();
        } while (true);
    }
    
    expect(TokenType::RIGHT_BRACKET, "Expected ']' after array elements");
    return std::make_unique<ArrayLiteral>(std::move(elements));
}

std::unique_ptr<Expression> Parser::parseObjectLiteral() {
    expect(TokenType::LEFT_BRACE, "Expected '{'");
    
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> properties;
    if (currentToken.type != TokenType::RIGHT_BRACE) {
        do {
            Token key = expect(TokenType::STRING, "Expected string key");
            expect(TokenType::COLON, "Expected ':' after key");
            auto value = parseExpression();
            
            properties.emplace_back(key.lexeme, std::move(value));
            
            if (currentToken.type != TokenType::COMMA) {
                break;
            }
            advance();
        } while (true);
    }
    
    expect(TokenType::RIGHT_BRACE, "Expected '}' after object properties");
    return std::make_unique<ObjectLiteral>(std::move(properties));
}

// Парсинг for цикла
std::unique_ptr<Statement> Parser::parseForStatement() {
    expect(TokenType::FOR, "Expected 'for'");
    expect(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    
    // Инициализатор
    std::unique_ptr<Statement> initializer;
    if (currentToken.type == TokenType::LET) {
        initializer = parseVariableDeclaration();
    } else if (currentToken.type == TokenType::SEMICOLON) {
        advance(); // Пропускаем ';'
        initializer = nullptr;
    } else {
        initializer = parseExpressionStatement();
    }
    
    // Условие
    std::unique_ptr<Expression> condition;
    if (currentToken.type != TokenType::SEMICOLON) {
        condition = parseExpression();
    }
    expect(TokenType::SEMICOLON, "Expected ';' after for condition");
    
    // Инкремент
    std::unique_ptr<Expression> increment;
    if (currentToken.type != TokenType::RIGHT_PAREN) {
        increment = parseExpression();
    }
    expect(TokenType::RIGHT_PAREN, "Expected ')' after for clauses");
    
    auto body = parseBlock();
    
    return std::make_unique<ForStatement>(
        std::move(initializer),
        std::move(condition),
        std::move(increment),
        std::move(body)
    );
}