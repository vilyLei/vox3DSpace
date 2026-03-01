#include "mmrsl/math_parser.hpp"

namespace mmrsl {

mmrsl::mmrsl() : interpreter_(std::make_unique<Interpreter>()) {}

mmrsl::~mmrsl() = default;

bool mmrsl::compile(const std::string& source) {
    clearError();
    program_.reset();
    
    try {
        // Lexical analysis
        Lexer lexer(source);
        std::vector<Token> tokens;
        
        try {
            tokens = lexer.tokenize();
        } catch (const LexerError& e) {
            setError(std::string("Lexer error: ") + e.what());
            return false;
        }
        
        // Parsing
        Parser parser(tokens);
        program_ = parser.parse();
        
        if (!program_) {
            setError(std::string("Parser error: ") + parser.getError());
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        setError(std::string("Compilation error: ") + e.what());
        return false;
    }
}

Value mmrsl::execute(const std::vector<Value>& arguments) {
    if (!program_) {
        throw RuntimeError("No compiled program to execute");
    }
    
    try {
        return interpreter_->execute(*program_, arguments);
    } catch (const RuntimeError& e) {
        setError(std::string("Runtime error: ") + e.what());
        throw;
    }
}

Value mmrsl::compileAndExecute(const std::string& source, const std::vector<Value>& arguments) {
    if (!compile(source)) {
        throw RuntimeError("Compilation failed: " + lastError_);
    }
    return execute(arguments);
}

std::string mmrsl::getFunctionName() const {
    if (!program_ || !program_->function) {
        return "";
    }
    return program_->function->name;
}

TypeKind mmrsl::getReturnType() const {
    if (!program_ || !program_->function) {
        return TypeKind::Void;
    }
    return program_->function->returnType;
}

std::vector<TypeKind> mmrsl::getParameterTypes() const {
    std::vector<TypeKind> types;
    if (!program_ || !program_->function) {
        return types;
    }
    
    for (const auto& param : program_->function->parameters) {
        types.push_back(param.type);
    }
    return types;
}

void mmrsl::setError(const std::string& error) {
    lastError_ = error;
}

void mmrsl::clearError() {
    lastError_.clear();
}

} // namespace mmrsl
