#pragma once
#include "lexer.h"
#include "ast.h"
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens, Arena& arena);
    Program* parse();
    
private:
    std::vector<Token> tokens;
    size_t pos;
    Arena& arena;
    
    Token current();
    Token peek();
    void advance();
    bool match(TokenType type);
    Token expect(TokenType type);
    
    ASTNode* parse_statement();
    ASTNode* parse_var_decl();
    ASTNode* parse_print_stmt();
    ASTNode* parse_expression();
    ASTNode* parse_term();
    ASTNode* parse_factor();
    ASTNode* parse_primary();
    
    Type parse_type();
    
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        void* mem = arena.allocate(sizeof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }
};