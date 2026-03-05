#pragma once

#include "bytecode.hpp"
#include "parser.hpp"
#include "../types.hpp"
#include <unordered_map>
#include <string>
#include <bitset>

namespace mmrsl {
namespace highPerf {

// DoS protection limits
constexpr size_t MAX_SOURCE_LENGTH = 8192;        // 8KB source code limit
constexpr size_t MAX_IDENTIFIER_LENGTH = 64;      // Max identifier name length
constexpr int MAX_NESTING_DEPTH = 32;             // Max if/for/while nesting
constexpr int MAX_EXPRESSION_DEPTH = 64;          // Max expression nesting
constexpr size_t MAX_CONSTANT_POOL_SIZE = 1024;   // Max constants
constexpr int MAX_LOCAL_VARIABLES = 128;          // Max local variables
constexpr int MAX_FUNCTION_PARAMETERS = 16;       // Max function parameters
constexpr size_t MAX_BYTECODE_INSTRUCTIONS = 65536; // Max bytecode instructions (64K)
// Note: MAX_AST_NODES is defined in parser.hpp (simple/include/mmrsl/parser.hpp)

// Compiler error
class CompilerError : public std::runtime_error {
public:
    explicit CompilerError(const std::string& msg) : std::runtime_error(msg) {}
};

// AST to Bytecode compiler
class Compiler {
public:
    Compiler();
    
    // Compile a parsed program to bytecode
    BytecodeFunction compile(const Program& program);
    
    // Check if compilation succeeded
    bool hasError() const { return !errorMsg_.empty(); }
    const std::string& getError() const { return errorMsg_; }
    
private:
    BytecodeFunction* currentFunc_;
    std::string errorMsg_;
    
    // DoS protection: nesting depth tracking
    int currentNestingDepth_;
    int maxNestingDepth_;
    int currentExprDepth_;
    int maxExprDepth_;
    
    // Variable to local index mapping
    std::unordered_map<std::string, uint16_t> localVars_;
    std::unordered_map<std::string, TypeKind> localVarTypes_;
    uint16_t nextLocalIndex_;
    
    // Dedicated temporary local for return value transfer (reused across returns)
    uint16_t returnTempIdx_;
    
    // Register allocation (bitmap-based for arbitrary order free)
    std::bitset<128> usedRegisters_;
    uint8_t maxUsedRegister_;  // High-water mark for peak register usage
    
    // Statement compilation
    void compileStatement(const Statement& stmt);
    void compileVarDecl(const VarDeclStmt& stmt);
    void compileAssign(const AssignStmt& stmt);
    void compileReturn(const ReturnStmt& stmt);
    void compileCompound(const CompoundStmt& stmt);
    void compileIf(const IfStmt& stmt);
    void compileFor(const ForStmt& stmt);
    void compileBreak(const BreakStmt& stmt);
    void compileContinue(const ContinueStmt& stmt);
    
    // Loop context for break/continue backpatching (nested loop support)
    struct LoopContext {
        std::vector<size_t> breakPatches;    // indices of JUMP instructions to patch with loop-end
        std::vector<size_t> continuePatches; // indices of JUMP instructions to patch with update-start
    };
    std::vector<LoopContext> loopStack_;  // supports nested loops
    
    // Expression compilation - returns register containing result
    uint8_t compileExpression(const Expression& expr);
    uint8_t compileBinary(const BinaryExpr& expr);
    uint8_t compileUnary(const UnaryExpr& expr);
    uint8_t compileLiteral(const LiteralExpr& expr);
    uint8_t compileVariable(const VariableExpr& expr);
    uint8_t compileCall(const CallExpr& expr);
    uint8_t compileConstructor(const ConstructorExpr& expr);
    uint8_t compileMemberAccess(const MemberAccessExpr& expr);
    uint8_t compileIndexAccess(const IndexExpr& expr);
    uint8_t compileTernary(const TernaryExpr& expr);
    
    // Helper functions
    uint16_t getOrCreateLocal(const std::string& name);
    uint16_t getLocal(const std::string& name);
    uint8_t allocateRegister();
    void freeRegister(uint8_t reg);
    
    void setError(const std::string& msg);
    void emitMove(uint8_t destReg, uint8_t srcReg, TypeKind type);
    
    // Compute and write a 16-bit signed jump offset into the placeholder instruction at
    // patchIdx, targeting targetIdx.  Calls setError() if the offset exceeds int16_t range.
    void patchJump(size_t patchIdx, size_t targetIdx);
    
    // Check if builtin function exists but has wrong argument count
    void checkBuiltinFunctionArgCount(const std::string& funcName, size_t actualCount);
    
    // Get type of expression (for instruction selection)
    TypeKind getExpressionType(const Expression& expr);
    
    // RAII guard for exception-safe compilation
    class CompileGuard {
        Compiler* compiler_;
    public:
        explicit CompileGuard(Compiler* c) : compiler_(c) {}
        ~CompileGuard() {
            if (compiler_) {
                compiler_->currentFunc_ = nullptr;
            }
        }
        void release() { compiler_ = nullptr; }
    };
    friend class CompileGuard;
};

} // namespace highPerf
} // namespace mmrsl
