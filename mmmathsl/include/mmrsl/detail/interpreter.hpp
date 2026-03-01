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
    bool isReturning_ = false;  // Flag to propagate return from nested statements
    
    // Statement execution
    Value executeStatement(const Statement& stmt);
    Value executeVarDecl(const VarDeclStmt& stmt);
    Value executeAssign(const AssignStmt& stmt);
    Value executeReturn(const ReturnStmt& stmt);
    Value executeCompound(const CompoundStmt& stmt);
    Value executeIf(const IfStmt& stmt);
    
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
