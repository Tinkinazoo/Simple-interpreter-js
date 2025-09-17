#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void runRepl() {
    Interpreter interpreter;
    std::string line;
    
    std::cout << "Interpreter REPL. Type 'exit' to quit.\n";
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        if (line == "exit" || line == "quit") {
            break;
        }
        
        if (line.empty()) {
            continue;
        }
        
        try {
            Lexer lexer(line);
            Parser parser(lexer);
            auto program = parser.parse();
            
            interpreter.interpret(*program);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        runRepl();
    } else if (argc == 2) {
        try {
            std::string source = readFile(argv[1]);
            Lexer lexer(source);
            Parser parser(lexer);
            auto program = parser.parse();
            
            Interpreter interpreter;
            interpreter.interpret(*program);
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl;
        return 1;
    }
    
    return 0;
}
