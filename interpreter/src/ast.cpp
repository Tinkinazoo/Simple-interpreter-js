#include "ast.h"
#include <iomanip>

// NumberLiteral
NumberLiteral::NumberLiteral(double value) : value(value) {}
void NumberLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "NumberLiteral(" << value << ")\n";
}

// StringLiteral
StringLiteral::StringLiteral(const std::string& value) : value(value) {}
void StringLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "StringLiteral(\"" << value << "\")\n";
}

// BooleanLiteral
BooleanLiteral::BooleanLiteral(bool value) : value(value) {}
void BooleanLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BooleanLiteral(" << (value ? "true" : "false") << ")\n";
}

// Identifier
Identifier::Identifier(const std::string& name) : name(name) {}
void Identifier::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Identifier(" << name << ")\n";
}

// BinaryOperation
BinaryOperation::BinaryOperation(std::unique_ptr<Expression> left, const std::string& op, std::unique_ptr<Expression> right)
    : left(std::move(left)), op(op), right(std::move(right)) {}
void BinaryOperation::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BinaryOperation(" << op << ")\n";
    left->print(indent + 2);
    right->print(indent + 2);
}

// UnaryOperation
UnaryOperation::UnaryOperation(const std::string& op, std::unique_ptr<Expression> operand)
    : op(op), operand(std::move(operand)) {}
void UnaryOperation::print(int indent) const {
    std::cout << std::string(indent, ' ') << "UnaryOperation(" << op << ")\n";
    operand->print(indent + 2);
}

// FunctionCall
FunctionCall::FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments)
    : functionName(functionName), arguments(std::move(arguments)) {}
void FunctionCall::print(int indent) const {
    std::cout << std::string(indent, ' ') << "FunctionCall(" << functionName << ")\n";
    for (const auto& arg : arguments) {
        arg->print(indent + 2);
    }
}

// ExpressionStatement
ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression)
    : expression(std::move(expression)) {}
void ExpressionStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ExpressionStatement:\n";
    expression->print(indent + 2);
}

// Block
Block::Block() {}
void Block::addStatement(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));
}
void Block::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Block:\n";
    for (const auto& stmt : statements) {
        stmt->print(indent + 2);
    }
}

// VariableDeclaration
VariableDeclaration::VariableDeclaration(const std::string& variableName, std::unique_ptr<Expression> initializer)
    : variableName(variableName), initializer(std::move(initializer)) {}
void VariableDeclaration::print(int indent) const {
    std::cout << std::string(indent, ' ') << "VariableDeclaration(" << variableName << ")\n";
    if (initializer) {
        initializer->print(indent + 2);
    }
}

// Assignment
Assignment::Assignment(const std::string& variableName, std::unique_ptr<Expression> value)
    : variableName(variableName), value(std::move(value)) {}
void Assignment::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Assignment(" << variableName << ")\n";
    value->print(indent + 2);
}

// IfStatement
IfStatement::IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Block> thenBlock, std::unique_ptr<Block> elseBlock)
    : condition(std::move(condition)), thenBlock(std::move(thenBlock)), elseBlock(std::move(elseBlock)) {}
void IfStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "IfStatement:\n";
    condition->print(indent + 2);
    std::cout << std::string(indent, ' ') << "Then:\n";
    thenBlock->print(indent + 2);
    if (elseBlock) {
        std::cout << std::string(indent, ' ') << "Else:\n";
        elseBlock->print(indent + 2);
    }
}

// WhileStatement
WhileStatement::WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Block> body)
    : condition(std::move(condition)), body(std::move(body)) {}
void WhileStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "WhileStatement:\n";
    condition->print(indent + 2);
    std::cout << std::string(indent, ' ') << "Body:\n";
    body->print(indent + 2);
}

// ReturnStatement
ReturnStatement::ReturnStatement(std::unique_ptr<Expression> value) : value(std::move(value)) {}
void ReturnStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ReturnStatement:\n";
    if (value) {
        value->print(indent + 2);
    }
}

// PrintStatement
PrintStatement::PrintStatement(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {}
void PrintStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "PrintStatement:\n";
    expression->print(indent + 2);
}

// FunctionDeclaration
FunctionDeclaration::FunctionDeclaration(const std::string& functionName, const std::vector<std::string>& parameters, std::unique_ptr<Block> body)
    : functionName(functionName), parameters(parameters), body(std::move(body)) {}
void FunctionDeclaration::print(int indent) const {
    std::cout << std::string(indent, ' ') << "FunctionDeclaration(" << functionName << ")\n";
    std::cout << std::string(indent + 2, ' ') << "Parameters: ";
    for (const auto& param : parameters) {
        std::cout << param << " ";
    }
    std::cout << "\n";
    body->print(indent + 2);
}

// Program
Program::Program() {}
void Program::addStatement(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));
}
void Program::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Program:\n";
    for (const auto& stmt : statements) {
        stmt->print(indent + 2);
    }
}
