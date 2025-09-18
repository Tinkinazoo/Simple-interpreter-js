#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "environment.h"
#include <memory>

class Interpreter {
private:
    std::shared_ptr<Environment> globalEnv;
    std::shared_ptr<Environment> currentEnv;
    
    Value evaluateExpression(const Expression& expr);
    void executeStatement(const Statement& stmt);
    void executeBlock(const Block& block, std::shared_ptr<Environment> env);
    void evaluateTargetAssignment(const Expression& target, const Value& value); // НОВЫЙ МЕТОД
    void evaluateArrayAssignment(const Expression& target, const Value& value);
public:
    Interpreter();
    void interpret(const Program& program);
    void setGlobal(const std::string& name, const Value& value);
};

#endif // INTERPRETER_H