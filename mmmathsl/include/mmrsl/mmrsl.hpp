#pragma once

/**
 * mmrsl - High-performance mathematical expression parser
 * 
 * Features:
 * - GLSL-style syntax (vec2/3/4, mat2/3/4, swizzles)
 * - Two execution engines: Simple interpreter and High-Performance VM
 * - Zero external dependencies for public API
 */

#include "types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace mmrsl {

// Runtime error exception
class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

// ============================================================================
// Base Parser Interface
// ============================================================================

class Parser {
public:
    virtual ~Parser() = default;
    
    // Compile source code
    // Returns true on success, false on failure (check getLastError())
    [[nodiscard]] virtual bool compile(const std::string& source) = 0;
    
    // Execute the compiled function
    // Returns the result value
    // Throws RuntimeError on execution failure
    virtual Value execute(const std::vector<Value>& arguments) = 0;
    
    // Convenience method: compile and execute in one call
    virtual Value compileAndExecute(const std::string& source, const std::vector<Value>& arguments) = 0;
    
    // Get function info
    virtual std::string getFunctionName() const = 0;
    virtual TypeKind getReturnType() const = 0;
    virtual std::vector<TypeKind> getParameterTypes() const = 0;
    
    // Get the last error message
    virtual std::string getLastError() const = 0;
    
    // Check if a function is compiled and ready to execute
    virtual bool isCompiled() const = 0;
};

// ============================================================================
// Simple Interpreter
// ============================================================================

class SimpleParser : public Parser {
public:
    SimpleParser();
    ~SimpleParser();
    
    // Disable copy
    SimpleParser(const SimpleParser&) = delete;
    SimpleParser& operator=(const SimpleParser&) = delete;
    
    // Enable move
    SimpleParser(SimpleParser&&) noexcept;
    SimpleParser& operator=(SimpleParser&&) noexcept;
    
    // Parser interface
    bool compile(const std::string& source) override;
    Value execute(const std::vector<Value>& arguments) override;
    Value compileAndExecute(const std::string& source, const std::vector<Value>& arguments) override;
    std::string getFunctionName() const override;
    TypeKind getReturnType() const override;
    std::vector<TypeKind> getParameterTypes() const override;
    std::string getLastError() const override;
    bool isCompiled() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// Bytecode VM Parser (compile to bytecode, execute via register-based VM)
// ============================================================================

class BytecodeParser : public Parser {
public:
    BytecodeParser();
    ~BytecodeParser();
    
    // Disable copy
    BytecodeParser(const BytecodeParser&) = delete;
    BytecodeParser& operator=(const BytecodeParser&) = delete;
    
    // Enable move
    BytecodeParser(BytecodeParser&&) noexcept;
    BytecodeParser& operator=(BytecodeParser&&) noexcept;
    
    // Parser interface
    bool compile(const std::string& source) override;
    Value execute(const std::vector<Value>& arguments) override;
    Value compileAndExecute(const std::string& source, const std::vector<Value>& arguments) override;
    std::string getFunctionName() const override;
    TypeKind getReturnType() const override;
    std::vector<TypeKind> getParameterTypes() const override;
    std::string getLastError() const override;
    bool isCompiled() const override;
    
    // Performance statistics (bytecode VM only)
    double getLastCompileTimeMs() const;
    double getLastExecuteTimeMs() const;
    
    // Get bytecode disassembly (for debugging)
    std::string getBytecodeDisassembly() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mmrsl
