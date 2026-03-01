#include "mmrsl/detail/parser.hpp"
#include <sstream>

namespace mmrsl {

// ==================== AST toString implementations ====================

std::string BinaryExpr::toString() const {
    return "(" + left->toString() + " " + tokenTypeToString(op) + " " + right->toString() + ")";
}

std::string UnaryExpr::toString() const {
    return "(" + tokenTypeToString(op) + operand->toString() + ")";
}

std::string LiteralExpr::toString() const {
    return value.toString();
}

std::string VariableExpr::toString() const {
    return name;
}

std::string CallExpr::toString() const {
    std::string result = function + "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) result += ", ";
        result += arguments[i]->toString();
    }
    result += ")";
    return result;
}

std::string ConstructorExpr::toString() const {
    std::string result = typeKindToString(type) + "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) result += ", ";
        result += arguments[i]->toString();
    }
    result += ")";
    return result;
}

std::string MemberAccessExpr::toString() const {
    return object->toString() + "." + member;
}

std::string TernaryExpr::toString() const {
    return "(" + condition->toString() + " ? " + thenExpr->toString() + " : " + elseExpr->toString() + ")";
}

std::string IndexExpr::toString() const {
    std::string result = object->toString() + "[" + index->toString() + "]";
    if (secondIndex) {
        result += "[" + secondIndex.value()->toString() + "]";
    }
    return result;
}

std::string VarDeclStmt::toString() const {
    std::string result = typeKindToString(type) + " " + name;
    if (initializer) {
        result += " = " + initializer->toString();
    }
    return result + ";";
}

std::string AssignStmt::toString() const {
    return name + " = " + value->toString() + ";";
}

std::string ReturnStmt::toString() const {
    return "return " + value->toString() + ";";
}

std::string CompoundStmt::toString() const {
    std::string result = "{\n";
    for (const auto& stmt : statements) {
        result += "  " + stmt->toString() + "\n";
    }
    result += "}";
    return result;
}

std::string IfStmt::toString() const {
    std::string result = "if (" + condition->toString() + ") " + thenBranch->toString();
    if (elseBranch) {
        result += " else " + elseBranch->toString();
    }
    return result;
}

std::string FunctionDecl::toString() const {
    std::string result = typeKindToString(returnType) + " " + name + "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) result += ", ";
        result += typeKindToString(parameters[i].type) + " " + parameters[i].name;
    }
    result += ") " + body->toString();
    return result;
}

// ==================== Parser implementation ====================

RecursiveParser::RecursiveParser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::unique_ptr<Program> RecursiveParser::parse() {
    astNodeCount_ = 0;  // Reset counter at start of parsing
    try {
        auto func = parseFunctionDecl();
        if (!func) return nullptr;
        incrementNodeCount();  // Count Program node
        return std::make_unique<Program>(std::move(func));
    } catch (const ParserError& e) {
        hasError_ = true;
        errorMsg_ = e.what();
        return nullptr;
    }
}

// Token operations
const Token& RecursiveParser::peek() const {
    if (pos_ >= tokens_.size()) {
        return tokens_.back();  // EOF token
    }
    return tokens_[pos_];
}

const Token& RecursiveParser::previous() const {
    if (pos_ == 0) return tokens_[0];
    return tokens_[pos_ - 1];
}

const Token& RecursiveParser::advance() {
    if (!isAtEnd()) pos_++;
    return previous();
}

bool RecursiveParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool RecursiveParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool RecursiveParser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (match(type)) return true;
    }
    return false;
}

bool RecursiveParser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

// Error handling
void RecursiveParser::error(const std::string& message) {
    errorAt(peek(), message);
}

void RecursiveParser::errorAt(const Token& token, const std::string& message) {
    throw ParserError(message, token.location);
}

void RecursiveParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;
        
        switch (peek().type) {
            case TokenType::Int:
            case TokenType::Float:
            case TokenType::Bool:
            case TokenType::Vec2:
            case TokenType::Vec3:
            case TokenType::Vec4:
            case TokenType::Mat2:
            case TokenType::Mat3:
            case TokenType::Mat4:
            case TokenType::Return:
                return;
            default:
                break;
        }
        
        advance();
    }
}

// Grammar rules
std::unique_ptr<FunctionDecl> RecursiveParser::parseFunctionDecl() {
    // Return type
    TypeKind returnType = parseType();
    
    // Function name
    if (!check(TokenType::Identifier)) {
        error("Expected function name");
        return nullptr;
    }
    std::string name = advance().lexeme;
    
    // Parameters
    if (!match(TokenType::LeftParen)) {
        error("Expected '(' after function name");
        return nullptr;
    }
    
    std::vector<Parameter> params = parseParameterList();
    
    if (!match(TokenType::RightParen)) {
        error("Expected ')' after parameters");
        return nullptr;
    }
    
    // Body
    auto body = parseCompoundStmt();
    if (!body) {
        error("Expected function body");
        return nullptr;
    }
    
    incrementNodeCount();
    return std::make_unique<FunctionDecl>(returnType, name, std::move(params), std::move(body));
}

std::vector<Parameter> RecursiveParser::parseParameterList() {
    std::vector<Parameter> params;
    
    if (check(TokenType::RightParen)) {
        return params;
    }
    
    do {
        TypeKind type = parseType();
        if (!check(TokenType::Identifier)) {
            error("Expected parameter name");
            break;
        }
        std::string name = advance().lexeme;
        params.push_back({type, name});
    } while (match(TokenType::Comma));
    
    return params;
}

std::unique_ptr<CompoundStmt> RecursiveParser::parseCompoundStmt() {
    if (!match(TokenType::LeftBrace)) {
        error("Expected '{'");
        return nullptr;
    }
    
    // Check nesting depth
    nestingDepth_++;
    checkNestingDepth();
    
    std::vector<StmtPtr> statements;
    
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    if (!match(TokenType::RightBrace)) {
        nestingDepth_--;
        error("Expected '}'");
        return nullptr;
    }
    
    nestingDepth_--;
    incrementNodeCount();
    return std::make_unique<CompoundStmt>(std::move(statements));
}

StmtPtr RecursiveParser::parseStatement() {
    // Variable declaration
    if (check(TokenType::Int) || check(TokenType::Float) || check(TokenType::Bool) ||
        check(TokenType::Vec2) || check(TokenType::Vec3) || check(TokenType::Vec4) ||
        check(TokenType::Mat2) || check(TokenType::Mat3) || check(TokenType::Mat4)) {
        // Look ahead to see if it's a declaration or expression
        size_t lookahead = pos_ + 1;
        if (lookahead < tokens_.size() && tokens_[lookahead].type == TokenType::Identifier) {
            return parseVarDecl();
        }
    }
    
    // Return statement
    if (match(TokenType::Return)) {
        return parseReturnStmt();
    }
    
    // If statement
    if (match(TokenType::If)) {
        return parseIfStmt();
    }
    
    // Assignment or expression
    return parseAssignOrExprStmt();
}

StmtPtr RecursiveParser::parseVarDecl() {
    TypeKind type = parseType();
    
    if (!check(TokenType::Identifier)) {
        error("Expected variable name");
        return nullptr;
    }
    std::string name = advance().lexeme;
    
    ExprPtr initializer = nullptr;
    if (match(TokenType::Assign)) {
        initializer = parseExpression();
    }
    
    if (!match(TokenType::Semicolon)) {
        error("Expected ';' after variable declaration");
        return nullptr;
    }
    
    incrementNodeCount();
    return std::make_unique<VarDeclStmt>(type, name, std::move(initializer));
}

StmtPtr RecursiveParser::parseAssignOrExprStmt() {
    // Check if it's an assignment (identifier followed by =)
    if (check(TokenType::Identifier)) {
        size_t lookahead = pos_ + 1;
        if (lookahead < tokens_.size() && tokens_[lookahead].type == TokenType::Assign) {
            std::string name = advance().lexeme;
            advance();  // consume '='
            ExprPtr value = parseExpression();
            
            if (!match(TokenType::Semicolon)) {
                error("Expected ';' after assignment");
                return nullptr;
            }
            
            incrementNodeCount();
            return std::make_unique<AssignStmt>(name, std::move(value));
        }
    }
    
    // Otherwise, it's an expression statement (not really used in our subset)
    error("Expected statement");
    return nullptr;
}

StmtPtr RecursiveParser::parseReturnStmt() {
    ExprPtr value = parseExpression();
    
    if (!match(TokenType::Semicolon)) {
        error("Expected ';' after return statement");
        return nullptr;
    }
    
    incrementNodeCount();
    return std::make_unique<ReturnStmt>(std::move(value));
}

StmtPtr RecursiveParser::parseIfStmt() {
    // Expect '(' after 'if'
    if (!match(TokenType::LeftParen)) {
        error("Expected '(' after 'if'");
        return nullptr;
    }
    
    // Parse condition
    ExprPtr condition = parseExpression();
    
    // Expect ')'
    if (!match(TokenType::RightParen)) {
        error("Expected ')' after if condition");
        return nullptr;
    }
    
    // Parse then branch
    StmtPtr thenBranch;
    if (check(TokenType::LeftBrace)) {
        thenBranch = parseCompoundStmt();
    } else {
        thenBranch = parseStatement();
    }
    
    // Parse optional else branch
    StmtPtr elseBranch = nullptr;
    if (match(TokenType::Else)) {
        if (check(TokenType::LeftBrace)) {
            elseBranch = parseCompoundStmt();
        } else {
            elseBranch = parseStatement();
        }
    }
    
    incrementNodeCount();
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

// Expression parsing
ExprPtr RecursiveParser::parseExpression() {
    exprDepth_++;
    checkExprDepth();
    auto result = parseTernary();
    exprDepth_--;
    return result;
}

ExprPtr RecursiveParser::parseTernary() {
    ExprPtr condition = parseLogical();
    
    // Ternary conditional: condition ? thenExpr : elseExpr
    if (match(TokenType::Question)) {
        ExprPtr thenExpr = parseExpression();  // Right-associative: parse full expression
        
        if (!match(TokenType::Colon)) {
            error("Expected ':' after '?' in ternary expression");
            return nullptr;
        }
        
        ExprPtr elseExpr = parseExpression();  // Right-associative: parse full expression
        
        incrementNodeCount();
        return std::make_unique<TernaryExpr>(std::move(condition), std::move(thenExpr), std::move(elseExpr));
    }
    
    return condition;
}

ExprPtr RecursiveParser::parseLogical() {
    ExprPtr left = parseComparison();
    
    while (match({TokenType::And, TokenType::Or})) {
        TokenType op = previous().type;
        ExprPtr right = parseComparison();
        incrementNodeCount();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    
    return left;
}

ExprPtr RecursiveParser::parseComparison() {
    ExprPtr left = parseAdditive();
    
    while (match({TokenType::Greater, TokenType::GreaterEqual, 
                  TokenType::Less, TokenType::LessEqual,
                  TokenType::Equal, TokenType::NotEqual})) {
        TokenType op = previous().type;
        ExprPtr right = parseAdditive();
        incrementNodeCount();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    
    return left;
}

ExprPtr RecursiveParser::parseAdditive() {
    ExprPtr left = parseMultiplicative();
    
    while (match({TokenType::Plus, TokenType::Minus})) {
        TokenType op = previous().type;
        ExprPtr right = parseMultiplicative();
        incrementNodeCount();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    
    return left;
}

ExprPtr RecursiveParser::parseMultiplicative() {
    ExprPtr left = parseUnary();
    
    while (match({TokenType::Multiply, TokenType::Divide, TokenType::Modulo})) {
        TokenType op = previous().type;
        ExprPtr right = parseUnary();
        incrementNodeCount();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    
    return left;
}

ExprPtr RecursiveParser::parseUnary() {
    if (match({TokenType::Minus, TokenType::Plus, TokenType::Not})) {
        TokenType op = previous().type;
        ExprPtr operand = parseUnary();
        incrementNodeCount();
        return std::make_unique<UnaryExpr>(op, std::move(operand));
    }
    
    return parsePostfix(parsePrimary());
}

ExprPtr RecursiveParser::parsePrimary() {
    // Literals
    if (match(TokenType::IntLiteral)) {
        incrementNodeCount();
        return std::make_unique<LiteralExpr>(Value(previous().intValue.value()));
    }
    
    if (match(TokenType::FloatLiteral)) {
        incrementNodeCount();
        return std::make_unique<LiteralExpr>(Value(previous().floatValue.value()));
    }
    
    if (match(TokenType::BoolLiteral)) {
            incrementNodeCount();
            return std::make_unique<LiteralExpr>(Value(previous().boolValue.value()));
    }
    
    // Constructor or function call
    if (check(TokenType::Vec2) || check(TokenType::Vec3) || check(TokenType::Vec4) ||
        check(TokenType::Mat2) || check(TokenType::Mat3) || check(TokenType::Mat4)) {
        return parseConstructor();
    }
    
    // Function call (identifier or built-in function)
    if (check(TokenType::Identifier) || check(TokenType::Floor) || check(TokenType::Sin) ||
        check(TokenType::Cos) || check(TokenType::Tan) || check(TokenType::Abs) ||
        check(TokenType::Sqrt) || check(TokenType::Min) || check(TokenType::Max) ||
        check(TokenType::Dot) || check(TokenType::Cross) ||
        check(TokenType::Length) || check(TokenType::Normalize) ||
        check(TokenType::Reflect) || check(TokenType::Refract)) {
        size_t lookahead = pos_ + 1;
        if (lookahead < tokens_.size() && tokens_[lookahead].type == TokenType::LeftParen) {
            // Function call
            std::string funcName = advance().lexeme;
            advance();  // consume '('
            
            std::vector<ExprPtr> args;
            if (!check(TokenType::RightParen)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::Comma));
            }
            
            if (!match(TokenType::RightParen)) {
                error("Expected ')' after arguments");
                return nullptr;
            }
            
            incrementNodeCount();
            return std::make_unique<CallExpr>(funcName, std::move(args));
        }
    }
    
    // Variable
    if (match(TokenType::Identifier)) {
        incrementNodeCount();
        return std::make_unique<VariableExpr>(previous().lexeme);
    }
    
    // Grouping
    if (match(TokenType::LeftParen)) {
        ExprPtr expr = parseExpression();
        if (!match(TokenType::RightParen)) {
            error("Expected ')' after expression");
            return nullptr;
        }
        return expr;
    }
    
    error("Expected expression");
    return nullptr;
}

ExprPtr RecursiveParser::parsePostfix(ExprPtr primary) {
    // Handle member access and index access
    while (true) {
        if (match(TokenType::Period)) {
            // Member access: primary.x, primary.y, primary.z, primary.w
            if (!check(TokenType::Identifier)) {
                error("Expected member name after '.'");
                return nullptr;
            }
            std::string member = advance().lexeme;
            // Support XYZW and RGBA swizzles
            if (member != "x" && member != "y" && member != "z" && member != "w" &&
                member != "r" && member != "g" && member != "b" && member != "a") {
                error("Invalid member name: " + member);
                return nullptr;
            }
            incrementNodeCount();
            primary = std::make_unique<MemberAccessExpr>(std::move(primary), member);
        } else if (match(TokenType::LeftBracket)) {
            // Index access: primary[index] or primary[row][col]
            ExprPtr index = parseExpression();
            if (!match(TokenType::RightBracket)) {
                error("Expected ']' after index");
                return nullptr;
            }
            
            // Check for second index (matrix access m[i][j])
            std::optional<ExprPtr> secondIndex = std::nullopt;
            if (match(TokenType::LeftBracket)) {
                secondIndex = parseExpression();
                if (!match(TokenType::RightBracket)) {
                    error("Expected ']' after second index");
                    return nullptr;
                }
            }
            
            incrementNodeCount();
            primary = std::make_unique<IndexExpr>(std::move(primary), std::move(index), std::move(secondIndex));
        } else {
            break;
        }
    }
    return primary;
}

ExprPtr RecursiveParser::parseConstructor() {
    TypeKind type = parseType();
    
    if (!match(TokenType::LeftParen)) {
        error("Expected '(' after type constructor");
        return nullptr;
    }
    
    std::vector<ExprPtr> args;
    if (!check(TokenType::RightParen)) {
        do {
            args.push_back(parseExpression());
        } while (match(TokenType::Comma));
    }
    
    if (!match(TokenType::RightParen)) {
        error("Expected ')' after constructor arguments");
        return nullptr;
    }
    
    incrementNodeCount();
    return std::make_unique<ConstructorExpr>(type, std::move(args));
}

TypeKind RecursiveParser::parseType() {
    if (match(TokenType::Void)) return TypeKind::Void;
    if (match(TokenType::Int)) return TypeKind::Int;
    if (match(TokenType::Float)) return TypeKind::Float;
    if (match(TokenType::Bool)) return TypeKind::Bool;
    if (match(TokenType::Vec2)) return TypeKind::Vec2;
    if (match(TokenType::Vec3)) return TypeKind::Vec3;
    if (match(TokenType::Vec4)) return TypeKind::Vec4;
    if (match(TokenType::Mat2)) return TypeKind::Mat2;
    if (match(TokenType::Mat3)) return TypeKind::Mat3;
    if (match(TokenType::Mat4)) return TypeKind::Mat4;
    
    error("Expected type");
    return TypeKind::Void;
}

} // namespace mmrsl
