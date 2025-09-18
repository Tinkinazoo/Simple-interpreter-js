#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "ast.h"

// Forward declarations
class Block;
class Expression;

class Value {
public:
    enum Type { NUMBER, STRING, BOOLEAN, FUNCTION, NIL, ARRAY, OBJECT };
    
    Type type;
    double numberValue;
    std::string stringValue;
    bool booleanValue;
    
    // Для функций - используем shared_ptr вместо unique_ptr
    std::vector<std::string> parameters;
    std::shared_ptr<Block> body;
    
    // Для массивов и объектов
    std::unique_ptr<std::vector<Value>> arrayValue;
    std::unique_ptr<std::unordered_map<std::string, Value>> objectValue;

    // Конструкторы
    Value();
    Value(double value);
    Value(const std::string& value);
    Value(bool value);
    Value(const std::vector<std::string>& params, std::shared_ptr<Block> body);
    Value(const std::vector<Value>& array); // НОВЫЙ
    Value(const std::unordered_map<std::string, Value>& object); // НОВЫЙ
    
    // Правило пяти (Rule of Five)
    ~Value();
    Value(const Value& other);
    Value(Value&& other) noexcept;
    Value& operator=(const Value& other);
    Value& operator=(Value&& other) noexcept;

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