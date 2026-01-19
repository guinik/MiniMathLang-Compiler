#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) 
    : source(source), pos(0), line(1), column(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (pos < source.length()) {
        skip_whitespace();
        
        if (pos >= source.length()) break;
        
        if (source[pos] == '/' && peek() == '/') {
            skip_comment();
            continue;
        }
        
        char c = current_char();
        
        if (is_digit(c)) {
            tokens.push_back(read_number());
        } else if (is_alpha(c)) {
            tokens.push_back(read_identifier());
        } else {
            int col = column;
            switch (c) {
                case '+': tokens.push_back(Token(TokenType::PLUS, "+", line, col)); advance(); break;
                case '-': tokens.push_back(Token(TokenType::MINUS, "-", line, col)); advance(); break;
                case '*': tokens.push_back(Token(TokenType::STAR, "*", line, col)); advance(); break;
                case '/': tokens.push_back(Token(TokenType::SLASH, "/", line, col)); advance(); break;
                case '=': tokens.push_back(Token(TokenType::ASSIGN, "=", line, col)); advance(); break;
                case ':': tokens.push_back(Token(TokenType::COLON, ":", line, col)); advance(); break;
                case ',': tokens.push_back(Token(TokenType::COMMA, ",", line, col)); advance(); break;
                case '(': tokens.push_back(Token(TokenType::LPAREN, "(", line, col)); advance(); break;
                case ')': tokens.push_back(Token(TokenType::RPAREN, ")", line, col)); advance(); break;
                case '[': tokens.push_back(Token(TokenType::LBRACKET, "[", line, col)); advance(); break;
                case ']': tokens.push_back(Token(TokenType::RBRACKET, "]", line, col)); advance(); break;
                default:
                    tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, c), line, col));
                    advance();
            }
        }
    }
    
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
    return tokens;
}

char Lexer::current_char() {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::peek() {
    if (pos + 1 >= source.length()) return '\0';
    return source[pos + 1];
}

void Lexer::advance() {
    if (pos < source.length()) {
        if (source[pos] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
    }
}

void Lexer::skip_whitespace() {
    while (pos < source.length() && isspace(current_char())) {
        advance();
    }
}

void Lexer::skip_comment() {
    while (pos < source.length() && current_char() != '\n') {
        advance();
    }
}

Token Lexer::read_number() {
    int col = column;
    std::string num;
    bool is_float = false;
    
    while (is_digit(current_char())) {
        num += current_char();
        advance();
    }
    
    if (current_char() == '.') {
        is_float = true;
        num += current_char();
        advance();
        
        while (is_digit(current_char())) {
            num += current_char();
            advance();
        }
    }
    
    TokenType type = is_float ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL;
    return Token(type, num, line, col);
}

Token Lexer::read_identifier() {
    int col = column;
    std::string id;
    
    while (is_alnum(current_char())) {
        id += current_char();
        advance();
    }
    
    TokenType type;
    if (id == "let") type = TokenType::LET;
    else if (id == "print") type = TokenType::PRINT;
    else if (id == "int") type = TokenType::TYPE_INT;
    else if (id == "float") type = TokenType::TYPE_FLOAT;
    else if (id == "vec") type = TokenType::TYPE_VEC;
    else type = TokenType::IDENTIFIER;
    
    return Token(type, id, line, col);
}

bool Lexer::is_digit(char c) {
    return isdigit(c);
}

bool Lexer::is_alpha(char c) {
    return isalpha(c) || c == '_';
}

bool Lexer::is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}