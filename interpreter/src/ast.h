#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>

// Базовый класс для всех узлов AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// Выражения
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

// Числовой литерал
class NumberLiteral : public Expression {
public:
    double value;
    NumberLiteral(double value);
    void print(int indent) const override;
};

// Строковый литерал
class StringLiteral : public Expression {
public:
    std::string value;
    StringLiteral(const std::string& value);
    void print(int indent) const override;
};

// Булев литерал
class BooleanLiteral : public Expression {
public:
    bool value;
    BooleanLiteral(bool value);
    void print(int indent) const override;
};

// Идентификатор
class Identifier : public Expression {
public:
    std::string name;
    Identifier(const std::string& name);
    void print(int indent) const override;
};

// Бинарная операция
class BinaryOperation : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;
    BinaryOperation(std::unique_ptr<Expression> left, const std::string& op, std::unique_ptr<Expression> right);
    void print(int indent) const override;
};

// Унарная операция
class UnaryOperation : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> operand;
    UnaryOperation(const std::string& op, std::unique_ptr<Expression> operand);
    void print(int indent) const override;
};

// Вызов функции
class FunctionCall : public Expression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;
    FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments);
    void print(int indent) const override;
};

// Операторы
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

// Выражение как оператор
class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    ExpressionStatement(std::unique_ptr<Expression> expression);
    void print(int indent) const override;
};

// Блок кода
class Block : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    Block();
    void addStatement(std::unique_ptr<Statement> stmt);
    void print(int indent) const override;
};

// Объявление переменной
class VariableDeclaration : public Statement {
public:
    std::string variableName;
    std::unique_ptr<Expression> initializer;
    VariableDeclaration(const std::string& variableName, std::unique_ptr<Expression> initializer);
    void print(int indent) const override;
};

// Присваивание
class Assignment : public Statement {
public:
    std::string variableName;
    std::unique_ptr<Expression> value;
    Assignment(const std::string& variableName, std::unique_ptr<Expression> value);
    void print(int indent) const override;
};

// Оператор if
class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> thenBlock;
    std::unique_ptr<Block> elseBlock;
    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Block> thenBlock, std::unique_ptr<Block> elseBlock);
    void print(int indent) const override;
};

// Цикл while
class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> body;
    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Block> body);
    void print(int indent) const override;
};

// Оператор return
class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;
    ReturnStatement(std::unique_ptr<Expression> value);
    void print(int indent) const override;
};

// Оператор print
class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    PrintStatement(std::unique_ptr<Expression> expression);
    void print(int indent) const override;
};

// Объявление функции
class FunctionDeclaration : public Statement {
public:
    std::string functionName;
    std::vector<std::string> parameters;
    std::unique_ptr<Block> body;
    FunctionDeclaration(const std::string& functionName, const std::vector<std::string>& parameters, std::unique_ptr<Block> body);
    void print(int indent) const override;
};

// Программа
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    Program();
    void addStatement(std::unique_ptr<Statement> stmt);
    void print(int indent) const override;
};

#endif // AST_H