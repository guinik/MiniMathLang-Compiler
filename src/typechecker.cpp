#include "typechecker.h"
#include <iostream>

TypeChecker::TypeChecker() : has_errors(false) {}

bool TypeChecker::check(Program* program) {
    for (ASTNode* stmt : program->statements) {
        check_node(stmt);
    }
    return !has_errors;
}

Type TypeChecker::check_node(ASTNode* node) {
    if (!node) return Type::UNKNOWN;
    
    switch (node->node_type) {
        case NodeType::VAR_DECL: {
            VarDecl* decl = static_cast<VarDecl*>(node);
            Type init_type = check_node(decl->initializer);
            
            if (init_type != decl->var_type && init_type != Type::UNKNOWN) {
                error("Type mismatch in variable declaration '" + decl->name + 
                      "': expected " + type_to_string(decl->var_type) + 
                      ", got " + type_to_string(init_type));
            }
            
            symbol_table[decl->name] = decl->var_type;
            return decl->var_type;
        }
        
        case NodeType::PRINT_STMT: {
            PrintStmt* stmt = static_cast<PrintStmt*>(node);
            check_node(stmt->expr);
            return Type::UNKNOWN;
        }
        
        case NodeType::BINARY_OP: {
            BinaryOp* binop = static_cast<BinaryOp*>(node);
            Type left_type = check_node(binop->left);
            Type right_type = check_node(binop->right);
            
            Type result = infer_binary_op(binop->op, left_type, right_type);
            binop->type = result;
            return result;
        }
        
        case NodeType::LITERAL_INT:
            return Type::INT;
            
        case NodeType::LITERAL_FLOAT:
            return Type::FLOAT;
            
        case NodeType::LITERAL_VEC:
            return Type::VEC;
            
        case NodeType::IDENTIFIER: {
            Identifier* id = static_cast<Identifier*>(node);
            if (symbol_table.find(id->name) == symbol_table.end()) {
                error("Undefined variable '" + id->name + "'");
                return Type::UNKNOWN;
            }
            Type t = symbol_table[id->name];
            id->type = t;
            return t;
        }
        
        default:
            return Type::UNKNOWN;
    }
}

void TypeChecker::error(const std::string& message) {
    std::cerr << "Type error: " << message << std::endl;
    has_errors = true;
}

bool TypeChecker::is_numeric(Type t) {
    return t == Type::INT || t == Type::FLOAT;
}

Type TypeChecker::infer_binary_op(char op, Type left, Type right) {
    // Scalar + Scalar
    if (is_numeric(left) && is_numeric(right)) {
        if (left == Type::FLOAT || right == Type::FLOAT) {
            return Type::FLOAT;
        }
        return Type::INT;
    }
    
    // Vec + Vec
    if (left == Type::VEC && right == Type::VEC) {
        return Type::VEC;
    }
    
    // Vec + Scalar or Scalar + Vec
    if ((left == Type::VEC && is_numeric(right)) || 
        (is_numeric(left) && right == Type::VEC)) {
        return Type::VEC;
    }
    
    error("Invalid operand types for operator '" + std::string(1, op) + 
          "': " + type_to_string(left) + " and " + type_to_string(right));
    return Type::UNKNOWN;
}