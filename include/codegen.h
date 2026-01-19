#pragma once
#include "ast.h"
#include <string>
#include <sstream>

class CodeGen {
public:
    CodeGen();
    std::string generate(Program* program);

private:
    std::stringstream output;
    int temp_counter;

    void generate_statement(ASTNode* node);
    std::string generate_expression(ASTNode* node);

    std::string generate_binary_op(BinaryOp* node);
    std::string generate_literal_int(LiteralInt* node);
    std::string generate_literal_float(LiteralFloat* node);
    std::string generate_literal_vec(LiteralVec* node);
    std::string generate_identifier(Identifier* node);

    void generate_var_decl(VarDecl* node);
    void generate_print_stmt(PrintStmt* node);

    std::string new_temp();
    void emit_runtime();
};