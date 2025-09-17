#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "ast.h"

class Value {
public:
    enum Type { NUMBER, STRING, BOOLEAN, FUNCTION, NIL };
    
    Type type;
    double numberValue;
    std::string stringValue;
    bool booleanValue;
    
    // Для функций
    std::vector<std::string> parameters;
    std::unique_ptr<Block> body;
    
    Value();
    Value(double value);
    Value(const std::string& value);
    Value(bool value);
    Value(const std::vector<std::string>& params, std::unique_ptr<Block> body);
    
    std::string toString() const;
};

class Environment {
public:
    std::unordered_map<std::string, Value> variables;
    std::shared_ptr<Environment> parent;
    
    Environment();
    Environment(std::shared_ptr<Environment> parent);
    
    void define(const std::string& name, const Value& value);
    Value& get(const std::string& name);
    void set(const std::string& name, const Value& value);
    bool exists(const std::string& name) const;
};

#endif // ENVIRONMENT_H
