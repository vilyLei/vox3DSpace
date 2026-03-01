#pragma once

// High-performance math parser using bytecode VM

#include "mmrsl/detail/compiler.hpp"
#include "mmrsl/detail/vm.hpp"
#include "../../../simple/include/mmrsl/lexer.hpp"
#include "../../../simple/include/mmrsl/parser.hpp"
#include <chrono>
#include <memory>

namespace mmrsl {
namespace highPerf {

// High-performance parser using bytecode compilation and VM execution
class HPmmrsl {
public:
    HPmmrsl();
    
    // Compile source code to bytecode (measures compilation time)
    bool compile(const std::string& source);
    
    // Execute the compiled bytecode (measures execution time)
    Value execute(const std::vector<Value>& arguments);
    
    // Compile and execute with timing
    Value compileAndExecute(const std::string& source, const std::vector<Value>& arguments);
    
    // Get performance statistics
    double getLastCompileTimeMs() const { return compileTimeMs_; }
    double getLastExecuteTimeMs() const { return executeTimeMs_; }
    
    // Get function info
    std::string getFunctionName() const;
    TypeKind getReturnType() const;
    std::vector<TypeKind> getParameterTypes() const;
    std::string getLastError() const;
    bool isCompiled() const;
    
    // Get bytecode disassembly (for debugging)
    std::string getBytecodeDisassembly() const;
    
private:
    std::unique_ptr<BytecodeFunction> bytecodeFunc_;
    std::unique_ptr<VM> vm_;
    Compiler compiler_;
    
    double compileTimeMs_;
    double executeTimeMs_;
    std::string lastError_;
    std::string functionName_;
    
    void setError(const std::string& msg);
};

} // namespace highPerf
} // namespace mmrsl
