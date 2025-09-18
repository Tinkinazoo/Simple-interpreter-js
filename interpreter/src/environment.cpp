#include "environment.h"
#include "ast.h" // Теперь подключаем здесь, где Value уже объявлен
#include <stdexcept>
#include <sstream>

// Value implementations
Value::Value() : type(NIL), numberValue(0), booleanValue(false), 
                arrayValue(nullptr), objectValue(nullptr) {}

Value::Value(double value) : type(NUMBER), numberValue(value), booleanValue(false),
                           arrayValue(nullptr), objectValue(nullptr) {}

Value::Value(const std::string& value) : type(STRING), stringValue(value), booleanValue(false),
                                       arrayValue(nullptr), objectValue(nullptr) {}

Value::Value(bool value) : type(BOOLEAN), booleanValue(value), numberValue(0),
                         arrayValue(nullptr), objectValue(nullptr) {}

Value::Value(const std::vector<std::string>& params, std::shared_ptr<Block> body)
    : type(FUNCTION), parameters(params), body(body), numberValue(0), booleanValue(false),
      arrayValue(nullptr), objectValue(nullptr) {}

Value::Value(const std::vector<Value>& array) 
    : type(ARRAY), numberValue(0), booleanValue(false), objectValue(nullptr) {
    arrayValue = std::make_unique<std::vector<Value>>(array);
}

Value::Value(const std::unordered_map<std::string, Value>& object)
    : type(OBJECT), numberValue(0), booleanValue(false), arrayValue(nullptr) {
    objectValue = std::make_unique<std::unordered_map<std::string, Value>>(object);
}

// Rule of Five implementations
Value::~Value() = default;

Value::Value(const Value& other)
    : type(other.type),
      numberValue(other.numberValue),
      stringValue(other.stringValue),
      booleanValue(other.booleanValue),
      parameters(other.parameters),
      body(other.body) {
    if (other.arrayValue) {
        arrayValue = std::make_unique<std::vector<Value>>(*other.arrayValue);
    }
    if (other.objectValue) {
        objectValue = std::make_unique<std::unordered_map<std::string, Value>>(*other.objectValue);
    }
}

Value::Value(Value&& other) noexcept
    : type(other.type),
      numberValue(other.numberValue),
      stringValue(std::move(other.stringValue)),
      booleanValue(other.booleanValue),
      parameters(std::move(other.parameters)),
      body(std::move(other.body)),
      arrayValue(std::move(other.arrayValue)),
      objectValue(std::move(other.objectValue)) {
    other.type = NIL;
    other.numberValue = 0;
    other.booleanValue = false;
}

Value& Value::operator=(const Value& other) {
    if (this != &other) {
        type = other.type;
        numberValue = other.numberValue;
        stringValue = other.stringValue;
        booleanValue = other.booleanValue;
        parameters = other.parameters;
        body = other.body;
        
        if (other.arrayValue) {
            arrayValue = std::make_unique<std::vector<Value>>(*other.arrayValue);
        } else {
            arrayValue.reset();
        }
        
        if (other.objectValue) {
            objectValue = std::make_unique<std::unordered_map<std::string, Value>>(*other.objectValue);
        } else {
            objectValue.reset();
        }
    }
    return *this;
}

Value& Value::operator=(Value&& other) noexcept {
    if (this != &other) {
        type = other.type;
        numberValue = other.numberValue;
        stringValue = std::move(other.stringValue);
        booleanValue = other.booleanValue;
        parameters = std::move(other.parameters);
        body = std::move(other.body);
        arrayValue = std::move(other.arrayValue);
        objectValue = std::move(other.objectValue);
        
        other.type = NIL;
        other.numberValue = 0;
        other.booleanValue = false;
    }
    return *this;
}

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
        case NIL: return "null";
        case ARRAY: {
            if (!arrayValue) return "[]";
            std::string result = "[";
            for (size_t i = 0; i < arrayValue->size(); i++) {
                if (i > 0) result += ", ";
                result += (*arrayValue)[i].toString();
            }
            return result + "]";
        }
        case OBJECT: {
            if (!objectValue) return "{}";
            std::string result = "{";
            bool first = true;
            for (const auto& [key, value] : *objectValue) {
                if (!first) result += ", ";
                result += key + ": " + value.toString();
                first = false;
            }
            return result + "}";
        }
        default: return "unknown";
    }
}

// Environment implementations (остаются без изменений)
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