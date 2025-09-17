#include "environment.h"
#include <stdexcept>
#include <sstream>

// Value implementations
Value::Value() : type(NIL), numberValue(0), booleanValue(false) {}
Value::Value(double value) : type(NUMBER), numberValue(value), booleanValue(false) {}
Value::Value(const std::string& value) : type(STRING), stringValue(value), booleanValue(false) {}
Value::Value(bool value) : type(BOOLEAN), booleanValue(value), numberValue(0) {}
Value::Value(const std::vector<std::string>& params, std::unique_ptr<Block> body)
    : type(FUNCTION), parameters(params), body(std::move(body)), numberValue(0), booleanValue(false) {}

std::string Value::toString() const {
    switch (type) {
        case NUMBER: {
            std::ostringstream oss;
            oss << numberValue;
            return oss.str();
        }
        case STRING: return stringValue;
        case BOOLEAN: return booleanValue ? "true" : "false";
        case FUNCTION: return "<function>";
        case NIL: return "nil";
        default: return "unknown";
    }
}

// Environment implementations
Environment::Environment() : parent(nullptr) {}
Environment::Environment(std::shared_ptr<Environment> parent) : parent(parent) {}

void Environment::define(const std::string& name, const Value& value) {
    variables[name] = value;
}

Value& Environment::get(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    
    if (parent) {
        return parent->get(name);
    }
    
    throw std::runtime_error("Undefined variable: " + name);
}

void Environment::set(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second = value;
        return;
    }
    
    if (parent) {
        parent->set(name, value);
        return;
    }
    
    throw std::runtime_error("Undefined variable: " + name);
}

bool Environment::exists(const std::string& name) const {
    if (variables.find(name) != variables.end()) {
        return true;
    }
    
    if (parent) {
        return parent->exists(name);
    }
    
    return false;
}
