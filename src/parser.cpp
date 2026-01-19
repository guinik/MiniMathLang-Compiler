#include "parser.h"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens, Arena& arena)
    : tokens(tokens), pos(0), arena(arena) {}

Program* Parser::parse() {
    Program* program = allocate<Program>();
    
    while (current().type != TokenType::END_OF_FILE) {
        program->statements.push_back(parse_statement());
    }
    
    return program;
}

Token Parser::current() {
    if (pos >= tokens.size()) return tokens.back();
    return tokens[pos];
}

Token Parser::peek() {
    if (pos + 1 >= tokens.size()) return tokens.back();
    return tokens[pos + 1];
}

void Parser::advance() {
    if (pos < tokens.size()) pos++;
}

bool Parser::match(TokenType type) {
    return current().type == type;
}

Token Parser::expect(TokenType type) {
    Token tok = current();
    if (tok.type != type) {
        throw std::runtime_error("Parse error at line " + std::to_string(tok.line) + 
                                 ": unexpected token '" + tok.value + "'");
    }
    advance();
    return tok;
}

ASTNode* Parser::parse_statement() {
    if (match(TokenType::LET)) {
        return parse_var_decl();
    } else if (match(TokenType::PRINT)) {
        return parse_print_stmt();
    } else {
        throw std::runtime_error("Parse error: expected statement");
    }
}

ASTNode* Parser::parse_var_decl() {
    expect(TokenType::LET);
    Token name = expect(TokenType::IDENTIFIER);
    expect(TokenType::COLON);
    Type var_type = parse_type();
    expect(TokenType::ASSIGN);
    ASTNode* init = parse_expression();
    
    return allocate<VarDecl>(name.value, var_type, init);
}

ASTNode* Parser::parse_print_stmt() {
    expect(TokenType::PRINT);
    expect(TokenType::LPAREN);
    ASTNode* expr = parse_expression();
    expect(TokenType::RPAREN);
    
    return allocate<PrintStmt>(expr);
}

ASTNode* Parser::parse_expression() {
    return parse_term();
}

ASTNode* Parser::parse_term() {
    ASTNode* left = parse_factor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        char op = current().value[0];
        advance();
        ASTNode* right = parse_factor();
        left = allocate<BinaryOp>(op, left, right);
    }
    
    return left;
}

ASTNode* Parser::parse_factor() {
    ASTNode* left = parse_primary();
    
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        char op = current().value[0];
        advance();
        ASTNode* right = parse_primary();
        left = allocate<BinaryOp>(op, left, right);
    }
    
    return left;
}

ASTNode* Parser::parse_primary() {
    if (match(TokenType::INT_LITERAL)) {
        int value = std::stoi(current().value);
        advance();
        return allocate<LiteralInt>(value);
    }
    
    if (match(TokenType::FLOAT_LITERAL)) {
        float value = std::stof(current().value);
        advance();
        return allocate<LiteralFloat>(value);
    }
    
    if (match(TokenType::LBRACKET)) {
        advance();
        std::vector<float> values;
        
        if (!match(TokenType::RBRACKET)) {
            do {
                if (match(TokenType::COMMA)) advance();
                Token tok = current();
                if (tok.type == TokenType::FLOAT_LITERAL) {
                    values.push_back(std::stof(tok.value));
                } else if (tok.type == TokenType::INT_LITERAL) {
                    values.push_back(static_cast<float>(std::stoi(tok.value)));
                } else {
                    throw std::runtime_error("Expected number in vector literal");
                }
                advance();
            } while (match(TokenType::COMMA));
        }
        
        expect(TokenType::RBRACKET);
        return allocate<LiteralVec>(values);
    }
    
    if (match(TokenType::IDENTIFIER)) {
        std::string name = current().value;
        advance();
        return allocate<Identifier>(name);
    }
    
    if (match(TokenType::LPAREN)) {
        advance();
        ASTNode* expr = parse_expression();
        expect(TokenType::RPAREN);
        return expr;
    }
    
    throw std::runtime_error("Parse error: unexpected token '" + current().value + "'");
}

Type Parser::parse_type() {
    if (match(TokenType::TYPE_INT)) {
        advance();
        return Type::INT;
    } else if (match(TokenType::TYPE_FLOAT)) {
        advance();
        return Type::FLOAT;
    } else if (match(TokenType::TYPE_VEC)) {
        advance();
        return Type::VEC;
    } else {
        throw std::runtime_error("Parse error: expected type");
    }
}

// Arena allocator implementation
Arena::Arena(size_t block_size) 
    : block_size(block_size), current_block(nullptr), offset(0) {}

Arena::~Arena() {
    for (char* block : blocks) {
        delete[] block;
    }
}

void* Arena::allocate(size_t size) {
    if (!current_block || offset + size > block_size) {
        current_block = new char[block_size];
        blocks.push_back(current_block);
        offset = 0;
    }
    
    void* ptr = current_block + offset;
    offset += size;
    return ptr;
}

void Arena::reset() {
    offset = 0;
    if (!blocks.empty()) {
        current_block = blocks[0];
    }
}

std::string type_to_string(Type t) {
    switch (t) {
        case Type::INT: return "int";
        case Type::FLOAT: return "float";
        case Type::VEC: return "vec";
        default: return "unknown";
    }
}