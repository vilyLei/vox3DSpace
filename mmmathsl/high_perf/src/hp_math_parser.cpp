#include "mmrsl/hp_math_parser.hpp"

namespace mmrsl {
namespace highPerf {

HPmmrsl::HPmmrsl() 
    : bytecodeFunc_(nullptr), vm_(std::make_unique<VM>()), 
      compileTimeMs_(0.0), executeTimeMs_(0.0) {}

bool HPmmrsl::compile(const std::string& source) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Check source length limit (8KB = 8192 bytes)
    if (source.length() > MAX_SOURCE_LENGTH) {
        setError("Source code exceeds maximum length of " + std::to_string(MAX_SOURCE_LENGTH) + " bytes");
        return false;
    }
    
    // Clear previous error state and bytecode
    lastError_.clear();
    bytecodeFunc_.reset();
    
    try {
        // Step 1: Lexical analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Step 2: Parsing
        Parser parser(tokens);
        auto program = parser.parse();
        
        if (!program || parser.hasError()) {
            setError("Parser error: " + parser.getError());
            return false;
        }
        
        // Store function name
        if (program->function) {
            functionName_ = program->function->name;
        }
        
        // Step 3: Compile to bytecode
        bytecodeFunc_ = std::make_unique<BytecodeFunction>(compiler_.compile(*program));
        
        if (compiler_.hasError()) {
            setError("Compiler error: " + compiler_.getError());
            bytecodeFunc_.reset();
            return false;
        }
        
    } catch (const std::exception& e) {
        setError(std::string("Compilation error: ") + e.what());
        return false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    compileTimeMs_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    return true;
}

Value HPmmrsl::execute(const std::vector<Value>& arguments) {
    if (!bytecodeFunc_) {
        throw VMError("No compiled bytecode to execute");
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    Value result = vm_->execute(*bytecodeFunc_, arguments);
    
    // Check for VM execution errors
    if (!vm_->getLastError().empty()) {
        throw VMError("Runtime error: " + vm_->getLastError());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    executeTimeMs_ = std::chrono::duration<double, std::milli>(end - start).count();
    
    return result;
}

Value HPmmrsl::compileAndExecute(const std::string& source, const std::vector<Value>& arguments) {
    if (!compile(source)) {
        throw VMError("Compilation failed: " + lastError_);
    }
    return execute(arguments);
}

std::string HPmmrsl::getFunctionName() const {
    return functionName_;
}

TypeKind HPmmrsl::getReturnType() const {
    if (bytecodeFunc_) {
        return bytecodeFunc_->returnType;
    }
    return TypeKind::Void;
}

std::vector<TypeKind> HPmmrsl::getParameterTypes() const {
    if (bytecodeFunc_) {
        return bytecodeFunc_->paramTypes;
    }
    return {};
}

std::string HPmmrsl::getLastError() const {
    return lastError_;
}

bool HPmmrsl::isCompiled() const {
    return bytecodeFunc_ != nullptr;
}

std::string HPmmrsl::getBytecodeDisassembly() const {
    if (bytecodeFunc_) {
        return bytecodeFunc_->disassemble();
    }
    return "No bytecode available";
}

void HPmmrsl::setError(const std::string& msg) {
    lastError_ = msg;
}

} // namespace highPerf
} // namespace mmrsl
