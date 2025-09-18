#include "interpreter.h"
#include <iostream>
#include <stdexcept>

// Класс для передачи значения return
class ReturnValue {
public:
    Value value;
    ReturnValue(const Value& val) : value(val) {}
};

Interpreter::Interpreter() {
    globalEnv = std::make_shared<Environment>();
    currentEnv = globalEnv;
}

void Interpreter::interpret(const Program& program) {
    try {
        for (const auto& stmt : program.statements) {
            executeStatement(*stmt);
        }
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

Value Interpreter::evaluateExpression(const Expression& expr) {
    if (auto num = dynamic_cast<const NumberLiteral*>(&expr)) {
        return Value(num->value);
    }
    else if (auto str = dynamic_cast<const StringLiteral*>(&expr)) {
        return Value(str->value);
    }
    else if (auto boolean = dynamic_cast<const BooleanLiteral*>(&expr)) {
        return Value(boolean->value);
    }
    else if (auto id = dynamic_cast<const Identifier*>(&expr)) {
        return currentEnv->get(id->name);
    }
    else if (auto binOp = dynamic_cast<const BinaryOperation*>(&expr)) {
        Value left = evaluateExpression(*binOp->left);
        Value right = evaluateExpression(*binOp->right);
        
        if (binOp->op == "+") {
            if (left.type == Value::NUMBER && right.type == Value::NUMBER) {
                return Value(left.numberValue + right.numberValue);
            } else if (left.type == Value::STRING || right.type == Value::STRING) {
                return Value(left.toString() + right.toString());
            }
        }
        else if (binOp->op == "-") {
            return Value(left.numberValue - right.numberValue);
        }
        else if (binOp->op == "*") {
            return Value(left.numberValue * right.numberValue);
        }
        else if (binOp->op == "/") {
            if (right.numberValue == 0) {
                throw std::runtime_error("Division by zero");
            }
            return Value(left.numberValue / right.numberValue);
        }
        else if (binOp->op == "==") {
            return Value(left.toString() == right.toString());
        }
        else if (binOp->op == "!=") {
            return Value(left.toString() != right.toString());
        }
        else if (binOp->op == "<") {
            return Value(left.numberValue < right.numberValue);
        }
        else if (binOp->op == ">") {
            return Value(left.numberValue > right.numberValue);
        }
        else if (binOp->op == "<=") {
            return Value(left.numberValue <= right.numberValue);
        }
        else if (binOp->op == ">=") {
            return Value(left.numberValue >= right.numberValue);
        }
        else if (binOp->op == "and") {
            return Value(left.booleanValue && right.booleanValue);
        }
        else if (binOp->op == "or") {
            return Value(left.booleanValue || right.booleanValue);
        }
    }
    else if (auto unOp = dynamic_cast<const UnaryOperation*>(&expr)) {
        Value operand = evaluateExpression(*unOp->operand);
        
        if (unOp->op == "not") {
            return Value(!operand.booleanValue);
        }
        else if (unOp->op == "-") {
            return Value(-operand.numberValue);
        }
    }
    else if (auto call = dynamic_cast<const FunctionCall*>(&expr)) {
        if (call->functionName == "print") {
            for (const auto& arg : call->arguments) {
                Value value = evaluateExpression(*arg);
                std::cout << value.toString() << " ";
            }
            std::cout << std::endl;
            return Value();
        }
        
        Value func = currentEnv->get(call->functionName);
        if (func.type != Value::FUNCTION) {
            throw std::runtime_error("Not a function: " + call->functionName);
        }
        
        if (call->arguments.size() != func.parameters.size()) {
            throw std::runtime_error("Wrong number of arguments for function: " + call->functionName);
        }
        
        auto funcEnv = std::make_shared<Environment>(currentEnv);
        for (size_t i = 0; i < call->arguments.size(); i++) {
            Value argValue = evaluateExpression(*call->arguments[i]);
            funcEnv->define(func.parameters[i], argValue);
        }
        
        auto oldEnv = currentEnv;
        currentEnv = funcEnv;
        
        try {
            executeBlock(*func.body, funcEnv);
        } catch (const ReturnValue& returnValue) {
            currentEnv = oldEnv;
            return returnValue.value;
        } catch (const ReturnStatement&) {
            currentEnv = oldEnv;
            return Value();
        }
        
        currentEnv = oldEnv;
        return Value();
    }
    
    return Value();
}

void Interpreter::executeStatement(const Statement& stmt) {
    if (auto varDecl = dynamic_cast<const VariableDeclaration*>(&stmt)) {
        Value value = varDecl->initializer ? evaluateExpression(*varDecl->initializer) : Value();
        currentEnv->define(varDecl->variableName, value);
    }
    else if (auto assignment = dynamic_cast<const Assignment*>(&stmt)) {
        Value value = evaluateExpression(*assignment->value);
        currentEnv->set(assignment->variableName, value);
    }
    else if (auto ifStmt = dynamic_cast<const IfStatement*>(&stmt)) {
        Value condition = evaluateExpression(*ifStmt->condition);
        if (condition.booleanValue) {
            executeBlock(*ifStmt->thenBlock, currentEnv);
        } else if (ifStmt->elseBlock) {
            executeBlock(*ifStmt->elseBlock, currentEnv);
        }
    }
    else if (auto whileStmt = dynamic_cast<const WhileStatement*>(&stmt)) {
        while (true) {
            Value condition = evaluateExpression(*whileStmt->condition);
            if (!condition.booleanValue) break;
            executeBlock(*whileStmt->body, currentEnv);
        }
    }
    else if (auto printStmt = dynamic_cast<const PrintStatement*>(&stmt)) {
        Value value = evaluateExpression(*printStmt->expression);
        std::cout << value.toString() << std::endl;
    }
    else if (auto returnStmt = dynamic_cast<const ReturnStatement*>(&stmt)) {
        Value returnValue = returnStmt->value ? evaluateExpression(*returnStmt->value) : Value();
        throw ReturnValue(returnValue);
    }
    else if (auto funcDecl = dynamic_cast<const FunctionDeclaration*>(&stmt)) {
        // Используем shared_ptr вместо unique_ptr
        Value funcValue(funcDecl->parameters, std::shared_ptr<Block>(funcDecl->body.get()));
        currentEnv->define(funcDecl->functionName, funcValue);
    }
    else if (auto block = dynamic_cast<const Block*>(&stmt)) {
        executeBlock(*block, currentEnv);
    }
    else if (auto exprStmt = dynamic_cast<const ExpressionStatement*>(&stmt)) {
        evaluateExpression(*exprStmt->expression);
    }
}

void Interpreter::executeBlock(const Block& block, std::shared_ptr<Environment> env) {
    auto oldEnv = currentEnv;
    currentEnv = env;
    
    for (const auto& stmt : block.statements) {
        try {
            executeStatement(*stmt);
        } catch (const ReturnValue&) {
            // Пробрасываем ReturnValue наверх
            currentEnv = oldEnv;
            throw;
        } catch (const ReturnStatement&) {
            // Для обратной совместимости
            currentEnv = oldEnv;
            throw;
        }
    }
    
    currentEnv = oldEnv;
}

void Interpreter::setGlobal(const std::string& name, const Value& value) {
    globalEnv->define(name, value);
}