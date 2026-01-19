#include "lexer.h"
#include "parser.h"
#include "typechecker.h"
#include "codegen.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void write_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not write to file " << filename << std::endl;
        exit(1);
    }
    file << content;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source.mml>" << std::endl;
        return 1;
    }
    
    std::string source_file = argv[1];
    std::string source = read_file(source_file);
    
    std::cout << "=== Lexing ===" << std::endl;
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    std::cout << "\n=== Parsing ===" << std::endl;
    Arena arena;
    Parser parser(tokens, arena);
    Program* program = parser.parse();
    std::cout << "Parsed " << program->statements.size() << " statements" << std::endl;
    
    std::cout << "\n=== Type Checking ===" << std::endl;
    TypeChecker checker;
    if (!checker.check(program)) {
        std::cerr << "Type checking failed!" << std::endl;
        return 1;
    }
    std::cout << "Type checking passed" << std::endl;
    
    std::cout << "\n=== Code Generation ===" << std::endl;
    CodeGen codegen;
    std::string cpp_code = codegen.generate(program);
    
    std::string output_file = "output.cpp";
    write_file(output_file, cpp_code);
    std::cout << "Generated C++ code to " << output_file << std::endl;
    
    std::cout << "\n=== Compiling with g++ ===" << std::endl;
    std::string compile_cmd = "g++ -std=c++17 -o output " + output_file;
    int result = system(compile_cmd.c_str());
    
    if (result == 0) {
        std::cout << "Compilation successful! Run with: ./output" << std::endl;
    } else {
        std::cerr << "Compilation failed!" << std::endl;
        return 1;
    }
    
    return 0;
}