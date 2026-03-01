#pragma once

#include "mmrsl/interpreter.hpp"
#include <string>

namespace mmrsl {

// Main API class for the math parser
class mmrsl {
public:
    mmrsl();
    ~mmrsl();
    
    // Compile source code
    // Returns true on success, false on failure (check getLastError())
    bool compile(const std::string& source);
    
    // Execute the compiled function
    // Returns the result value
    // Throws RuntimeError on execution failure
    Value execute(const std::vector<Value>& arguments);
    
    // Convenience method: compile and execute in one call
    Value compileAndExecute(const std::string& source, const std::vector<Value>& arguments);
    
    // Get the name of the compiled function
    std::string getFunctionName() const;
    
    // Get the return type of the compiled function
    TypeKind getReturnType() const;
    
    // Get the parameter types of the compiled function
    std::vector<TypeKind> getParameterTypes() const;
    
    // Get the last error message
    std::string getLastError() const { return lastError_; }
    
    // Check if a function is compiled and ready to execute
    bool isCompiled() const { return program_ != nullptr; }
    
private:
    std::unique_ptr<Program> program_;
    std::unique_ptr<Interpreter> interpreter_;
    std::string lastError_;
    
    void setError(const std::string& error);
    void clearError();
};

} // namespace mmrsl
