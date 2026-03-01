#pragma once

#include "mmrsl/lexer.hpp"
#include "mmrsl/types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace mmrsl {

// DoS protection limits
constexpr size_t MAX_AST_NODES = 10000;  // Max AST nodes during parsing

// Forward declarations
class Expression;
class Statement;

using ExprPtr = std::unique_ptr<Expression>;
using StmtPtr = std::unique_ptr<Statement>;

// ==================== AST Nodes ====================

// Base expression class
class Expression {
public:
    virtual ~Expression() = default;
    virtual std::string toString() const = 0;
};

// Binary expression: left op right
class BinaryExpr : public Expression {
public:
    TokenType op;
    ExprPtr left;
    ExprPtr right;
    
    BinaryExpr(TokenType oper, ExprPtr l, ExprPtr r)
        : op(oper), left(std::move(l)), right(std::move(r)) {}
    
    std::string toString() const override;
};

// Unary expression: op operand
class UnaryExpr : public Expression {
public:
    TokenType op;
    ExprPtr operand;
    
    UnaryExpr(TokenType oper, ExprPtr expr)
        : op(oper), operand(std::move(expr)) {}
    
    std::string toString() const override;
};

// Literal expression: 42, 3.14, true, {1, 2, 3}
class LiteralExpr : public Expression {
public:
    Value value;
    
    explicit LiteralExpr(Value val) : value(std::move(val)) {}
    
    std::string toString() const override;
};

// Variable expression: x, myVar
class VariableExpr : public Expression {
public:
    std::string name;
    
    explicit VariableExpr(std::string n) : name(std::move(n)) {}
    
    std::string toString() const override;
};

// Function call expression: floor(x), sin(angle)
class CallExpr : public Expression {
public:
    std::string function;
    std::vector<ExprPtr> arguments;
    
    CallExpr(std::string func, std::vector<ExprPtr> args)
        : function(std::move(func)), arguments(std::move(args)) {}
    
    std::string toString() const override;
};

// Constructor expression: vec2(1, 2), vec3(x, y, z)
class ConstructorExpr : public Expression {
public:
    TypeKind type;
    std::vector<ExprPtr> arguments;
    
    ConstructorExpr(TypeKind t, std::vector<ExprPtr> args)
        : type(t), arguments(std::move(args)) {}
    
    std::string toString() const override;
};

// Member access expression: v.x, v.y, v.z, v.w
class MemberAccessExpr : public Expression {
public:
    ExprPtr object;
    std::string member;  // "x", "y", "z", "w"
    
    MemberAccessExpr(ExprPtr obj, std::string m)
        : object(std::move(obj)), member(std::move(m)) {}
    
    std::string toString() const override;
};

// Index expression: m[i] or m[i][j]
class IndexExpr : public Expression {
public:
    ExprPtr object;
    ExprPtr index;
    std::optional<ExprPtr> secondIndex;  // for m[i][j]
    
    IndexExpr(ExprPtr obj, ExprPtr idx, std::optional<ExprPtr> idx2 = std::nullopt)
        : object(std::move(obj)), index(std::move(idx)), secondIndex(std::move(idx2)) {}
    
    std::string toString() const override;
};

// Base statement class
class Statement {
public:
    virtual ~Statement() = default;
    virtual std::string toString() const = 0;
};

// Variable declaration: float x = 1.0;
class VarDeclStmt : public Statement {
public:
    TypeKind type;
    std::string name;
    ExprPtr initializer;  // can be null
    
    VarDeclStmt(TypeKind t, std::string n, ExprPtr init)
        : type(t), name(std::move(n)), initializer(std::move(init)) {}
    
    std::string toString() const override;
};

// Assignment statement: x = 5.0;
class AssignStmt : public Statement {
public:
    std::string name;
    ExprPtr value;
    
    AssignStmt(std::string n, ExprPtr v)
        : name(std::move(n)), value(std::move(v)) {}
    
    std::string toString() const override;
};

// Return statement: return x + y;
class ReturnStmt : public Statement {
public:
    ExprPtr value;
    
    explicit ReturnStmt(ExprPtr v) : value(std::move(v)) {}
    
    std::string toString() const override;
};

// Compound statement: { stmt1; stmt2; }
class CompoundStmt : public Statement {
public:
    std::vector<StmtPtr> statements;
    
    explicit CompoundStmt(std::vector<StmtPtr> stmts)
        : statements(std::move(stmts)) {}
    
    std::string toString() const override;
};

// If statement
class IfStmt : public Statement {
public:
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;  // can be null
    
    IfStmt(ExprPtr cond, StmtPtr thenB, StmtPtr elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
    
    std::string toString() const override;
};

// Function parameter
struct Parameter {
    TypeKind type;
    std::string name;
};

// Function declaration
class FunctionDecl {
public:
    TypeKind returnType;
    std::string name;
    std::vector<Parameter> parameters;
    std::unique_ptr<CompoundStmt> body;
    
    FunctionDecl(TypeKind ret, std::string n, std::vector<Parameter> params,
                 std::unique_ptr<CompoundStmt> b)
        : returnType(ret), name(std::move(n)), parameters(std::move(params)),
          body(std::move(b)) {}
    
    std::string toString() const;
};

// Program (single function for now)
class Program {
public:
    std::unique_ptr<FunctionDecl> function;
    
    explicit Program(std::unique_ptr<FunctionDecl> func)
        : function(std::move(func)) {}
};

// ==================== Parser ====================

class ParserError : public std::runtime_error {
public:
    SourceLocation location;
    
    ParserError(const std::string& msg, const SourceLocation& loc)
        : std::runtime_error("[" + loc.toString() + "] " + msg), location(loc) {}
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    
    // Parse the entire program
    std::unique_ptr<Program> parse();
    
    // Check if there were any errors
    bool hasError() const { return hasError_; }
    const std::string& getError() const { return errorMsg_; }
    
private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;
    bool hasError_ = false;
    std::string errorMsg_;
    size_t astNodeCount_ = 0;  // AST node counter for DoS protection
    
    // Increment and check AST node count
    void incrementNodeCount() {
        if (++astNodeCount_ > MAX_AST_NODES) {
            throw ParserError("Maximum AST node count (" + std::to_string(MAX_AST_NODES) + ") exceeded", peek().location);
        }
    }
    
    // Token operations
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    bool isAtEnd() const;
    
    // Error handling
    void error(const std::string& message);
    void errorAt(const Token& token, const std::string& message);
    void synchronize();
    
    // Grammar rules
    std::unique_ptr<FunctionDecl> parseFunctionDecl();
    std::vector<Parameter> parseParameterList();
    std::unique_ptr<CompoundStmt> parseCompoundStmt();
    StmtPtr parseStatement();
    StmtPtr parseVarDecl();
    StmtPtr parseAssignOrExprStmt();
    StmtPtr parseReturnStmt();
    StmtPtr parseIfStmt();
    
    // Expression parsing (precedence climbing)
    ExprPtr parseExpression();
    ExprPtr parseLogical();     // &&, ||
    ExprPtr parseComparison();  // comparison operators
    ExprPtr parseAssignment();  // lowest precedence
    ExprPtr parseAdditive();
    ExprPtr parseMultiplicative();
    ExprPtr parseUnary();
    ExprPtr parsePrimary();
    ExprPtr parsePostfix(ExprPtr primary);
    ExprPtr parseConstructor();
    
    // Type parsing
    TypeKind parseType();
};

} // namespace mmrsl
