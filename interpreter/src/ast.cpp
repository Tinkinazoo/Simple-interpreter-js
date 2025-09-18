#include "ast.h"
#include <iomanip>

// NumberLiteral
NumberLiteral::NumberLiteral(double value) : value(value) {}
void NumberLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "NumberLiteral(" << value << ")\n";
}
std::unique_ptr<ASTNode> NumberLiteral::clone() const {
    return std::make_unique<NumberLiteral>(value);
}

// StringLiteral
StringLiteral::StringLiteral(const std::string& value) : value(value) {}
void StringLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "StringLiteral(\"" << value << "\")\n";
}
std::unique_ptr<ASTNode> StringLiteral::clone() const {
    return std::make_unique<StringLiteral>(value);
}

// BooleanLiteral
BooleanLiteral::BooleanLiteral(bool value) : value(value) {}
void BooleanLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BooleanLiteral(" << (value ? "true" : "false") << ")\n";
}
std::unique_ptr<ASTNode> BooleanLiteral::clone() const {
    return std::make_unique<BooleanLiteral>(value);
}

// Identifier
Identifier::Identifier(const std::string& name) : name(name) {}
void Identifier::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Identifier(" << name << ")\n";
}
std::unique_ptr<ASTNode> Identifier::clone() const {
    return std::make_unique<Identifier>(name);
}

// BinaryOperation
BinaryOperation::BinaryOperation(std::unique_ptr<Expression> left, const std::string& op, std::unique_ptr<Expression> right)
    : left(std::move(left)), op(op), right(std::move(right)) {}
void BinaryOperation::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BinaryOperation(" << op << ")\n";
    left->print(indent + 2);
    right->print(indent + 2);
}
std::unique_ptr<ASTNode> BinaryOperation::clone() const {
    return std::make_unique<BinaryOperation>(
        std::unique_ptr<Expression>(static_cast<Expression*>(left->clone().release())),
        op,
        std::unique_ptr<Expression>(static_cast<Expression*>(right->clone().release()))
    );
}

// UnaryOperation
UnaryOperation::UnaryOperation(const std::string& op, std::unique_ptr<Expression> operand)
    : op(op), operand(std::move(operand)) {}
void UnaryOperation::print(int indent) const {
    std::cout << std::string(indent, ' ') << "UnaryOperation(" << op << ")\n";
    operand->print(indent + 2);
}
std::unique_ptr<ASTNode> UnaryOperation::clone() const {
    return std::make_unique<UnaryOperation>(
        op,
        std::unique_ptr<Expression>(static_cast<Expression*>(operand->clone().release()))
    );
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
std::unique_ptr<ASTNode> FunctionCall::clone() const {
    std::vector<std::unique_ptr<Expression>> clonedArgs;
    for (const auto& arg : arguments) {
        clonedArgs.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(arg->clone().release())));
    }
    return std::make_unique<FunctionCall>(functionName, std::move(clonedArgs));
}

// ExpressionStatement
ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression)
    : expression(std::move(expression)) {}
void ExpressionStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ExpressionStatement:\n";
    expression->print(indent + 2);
}
std::unique_ptr<ASTNode> ExpressionStatement::clone() const {
    return std::make_unique<ExpressionStatement>(
        std::unique_ptr<Expression>(static_cast<Expression*>(expression->clone().release()))
    );
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
std::unique_ptr<ASTNode> Block::clone() const {
    auto newBlock = std::make_unique<Block>();
    for (const auto& stmt : statements) {
        newBlock->addStatement(std::unique_ptr<Statement>(static_cast<Statement*>(stmt->clone().release())));
    }
    return newBlock;
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
std::unique_ptr<ASTNode> VariableDeclaration::clone() const {
    std::unique_ptr<Expression> clonedInitializer = nullptr;
    if (initializer) {
        clonedInitializer = std::unique_ptr<Expression>(static_cast<Expression*>(initializer->clone().release()));
    }
    return std::make_unique<VariableDeclaration>(variableName, std::move(clonedInitializer));
}

// Assignment
Assignment::Assignment(const std::string& variableName, std::unique_ptr<Expression> value)
    : variableName(variableName), value(std::move(value)), target(nullptr) {}

Assignment::Assignment(const std::string& variableName, std::unique_ptr<Expression> value, std::unique_ptr<Expression> target)
    : variableName(variableName), value(std::move(value)), target(std::move(target)) {}

void Assignment::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Assignment(" << variableName << ")\n";
    if (target) {
        std::cout << std::string(indent + 2, ' ') << "Target:\n";
        target->print(indent + 4);
    }
    std::cout << std::string(indent + 2, ' ') << "Value:\n";
    value->print(indent + 4);
}

std::unique_ptr<ASTNode> Assignment::clone() const {
    std::unique_ptr<Expression> clonedTarget = nullptr;
    if (target) {
        clonedTarget = std::unique_ptr<Expression>(static_cast<Expression*>(target->clone().release()));
    }
    
    return std::make_unique<Assignment>(
        variableName,
        std::unique_ptr<Expression>(static_cast<Expression*>(value->clone().release())),
        std::move(clonedTarget)
    );
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
std::unique_ptr<ASTNode> IfStatement::clone() const {
    std::unique_ptr<Block> clonedElseBlock = nullptr;
    if (elseBlock) {
        clonedElseBlock = std::unique_ptr<Block>(static_cast<Block*>(elseBlock->clone().release()));
    }
    return std::make_unique<IfStatement>(
        std::unique_ptr<Expression>(static_cast<Expression*>(condition->clone().release())),
        std::unique_ptr<Block>(static_cast<Block*>(thenBlock->clone().release())),
        std::move(clonedElseBlock)
    );
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
std::unique_ptr<ASTNode> WhileStatement::clone() const {
    return std::make_unique<WhileStatement>(
        std::unique_ptr<Expression>(static_cast<Expression*>(condition->clone().release())),
        std::unique_ptr<Block>(static_cast<Block*>(body->clone().release()))
    );
}

// ReturnStatement
ReturnStatement::ReturnStatement(std::unique_ptr<Expression> value) : value(std::move(value)) {}
void ReturnStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ReturnStatement:\n";
    if (value) {
        value->print(indent + 2);
    }
}
std::unique_ptr<ASTNode> ReturnStatement::clone() const {
    std::unique_ptr<Expression> clonedValue = nullptr;
    if (value) {
        clonedValue = std::unique_ptr<Expression>(static_cast<Expression*>(value->clone().release()));
    }
    return std::make_unique<ReturnStatement>(std::move(clonedValue));
}

// PrintStatement
PrintStatement::PrintStatement(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {}
void PrintStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "PrintStatement:\n";
    expression->print(indent + 2);
}
std::unique_ptr<ASTNode> PrintStatement::clone() const {
    return std::make_unique<PrintStatement>(
        std::unique_ptr<Expression>(static_cast<Expression*>(expression->clone().release()))
    );
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
std::unique_ptr<ASTNode> FunctionDeclaration::clone() const {
    return std::make_unique<FunctionDeclaration>(
        functionName,
        parameters,
        std::unique_ptr<Block>(static_cast<Block*>(body->clone().release()))
    );
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
std::unique_ptr<ASTNode> Program::clone() const {
    auto newProgram = std::make_unique<Program>();
    for (const auto& stmt : statements) {
        newProgram->addStatement(std::unique_ptr<Statement>(static_cast<Statement*>(stmt->clone().release())));
    }
    return newProgram;
}

// ArrayLiteral
ArrayLiteral::ArrayLiteral(std::vector<std::unique_ptr<Expression>> elements)
    : elements(std::move(elements)) {}
void ArrayLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ArrayLiteral:\n";
    for (const auto& element : elements) {
        element->print(indent + 2);
    }
}
std::unique_ptr<ASTNode> ArrayLiteral::clone() const {
    std::vector<std::unique_ptr<Expression>> clonedElements;
    for (const auto& element : elements) {
        clonedElements.push_back(std::unique_ptr<Expression>(
            static_cast<Expression*>(element->clone().release())));
    }
    return std::make_unique<ArrayLiteral>(std::move(clonedElements));
}

// IndexExpression
IndexExpression::IndexExpression(std::unique_ptr<Expression> object, std::unique_ptr<Expression> index)
    : object(std::move(object)), index(std::move(index)) {}
void IndexExpression::print(int indent) const {
    std::cout << std::string(indent, ' ') << "IndexExpression:\n";
    object->print(indent + 2);
    index->print(indent + 2);
}
std::unique_ptr<ASTNode> IndexExpression::clone() const {
    return std::make_unique<IndexExpression>(
        std::unique_ptr<Expression>(static_cast<Expression*>(object->clone().release())),
        std::unique_ptr<Expression>(static_cast<Expression*>(index->clone().release()))
    );
}

// ObjectLiteral
ObjectLiteral::ObjectLiteral(std::vector<std::pair<std::string, std::unique_ptr<Expression>>> properties)
    : properties(std::move(properties)) {}

void ObjectLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ObjectLiteral:\n";
    for (const auto& [key, value] : properties) {
        std::cout << std::string(indent + 2, ' ') << key << ":\n";
        value->print(indent + 4);
    }
}

std::unique_ptr<ASTNode> ObjectLiteral::clone() const {
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>> clonedProperties;
    for (const auto& [key, value] : properties) {
        clonedProperties.emplace_back(
            key,
            std::unique_ptr<Expression>(static_cast<Expression*>(value->clone().release()))
        );
    }
    return std::make_unique<ObjectLiteral>(std::move(clonedProperties));
}

// PropertyAccess
PropertyAccess::PropertyAccess(std::unique_ptr<Expression> object, const std::string& property)
    : object(std::move(object)), property(property) {}

void PropertyAccess::print(int indent) const {
    std::cout << std::string(indent, ' ') << "PropertyAccess: ." << property << "\n";
    object->print(indent + 2);
}

std::unique_ptr<ASTNode> PropertyAccess::clone() const {
    return std::make_unique<PropertyAccess>(
        std::unique_ptr<Expression>(static_cast<Expression*>(object->clone().release())),
        property
    );
}

// ForStatement
ForStatement::ForStatement(std::unique_ptr<Statement> initializer, 
                         std::unique_ptr<Expression> condition,
                         std::unique_ptr<Expression> increment,
                         std::unique_ptr<Block> body)
    : initializer(std::move(initializer)), condition(std::move(condition)),
      increment(std::move(increment)), body(std::move(body)) {}
void ForStatement::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ForStatement:\n";
    if (initializer) initializer->print(indent + 2);
    if (condition) condition->print(indent + 2);
    if (increment) increment->print(indent + 2);
    body->print(indent + 2);
}
std::unique_ptr<ASTNode> ForStatement::clone() const {
    return std::make_unique<ForStatement>(
        initializer ? std::unique_ptr<Statement>(static_cast<Statement*>(initializer->clone().release())) : nullptr,
        condition ? std::unique_ptr<Expression>(static_cast<Expression*>(condition->clone().release())) : nullptr,
        increment ? std::unique_ptr<Expression>(static_cast<Expression*>(increment->clone().release())) : nullptr,
        std::unique_ptr<Block>(static_cast<Block*>(body->clone().release()))
    );
}

// NullLiteral
NullLiteral::NullLiteral() {}
void NullLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "NullLiteral\n";
}
std::unique_ptr<ASTNode> NullLiteral::clone() const {
    return std::make_unique<NullLiteral>();
}