#include "mmrsl/detail/lexer.hpp"
#include <cctype>
#include <unordered_map>

namespace mmrsl {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EOF";
        case TokenType::Int: return "int";
        case TokenType::Float: return "float";
        case TokenType::Bool: return "bool";
        case TokenType::Vec2: return "vec2";
        case TokenType::Vec3: return "vec3";
        case TokenType::Vec4: return "vec4";
        case TokenType::Mat2: return "mat2";
        case TokenType::Mat3: return "mat3";
        case TokenType::Mat4: return "mat4";
        case TokenType::Void: return "void";
        case TokenType::Return: return "return";
        case TokenType::If: return "if";
        case TokenType::Else: return "else";
        case TokenType::Floor: return "floor";
        case TokenType::Sin: return "sin";
        case TokenType::Cos: return "cos";
        case TokenType::Tan: return "tan";
        case TokenType::Abs: return "abs";
        case TokenType::Sqrt: return "sqrt";
        case TokenType::Min: return "min";
        case TokenType::Max: return "max";
        case TokenType::Dot: return "dot";
        case TokenType::Cross: return "cross";
        case TokenType::Length: return "length";
        case TokenType::Normalize: return "normalize";
        case TokenType::Reflect: return "reflect";
        case TokenType::Refract: return "refract";
        case TokenType::IntLiteral: return "IntLiteral";
        case TokenType::FloatLiteral: return "FloatLiteral";
        case TokenType::BoolLiteral: return "BoolLiteral";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Multiply: return "*";
        case TokenType::Divide: return "/";
        case TokenType::Modulo: return "%";
        case TokenType::Assign: return "=";
        case TokenType::Greater: return ">";
        case TokenType::GreaterEqual: return ">=";
        case TokenType::Less: return "<";
        case TokenType::LessEqual: return "<=";
        case TokenType::Equal: return "==";
        case TokenType::NotEqual: return "!=";
        case TokenType::And: return "&&";
        case TokenType::Or: return "||";
        case TokenType::Not: return "!";
        case TokenType::LeftParen: return "(";
        case TokenType::RightParen: return ")";
        case TokenType::LeftBrace: return "{";
        case TokenType::RightBrace: return "}";
        case TokenType::LeftBracket: return "[";
        case TokenType::RightBracket: return "]";
        case TokenType::Comma: return ",";
        case TokenType::Semicolon: return ";";
        case TokenType::Period: return ".";
        case TokenType::Question: return "?";
        case TokenType::Colon: return ":";
        case TokenType::Unknown: return "Unknown";
    }
    return "Unknown";
}

Lexer::Lexer(const std::string& source) : source_(source) {}

std::vector<Token> Lexer::tokenize() {
    // Check source code length limit (8KB)
    if (source_.length() > 8192) {
        throw LexerError("Source code length (" + std::to_string(source_.length()) + 
                        ") exceeds maximum length (8192)", SourceLocation{1, 1});
    }
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        tokens.push_back(nextToken());
    }
    
    // Add EOF token
    tokens.emplace_back(TokenType::EndOfFile, "", currentLocation());
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EndOfFile, "", currentLocation());
    }
    
    char c = peek();
    SourceLocation startLoc = currentLocation();
    
    // Numbers
    if (std::isdigit(c)) {
        return makeNumber();
    }
    
    // Identifiers and keywords
    if (std::isalpha(c) || c == '_') {
        return makeIdentifier();
    }
    
    // Single character tokens
    advance();
    
    switch (c) {
        case '+': return makeToken(TokenType::Plus, "+");
        case '-': return makeToken(TokenType::Minus, "-");
        case '*': return makeToken(TokenType::Multiply, "*");
        case '/': return makeToken(TokenType::Divide, "/");
        case '%': return makeToken(TokenType::Modulo, "%");
        
        // Comparison operators and assignment
        case '=':
            if (match('=')) {
                return makeToken(TokenType::Equal, "==");
            }
            return makeToken(TokenType::Assign, "=");
        
        case '>':
            if (match('=')) {
                return makeToken(TokenType::GreaterEqual, ">=");
            }
            return makeToken(TokenType::Greater, ">");
        
        case '<':
            if (match('=')) {
                return makeToken(TokenType::LessEqual, "<=");
            }
            return makeToken(TokenType::Less, "<");
        
        case '!':
            if (match('=')) {
                return makeToken(TokenType::NotEqual, "!=");
            }
            return makeToken(TokenType::Not, "!");
        
        case '&':
            if (match('&')) {
                return makeToken(TokenType::And, "&&");
            }
            throw LexerError(std::string("Unexpected character: &"), startLoc);
        
        case '|':
            if (match('|')) {
                return makeToken(TokenType::Or, "||");
            }
            throw LexerError(std::string("Unexpected character: |"), startLoc);
        
        case '(': return makeToken(TokenType::LeftParen, "(");
        case ')': return makeToken(TokenType::RightParen, ")");
        case '{': return makeToken(TokenType::LeftBrace, "{");
        case '}': return makeToken(TokenType::RightBrace, "}");
        case '[': return makeToken(TokenType::LeftBracket, "[");
        case ']': return makeToken(TokenType::RightBracket, "]");
        case ',': return makeToken(TokenType::Comma, ",");
        case ';': return makeToken(TokenType::Semicolon, ";");
        case '.': return makeToken(TokenType::Period, ".");
        case '?': return makeToken(TokenType::Question, "?");
        case ':': return makeToken(TokenType::Colon, ":");
        default:
            throw LexerError(std::string("Unexpected character: ") + c, startLoc);
    }
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[pos_];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source_[pos_];
    pos_++;
    updateLocation(c);
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || peek() != expected) return false;
    advance();
    return true;
}

bool Lexer::isAtEnd() const {
    return pos_ >= source_.size();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/') {
            // Check for comment
            if (pos_ + 1 < source_.size() && source_[pos_ + 1] == '/') {
                // Single line comment, skip to end of line
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, currentLocation());
}

Token Lexer::makeNumber() {
    SourceLocation startLoc = currentLocation();
    size_t start = pos_;
    bool isFloat = false;
    
    while (!isAtEnd() && std::isdigit(peek())) {
        advance();
    }
    
    if (!isAtEnd() && peek() == '.') {
        isFloat = true;
        advance();
        
        while (!isAtEnd() && std::isdigit(peek())) {
            advance();
        }
    }
    
    std::string lexeme = source_.substr(start, pos_ - start);
    Token token(TokenType::IntLiteral, lexeme, startLoc);
    
    if (isFloat) {
        token.type = TokenType::FloatLiteral;
        token.floatValue = std::stof(lexeme);
    } else {
        token.intValue = std::stoi(lexeme);
    }
    
    return token;
}

Token Lexer::makeIdentifier() {
    SourceLocation startLoc = currentLocation();
    size_t start = pos_;
    
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }
    
    std::string lexeme = source_.substr(start, pos_ - start);
    
    // Check identifier length limit (64 characters)
    if (lexeme.length() > 64) {
        throw LexerError("Identifier '" + lexeme.substr(0, 32) + "...' exceeds maximum length (64)", startLoc);
    }
    
    TokenType type = lookupKeyword(lexeme);
    
    Token token(type, lexeme, startLoc);
    
    // Set literal values for boolean literals
    if (type == TokenType::BoolLiteral) {
        token.boolValue = (lexeme == "true");
    }
    
    return token;
}

SourceLocation Lexer::currentLocation() const {
    return SourceLocation{line_, column_};
}

void Lexer::updateLocation(char c) {
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
}

TokenType Lexer::lookupKeyword(const std::string& identifier) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        // Types
        {"int", TokenType::Int},
        {"float", TokenType::Float},
        {"bool", TokenType::Bool},
        {"vec2", TokenType::Vec2},
        {"vec3", TokenType::Vec3},
        {"vec4", TokenType::Vec4},
        {"mat2", TokenType::Mat2},
        {"mat3", TokenType::Mat3},
        {"mat4", TokenType::Mat4},
        {"void", TokenType::Void},
        
        // Keywords
        {"return", TokenType::Return},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        
        // Built-in functions
        {"floor", TokenType::Floor},
        {"sin", TokenType::Sin},
        {"cos", TokenType::Cos},
        {"tan", TokenType::Tan},
        {"abs", TokenType::Abs},
        {"sqrt", TokenType::Sqrt},
        {"min", TokenType::Min},
        {"max", TokenType::Max},
        {"dot", TokenType::Dot},
        {"cross", TokenType::Cross},
        {"length", TokenType::Length},
        {"normalize", TokenType::Normalize},
        {"reflect", TokenType::Reflect},
        {"refract", TokenType::Refract},
        
        // Boolean literals
        {"true", TokenType::BoolLiteral},
        {"false", TokenType::BoolLiteral},
    };
    
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return it->second;
    }
    
    return TokenType::Identifier;
}

} // namespace mmrsl
