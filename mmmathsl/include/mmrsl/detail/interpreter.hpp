#pragma once

#include "parser.hpp"
#include "../mmrsl.hpp"
#include <unordered_map>
#include <cmath>

namespace mmrsl {

// Environment for variable storage
class Environment {
public:
    void define(const std::string& name, const Value& value);
    void assign(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    bool has(const std::string& name) const;
    void clear();
    
private:
    std::unordered_map<std::string, Value> variables_;
};

// Total loop iterations across ALL loops in a single function execution (DoS protection).
// This is a cumulative counter shared across nested loops, intentionally set high enough
// for typical GLSL compute patterns (e.g. for(200){for(200){}} = 40,000 iterations).
// An infinite loop will still be caught; an accidental double-nested 200x200 will not
// be rejected as a false positive.
constexpr uint32_t MAX_LOOP_ITERATIONS = 1'000'000;

// Interpreter for AST execution
class Interpreter {
public:
    Interpreter();
    
    // Execute a function
    Value execute(const FunctionDecl& function, const std::vector<Value>& arguments);
    
    // Execute with program
    Value execute(const Program& program, const std::vector<Value>& arguments);
    
private:
    Environment env_;
    bool isReturning_ = false;   // Flag to propagate return from nested statements
    bool isBreaking_ = false;    // Flag to propagate break from for loops
    bool isContinuing_ = false;  // Flag to propagate continue from for loops
    uint32_t loopIterationCount_ = 0;  // Total loop iterations counter (DoS protection)
    
    // Statement execution
    Value executeStatement(const Statement& stmt);
    Value executeVarDecl(const VarDeclStmt& stmt);
    Value executeAssign(const AssignStmt& stmt);
    Value executeReturn(const ReturnStmt& stmt);
    Value executeCompound(const CompoundStmt& stmt);
    Value executeIf(const IfStmt& stmt);
    Value executeFor(const ForStmt& stmt);
    Value executeBreak(const BreakStmt& stmt);
    Value executeContinue(const ContinueStmt& stmt);
    
    // Expression evaluation
    Value evaluateExpression(const Expression& expr);
    Value evaluateBinary(const BinaryExpr& expr);
    Value evaluateUnary(const UnaryExpr& expr);
    Value evaluateLiteral(const LiteralExpr& expr);
    Value evaluateVariable(const VariableExpr& expr);
    Value evaluateCall(const CallExpr& expr);
    Value evaluateConstructor(const ConstructorExpr& expr);
    Value evaluateMemberAccess(const MemberAccessExpr& expr);
    Value evaluateIndex(const IndexExpr& expr);
    Value evaluateTernary(const TernaryExpr& expr);
    
    // Built-in functions
    Value callBuiltin(const std::string& name, const std::vector<Value>& args);
    
    // Type conversion helpers
    float toFloat(const Value& value);
    int toInt(const Value& value);
};

} // namespace mmrsl
