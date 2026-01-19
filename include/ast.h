#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>

// Arena allocator for efficient memory management
class Arena {
    public:
        Arena(size_t block_size = 4096);
        ~Arena();
    
        void* allocate(size_t size);
        void reset();
    
    private:
        size_t block_size;
        std::vector<char*> blocks;
        char* current_block;
        size_t offset;
};

// Type system
enum class Type {
    INT,
    FLOAT,
    VEC,
    UNKNOWN
};

std::string type_to_string(Type t);

// AST Node types
enum class NodeType {
    PROGRAM,
    VAR_DECL,
    BINARY_OP,
    LITERAL_INT,
    LITERAL_FLOAT,
    LITERAL_VEC,
    IDENTIFIER,
    PRINT_STMT
};

// Base AST Node
struct ASTNode {
    NodeType node_type;
    Type type;
    
    ASTNode(NodeType nt) : node_type(nt), type(Type::UNKNOWN) {}
    virtual ~ASTNode() = default;
};

// Literal nodes
struct LiteralInt : ASTNode {
    int value;
    LiteralInt(int v) : ASTNode(NodeType::LITERAL_INT), value(v) {
        type = Type::INT;
    }
};

struct LiteralFloat : ASTNode {
    float value;
    LiteralFloat(float v) : ASTNode(NodeType::LITERAL_FLOAT), value(v) {
        type = Type::FLOAT;
    }
};

struct LiteralVec : ASTNode {
    std::vector<float> values;
    LiteralVec(std::vector<float> v) : ASTNode(NodeType::LITERAL_VEC), values(v) {
        type = Type::VEC;
    }
};

struct Identifier : ASTNode {
    std::string name;
    Identifier(std::string n) : ASTNode(NodeType::IDENTIFIER), name(n) {}
};

struct BinaryOp : ASTNode {
    char op; // '+', '-', '*', '/' are the only operation supported right now 
    ASTNode* left;
    ASTNode* right;
    
    BinaryOp(char o, ASTNode* l, ASTNode* r) 
        : ASTNode(NodeType::BINARY_OP), op(o), left(l), right(r) {}
};

struct VarDecl : ASTNode {
    std::string name;
    Type var_type;
    ASTNode* initializer;
    
    VarDecl(std::string n, Type t, ASTNode* init)
        : ASTNode(NodeType::VAR_DECL), name(n), var_type(t), initializer(init) {}
};

struct PrintStmt : ASTNode {
    ASTNode* expr;
    
    PrintStmt(ASTNode* e) : ASTNode(NodeType::PRINT_STMT), expr(e) {}
};

struct Program : ASTNode {
    std::vector<ASTNode*> statements;
    
    Program() : ASTNode(NodeType::PROGRAM) {}
};