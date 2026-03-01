#pragma once

#include <string>
#include <vector>
#include <optional>
#include <stdexcept>

namespace mmrsl {

// Token types
enum class TokenType {
    // End of file
    EndOfFile,
    
    // Types
    Int, Float, Bool, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4, Void,
    
    // Keywords
    Return,
    If,
    Else,
    
    // Built-in functions
    Floor, Sin, Cos, Tan, Abs, Sqrt, Min, Max, Dot, Cross,
    Length, Normalize, Reflect, Refract,
    
    // Literals
    IntLiteral, FloatLiteral, BoolLiteral,
    
    // Identifiers
    Identifier,
    
    // Operators
    Plus, Minus, Multiply, Divide, Modulo,
    Assign,
    
    // Comparison operators
    Greater,        // >
    GreaterEqual,   // >=
    Less,           // <
    LessEqual,      // <=
    Equal,          // ==
    NotEqual,       // !=
    
    // Logical operators
    And,            // &&
    Or,             // ||
    Not,            // !
    
    // Delimiters
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    LeftBracket, RightBracket,
    Comma, Semicolon,
    Period,
    
    // Ternary operator
    Question,       // ?
    Colon,          // :
    
    // Special
    Unknown
};

// Convert token type to string for debugging
std::string tokenTypeToString(TokenType type);

// Source location for error reporting
struct SourceLocation {
    size_t line = 1;
    size_t column = 1;
    
    std::string toString() const {
        return std::to_string(line) + ":" + std::to_string(column);
    }
};

// Token structure
struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
    
    // For literals
    std::optional<int> intValue;
    std::optional<float> floatValue;
    std::optional<bool> boolValue;
    
    Token(TokenType t, const std::string& l, const SourceLocation& loc)
        : type(t), lexeme(l), location(loc) {}
};

// Lexer error
class LexerError : public std::runtime_error {
public:
    SourceLocation location;
    
    LexerError(const std::string& msg, const SourceLocation& loc)
        : std::runtime_error("[" + loc.toString() + "] " + msg), location(loc) {}
};

// Lexer class
class Lexer {
public:
    explicit Lexer(const std::string& source);
    
    // Tokenize the entire source
    std::vector<Token> tokenize();
    
    // Get next token (for incremental parsing)
    Token nextToken();
    
private:
    std::string source_;
    size_t pos_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    
    // Character operations
    char peek() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;
    
    // Skip whitespace and comments
    void skipWhitespace();
    
    // Token creation helpers
    Token makeToken(TokenType type, const std::string& lexeme);
    Token makeNumber();
    Token makeIdentifier();
    
    // Source location tracking
    SourceLocation currentLocation() const;
    void updateLocation(char c);
    
    // Keyword lookup
    static TokenType lookupKeyword(const std::string& identifier);
};

} // namespace mmrsl
