#pragma once
#include "ast.h"
#include <unordered_map>
#include <string>

class TypeChecker {
public:
    TypeChecker();
    bool check(Program* program);

private:
    std::unordered_map<std::string, Type> symbol_table;
    bool has_errors;

    Type check_node(ASTNode* node);
    void error(const std::string& message);

    bool is_numeric(Type t);
    Type infer_binary_op(char op, Type left, Type right);
};