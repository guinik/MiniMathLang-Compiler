#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    LET,
    PRINT,
    
    // Types
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VEC,
    
    // Literals
    INT_LITERAL,
    FLOAT_LITERAL,
    
    // Identifiers
    IDENTIFIER,
    
    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    ASSIGN,
    COLON,
    COMMA,
    
    // Delimiters
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    
    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, std::string v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    
    char current_char();
    char peek();
    void advance();
    void skip_whitespace();
    void skip_comment();
    
    Token read_number();
    Token read_identifier();
    
    bool is_digit(char c);
    bool is_alpha(char c);
    bool is_alnum(char c);
};