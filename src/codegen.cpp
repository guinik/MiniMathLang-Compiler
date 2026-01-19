#include "codegen.h"
#include <iostream>

CodeGen::CodeGen() : temp_counter(0) {}

std::string CodeGen::generate(Program* program) {
    output.str("");

    emit_runtime();

    output << "\nint main() {\n";
    output << "    Arena arena(4096);\n\n";

    for (ASTNode* stmt : program->statements) {
        generate_statement(stmt);
    }

    output << "\n    return 0;\n";
    output << "}\n";

    return output.str();
}

void CodeGen::emit_runtime() {
    output << "#include <iostream>\n";
    output << "#include <vector>\n";
    output << "#include <cstring>\n\n";

    output << "// Arena allocator\n";
    output << "class Arena {\n";
    output << "public:\n";
    output << "    Arena(size_t size) : size(size), offset(0) {\n";
    output << "        buffer = new char[size];\n";
    output << "    }\n";
    output << "    ~Arena() { delete[] buffer; }\n";
    output << "    void* allocate(size_t n) {\n";
    output << "        if (offset + n > size) throw std::bad_alloc();\n";
    output << "        void* ptr = buffer + offset;\n";
    output << "        offset += n;\n";
    output << "        return ptr;\n";
    output << "    }\n";
    output << "private:\n";
    output << "    char* buffer;\n";
    output << "    size_t size;\n";
    output << "    size_t offset;\n";
    output << "};\n\n";

    output << "// Vector type\n";
    output << "struct Vec {\n";
    output << "    float* data;\n";
    output << "    size_t size;\n";
    output << "    Vec(Arena& arena, size_t s) : size(s) {\n";
    output << "        data = (float*)arena.allocate(s * sizeof(float));\n";
    output << "    }\n";
    output << "    float& operator[](size_t i) { return data[i]; }\n";
    output << "};\n\n";

    output << "Vec vec_add(Arena& arena, const Vec& a, const Vec& b) {\n";
    output << "    Vec result(arena, a.size);\n";
    output << "    for (size_t i = 0; i < a.size; i++)\n";
    output << "        result[i] = a.data[i] + b.data[i];\n";
    output << "    return result;\n";
    output << "}\n\n";

    output << "Vec vec_sub(Arena& arena, const Vec& a, const Vec& b) {\n";
    output << "    Vec result(arena, a.size);\n";
    output << "    for (size_t i = 0; i < a.size; i++)\n";
    output << "        result[i] = a.data[i] - b.data[i];\n";
    output << "    return result;\n";
    output << "}\n\n";

    output << "Vec vec_mul(Arena& arena, const Vec& a, const Vec& b) {\n";
    output << "    Vec result(arena, a.size);\n";
    output << "    for (size_t i = 0; i < a.size; i++)\n";
    output << "        result[i] = a.data[i] * b.data[i];\n";
    output << "    return result;\n";
    output << "}\n\n";

    output << "Vec vec_div(Arena& arena, const Vec& a, const Vec& b) {\n";
    output << "    Vec result(arena, a.size);\n";
    output << "    for (size_t i = 0; i < a.size; i++)\n";
    output << "        result[i] = a.data[i] / b.data[i];\n";
    output << "    return result;\n";
    output << "}\n\n";

    output << "Vec vec_scalar_add(Arena& arena, const Vec& v, float s) {\n";
    output << "    Vec result(arena, v.size);\n";
    output << "    for (size_t i = 0; i < v.size; i++)\n";
    output << "        result[i] = v.data[i] + s;\n";
    output << "    return result;\n";
    output << "}\n\n";

    output << "Vec vec_scalar_mul(Arena& arena, const Vec& v, float s) {\n";
    output << "    Vec result(arena, v.size);\n";
    output << "    for (size_t i = 0; i < v.size; i++)\n";
    output << "        result[i] = v.data[i] * s;\n";
    output << "    return result;\n";
    output << "}\n\n";

    output << "void print_vec(const Vec& v) {\n";
    output << "    std::cout << \"[\";\n";
    output << "    for (size_t i = 0; i < v.size; i++) {\n";
    output << "        std::cout << v.data[i];\n";
    output << "        if (i < v.size - 1) std::cout << \", \";\n";
    output << "    }\n";
    output << "    std::cout << \"]\" << std::endl;\n";
    output << "}\n";
}

void CodeGen::generate_statement(ASTNode* node) {
    switch (node->node_type) {
    case NodeType::VAR_DECL:
        generate_var_decl(static_cast<VarDecl*>(node));
        break;
    case NodeType::PRINT_STMT:
        generate_print_stmt(static_cast<PrintStmt*>(node));
        break;
    default:
        break;
    }
}

std::string CodeGen::generate_expression(ASTNode* node) {
    switch (node->node_type) {
    case NodeType::BINARY_OP:
        return generate_binary_op(static_cast<BinaryOp*>(node));
    case NodeType::LITERAL_INT:
        return generate_literal_int(static_cast<LiteralInt*>(node));
    case NodeType::LITERAL_FLOAT:
        return generate_literal_float(static_cast<LiteralFloat*>(node));
    case NodeType::LITERAL_VEC:
        return generate_literal_vec(static_cast<LiteralVec*>(node));
    case NodeType::IDENTIFIER:
        return generate_identifier(static_cast<Identifier*>(node));
    default:
        return "";
    }
}

std::string CodeGen::generate_binary_op(BinaryOp* node) {
    std::string left = generate_expression(node->left);
    std::string right = generate_expression(node->right);

    Type left_type = node->left->type;
    Type right_type = node->right->type;

    if (left_type == Type::VEC && right_type == Type::VEC) {
        std::string func;
        switch (node->op) {
        case '+': func = "vec_add"; break;
        case '-': func = "vec_sub"; break;
        case '*': func = "vec_mul"; break;
        case '/': func = "vec_div"; break;
        }
        return func + "(arena, " + left + ", " + right + ")";
    }

    if (left_type == Type::VEC || right_type == Type::VEC) {
        if (node->op == '*' || node->op == '+') {
            std::string vec_expr = (left_type == Type::VEC) ? left : right;
            std::string scalar_expr = (left_type == Type::VEC) ? right : left;
            std::string func = (node->op == '*') ? "vec_scalar_mul" : "vec_scalar_add";
            return func + "(arena, " + vec_expr + ", " + scalar_expr + ")";
        }
    }

    return "(" + left + " " + node->op + " " + right + ")";
}

std::string CodeGen::generate_literal_int(LiteralInt* node) {
    return std::to_string(node->value);
}

std::string CodeGen::generate_literal_float(LiteralFloat* node) {
    return std::to_string(node->value) + "f";
}

std::string CodeGen::generate_literal_vec(LiteralVec* node) {
    std::string temp = new_temp();
    output << "    Vec " << temp << "(arena, " << node->values.size() << ");\n";
    for (size_t i = 0; i < node->values.size(); i++) {
        output << "    " << temp << "[" << i << "] = " << std::to_string(node->values[i]) << "f;\n";
    }
    return temp;
}

std::string CodeGen::generate_identifier(Identifier* node) {
    return node->name;
}

void CodeGen::generate_var_decl(VarDecl* node) {
    std::string type_str;
    switch (node->var_type) {
    case Type::INT: type_str = "int"; break;
    case Type::FLOAT: type_str = "float"; break;
    case Type::VEC: type_str = "Vec"; break;
    default: type_str = "auto"; break;
    }

    std::string init = generate_expression(node->initializer);
    output << "    " << type_str << " " << node->name << " = " << init << ";\n";
}

void CodeGen::generate_print_stmt(PrintStmt* node) {
    std::string expr = generate_expression(node->expr);

    if (node->expr->type == Type::VEC) {
        output << "    print_vec(" << expr << ");\n";
    }
    else {
        output << "    std::cout << " << expr << " << std::endl;\n";
    }
}

std::string CodeGen::new_temp() {
    return "_t" + std::to_string(temp_counter++);
}