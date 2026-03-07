#include "mmrsl/mmrsl.hpp"
#include "mmrsl/detail/lexer.hpp"
#include "mmrsl/detail/parser.hpp"
#include "mmrsl/detail/interpreter.hpp"
#include "mmrsl/detail/compiler.hpp"
#include "mmrsl/detail/vm.hpp"
#include <chrono>

namespace mmrsl {

// ============================================================================
// SimpleParser Implementation
// ============================================================================

class SimpleParser::Impl {
public:
    std::unique_ptr<Program> program_;
    std::unique_ptr<Interpreter> interpreter_;
    std::string lastError_;
};

SimpleParser::SimpleParser() : impl_(std::make_unique<Impl>()) {}

SimpleParser::~SimpleParser() = default;

SimpleParser::SimpleParser(SimpleParser&&) noexcept = default;
SimpleParser& SimpleParser::operator=(SimpleParser&&) noexcept = default;

bool SimpleParser::compile(const std::string& source) {
    try {
        // Lexical analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Parsing
        RecursiveParser parser(tokens);
        impl_->program_ = parser.parse();
        
        if (!impl_->program_) {
            impl_->lastError_ = parser.getError();
            return false;
        }
        
        impl_->interpreter_ = std::make_unique<Interpreter>();
        impl_->lastError_.clear();
        return true;
    } catch (const std::exception& e) {
        impl_->lastError_ = e.what();
        return false;
    }
}

Value SimpleParser::execute(const std::vector<Value>& arguments) {
    if (!impl_->program_ || !impl_->interpreter_) {
        throw RuntimeError("No compiled program");
    }
    return impl_->interpreter_->execute(*impl_->program_, arguments);
}

Value SimpleParser::compileAndExecute(const std::string& source, const std::vector<Value>& arguments) {
    if (!compile(source)) {
        throw RuntimeError(impl_->lastError_);
    }
    return execute(arguments);
}

std::string SimpleParser::getFunctionName() const {
    return impl_->program_ ? impl_->program_->function->name : "";
}

TypeKind SimpleParser::getReturnType() const {
    return impl_->program_ ? impl_->program_->function->returnType : TypeKind::Void;
}

std::vector<TypeKind> SimpleParser::getParameterTypes() const {
    if (!impl_->program_) return {};
    std::vector<TypeKind> types;
    for (const auto& param : impl_->program_->function->parameters) {
        types.push_back(param.type);
    }
    return types;
}

std::string SimpleParser::getLastError() const {
    return impl_->lastError_;
}

bool SimpleParser::isCompiled() const {
    return impl_->program_ != nullptr;
}

// ============================================================================
// BytecodeParser Implementation
// ============================================================================

class BytecodeParser::Impl {
public:
    std::unique_ptr<bytecode::BytecodeFunction> bytecodeFunc_;
    std::unique_ptr<bytecode::VM> vm_;
    bytecode::Compiler compiler_;
    double compileTimeMs_ = 0.0;
    double executeTimeMs_ = 0.0;
    std::string lastError_;
    std::string functionName_;
};

BytecodeParser::BytecodeParser() : impl_(std::make_unique<Impl>()) {}

BytecodeParser::~BytecodeParser() = default;

BytecodeParser::BytecodeParser(BytecodeParser&&) noexcept = default;
BytecodeParser& BytecodeParser::operator=(BytecodeParser&&) noexcept = default;

bool BytecodeParser::compile(const std::string& source) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Clear previous state before attempting new compilation
    impl_->bytecodeFunc_.reset();
    impl_->vm_.reset();
    impl_->lastError_.clear();
    
    try {
        // Lexical analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Parsing
        RecursiveParser parser(tokens);
        auto program = parser.parse();
        
        if (!program) {
            impl_->lastError_ = parser.getError();
            return false;
        }
        
        // Compile to bytecode
        impl_->bytecodeFunc_ = std::make_unique<bytecode::BytecodeFunction>(impl_->compiler_.compile(*program));
        impl_->vm_ = std::make_unique<bytecode::VM>();
        
        auto end = std::chrono::high_resolution_clock::now();
        impl_->compileTimeMs_ = std::chrono::duration<double, std::milli>(end - start).count();
        return true;
    } catch (const std::exception& e) {
        impl_->lastError_ = e.what();
        // Ensure bytecode is cleared on failure
        impl_->bytecodeFunc_.reset();
        impl_->vm_.reset();
        return false;
    }
}

Value BytecodeParser::execute(const std::vector<Value>& arguments) {
    if (!impl_->bytecodeFunc_ || !impl_->vm_) {
        throw RuntimeError("No compiled bytecode");
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    Value result = impl_->vm_->execute(*impl_->bytecodeFunc_, arguments);
    auto end = std::chrono::high_resolution_clock::now();
    
    impl_->executeTimeMs_ = std::chrono::duration<double, std::milli>(end - start).count();
    return result;
}

Value BytecodeParser::compileAndExecute(const std::string& source, const std::vector<Value>& arguments) {
    if (!compile(source)) {
        throw RuntimeError(impl_->lastError_);
    }
    return execute(arguments);
}

std::string BytecodeParser::getFunctionName() const {
    // BytecodeFunction doesn't store function name currently
    return "";
}

TypeKind BytecodeParser::getReturnType() const {
    return impl_->bytecodeFunc_ ? impl_->bytecodeFunc_->returnType : TypeKind::Void;
}

std::vector<TypeKind> BytecodeParser::getParameterTypes() const {
    if (!impl_->bytecodeFunc_) return {};
    return impl_->bytecodeFunc_->paramTypes;
}

std::string BytecodeParser::getLastError() const {
    return impl_->lastError_;
}

bool BytecodeParser::isCompiled() const {
    return impl_->bytecodeFunc_ != nullptr;
}

double BytecodeParser::getLastCompileTimeMs() const {
    return impl_->compileTimeMs_;
}

double BytecodeParser::getLastExecuteTimeMs() const {
    return impl_->executeTimeMs_;
}

std::string BytecodeParser::getBytecodeDisassembly() const {
    if (!impl_->bytecodeFunc_) return "";
    return impl_->bytecodeFunc_->disassemble();
}

} // namespace mmrsl
