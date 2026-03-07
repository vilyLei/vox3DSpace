#include "mmrsl/detail/compiler.hpp"
#include <cassert>

namespace mmrsl {
namespace bytecode {

Compiler::Compiler() 
    : currentFunc_(nullptr), nextLocalIndex_(0), returnTempIdx_(0xFFFF), 
      maxUsedRegister_(0), currentNestingDepth_(0), maxNestingDepth_(0),
      currentExprDepth_(0), maxExprDepth_(0) {
}

BytecodeFunction Compiler::compile(const Program& program) {
    Compiler::CompileGuard guard(this);  // RAII guard for exception safety
    
    BytecodeFunction func;
    currentFunc_ = &func;
    localVars_.clear();
    localVarTypes_.clear();
    usedRegisters_.reset();
    nextLocalIndex_ = 0;
    returnTempIdx_ = 0xFFFF;  // Reset for lazy initialization
    maxUsedRegister_ = 0;     // Reset high-water mark
    currentNestingDepth_ = 0; // Reset nesting depth
    maxNestingDepth_ = 0;
    currentExprDepth_ = 0; // Reset expression depth
    maxExprDepth_ = 0;
    loopStack_.clear();  // Reset loop context stack
    errorMsg_.clear();
    
    if (!program.function) {
        setError("No function to compile");
        return func;
    }
    
    // Set function metadata
    func.returnType = program.function->returnType;
    
    // Check parameter count limit
    if (program.function->parameters.size() > MAX_FUNCTION_PARAMETERS) {
        setError("Maximum number of function parameters (" + std::to_string(MAX_FUNCTION_PARAMETERS) + ") exceeded");
        return func;
    }
    
    for (const auto& param : program.function->parameters) {
        // Check parameter name length
        if (param.name.length() > MAX_IDENTIFIER_LENGTH) {
            setError("Parameter name '" + param.name.substr(0, 32) + "...' exceeds maximum length (" + 
                     std::to_string(MAX_IDENTIFIER_LENGTH) + ")");
            return func;
        }
        
        func.paramTypes.push_back(param.type);
        // Allocate locals for parameters and store their types
        uint16_t idx = getOrCreateLocal(param.name);
        localVarTypes_[param.name] = param.type;
        (void)idx; // Parameters are initialized by caller
    }
    
    // Compile function body.
    // Wrapped in try-catch to convert CompilerError (thrown by BytecodeFunction::emit()
    // when MAX_BYTECODE_INSTRUCTIONS is exceeded) into the standard setError() path.
    // This ensures all error reporting goes through getLastError(), preserving the
    // public API contract regardless of whether the error was detected by setError() or throw.
    try {
        compileCompound(*program.function->body);
        
        // Check if the last statement is already a return
        bool hasTrailingReturn = false;
        if (!program.function->body->statements.empty()) {
            if (dynamic_cast<const ReturnStmt*>(program.function->body->statements.back().get())) {
                hasTrailingReturn = true;
            }
        }
        
        // Only emit trailing return if function doesn't end with one
        if (!hasTrailingReturn) {
            currentFunc_->emit(OpCode::RETURN);
        }
        currentFunc_->emit(OpCode::HALT);
    } catch (const CompilerError& e) {
        // Convert emit()-level CompilerError into the unified setError() path
        setError(std::string("Bytecode limit: ") + e.what());
    }
    
    func.numLocals = static_cast<uint8_t>(nextLocalIndex_);
    // Use high-water mark for peak register usage (+1 because registers are 0-indexed)
    func.maxRegisters = static_cast<uint8_t>(maxUsedRegister_ + 1);
    
    // Check if there was a compilation error
    if (hasError()) {
        throw std::runtime_error(getError());
    }
    
    guard.release();  // Success, don't cleanup
    return func;
}

void Compiler::compileStatement(const Statement& stmt) {
    if (auto varDecl = dynamic_cast<const VarDeclStmt*>(&stmt)) {
        compileVarDecl(*varDecl);
    } else if (auto assign = dynamic_cast<const AssignStmt*>(&stmt)) {
        compileAssign(*assign);
    } else if (auto ret = dynamic_cast<const ReturnStmt*>(&stmt)) {
        compileReturn(*ret);
    } else if (auto compound = dynamic_cast<const CompoundStmt*>(&stmt)) {
        compileCompound(*compound);
    } else if (auto ifStmt = dynamic_cast<const IfStmt*>(&stmt)) {
        compileIf(*ifStmt);
    } else if (auto forStmt = dynamic_cast<const ForStmt*>(&stmt)) {
        compileFor(*forStmt);
    } else if (auto breakStmt = dynamic_cast<const BreakStmt*>(&stmt)) {
        compileBreak(*breakStmt);
    } else if (auto continueStmt = dynamic_cast<const ContinueStmt*>(&stmt)) {
        compileContinue(*continueStmt);
    } else {
        setError("Unsupported statement type");
    }
}

void Compiler::compileVarDecl(const VarDeclStmt& stmt) {
    // Check identifier length limit
    if (stmt.name.length() > MAX_IDENTIFIER_LENGTH) {
        setError("Identifier '" + stmt.name.substr(0, 32) + "...' exceeds maximum length (" + 
                 std::to_string(MAX_IDENTIFIER_LENGTH) + ")");
        return;
    }
    
    // Allocate local variable
    uint16_t localIdx = getOrCreateLocal(stmt.name);
    
    // Store variable type for later type inference
    localVarTypes_[stmt.name] = stmt.type;
    
    // Compile initializer if present
    if (stmt.initializer) {
        uint8_t reg = compileExpression(*stmt.initializer);
        // Coerce int to float if variable is declared as float
        TypeKind initType = getExpressionType(*stmt.initializer);
        if (stmt.type == TypeKind::Float && initType == TypeKind::Int) {
            uint8_t floatReg = allocateRegister();
            currentFunc_->emit(OpCode::INT_TO_FLOAT, floatReg, reg, 0);
            freeRegister(reg);
            reg = floatReg;
        }
        currentFunc_->emit(OpCode::STORE_LOCAL, reg, localIdx);
        freeRegister(reg);
    }
}

void Compiler::compileAssign(const AssignStmt& stmt) {
    uint16_t localIdx = getLocal(stmt.name);
    uint8_t reg = compileExpression(*stmt.value);
    // Coerce int to float if the target variable is a float
    auto varTypeIt = localVarTypes_.find(stmt.name);
    if (varTypeIt != localVarTypes_.end() && varTypeIt->second == TypeKind::Float) {
        TypeKind valType = getExpressionType(*stmt.value);
        if (valType == TypeKind::Int) {
            uint8_t floatReg = allocateRegister();
            currentFunc_->emit(OpCode::INT_TO_FLOAT, floatReg, reg, 0);
            freeRegister(reg);
            reg = floatReg;
        }
    }
    currentFunc_->emit(OpCode::STORE_LOCAL, reg, localIdx);
    freeRegister(reg);
}

void Compiler::compileReturn(const ReturnStmt& stmt) {
    uint8_t reg = compileExpression(*stmt.value);
    // Coerce int to float if function return type is float
    if (currentFunc_->returnType == TypeKind::Float) {
        TypeKind exprType = getExpressionType(*stmt.value);
        if (exprType == TypeKind::Int) {
            uint8_t floatReg = allocateRegister();
            currentFunc_->emit(OpCode::INT_TO_FLOAT, floatReg, reg, 0);
            freeRegister(reg);
            reg = floatReg;
        }
    }
    // Ensure result is in register 0 (convention for return value)
    if (reg != 0) {
        // Lazy initialization: allocate return temp on first use
        if (returnTempIdx_ == 0xFFFF) {
            returnTempIdx_ = nextLocalIndex_++;
        }
        currentFunc_->emit(OpCode::STORE_LOCAL, reg, returnTempIdx_);
        currentFunc_->emit(OpCode::LOAD_LOCAL, 0, returnTempIdx_);
    }
    // Emit RETURN instruction to immediately return from function
    currentFunc_->emit(OpCode::RETURN);
    // Free the register used by the expression (even if it's register 0,
    // to ensure proper cleanup for subsequent code after this return)
    freeRegister(reg);
}

void Compiler::compileCompound(const CompoundStmt& stmt) {
    for (const auto& s : stmt.statements) {
        compileStatement(*s);
        if (hasError()) return;  // stop on first error â€?avoids cascading UB
    }
}

uint8_t Compiler::compileExpression(const Expression& expr) {
    // Check expression depth limit
    currentExprDepth_++;
    if (currentExprDepth_ > MAX_EXPRESSION_DEPTH) {
        setError("Maximum expression depth (" + std::to_string(MAX_EXPRESSION_DEPTH) + ") exceeded");
        currentExprDepth_--;
        return 0;
    }
    if (currentExprDepth_ > maxExprDepth_) {
        maxExprDepth_ = currentExprDepth_;
    }
    
    uint8_t result = 0;
    if (auto binary = dynamic_cast<const BinaryExpr*>(&expr)) {
        result = compileBinary(*binary);
    } else if (auto unary = dynamic_cast<const UnaryExpr*>(&expr)) {
        result = compileUnary(*unary);
    } else if (auto literal = dynamic_cast<const LiteralExpr*>(&expr)) {
        result = compileLiteral(*literal);
    } else if (auto var = dynamic_cast<const VariableExpr*>(&expr)) {
        result = compileVariable(*var);
    } else if (auto call = dynamic_cast<const CallExpr*>(&expr)) {
        result = compileCall(*call);
    } else if (auto ctor = dynamic_cast<const ConstructorExpr*>(&expr)) {
        result = compileConstructor(*ctor);
    } else if (auto member = dynamic_cast<const MemberAccessExpr*>(&expr)) {
        result = compileMemberAccess(*member);
    } else if (auto index = dynamic_cast<const IndexExpr*>(&expr)) {
        result = compileIndexAccess(*index);
    } else if (auto ternary = dynamic_cast<const TernaryExpr*>(&expr)) {
        result = compileTernary(*ternary);
    } else {
        setError("Unknown expression type");
    }
    
    currentExprDepth_--;
    return result;
}

uint8_t Compiler::compileBinary(const BinaryExpr& expr) {
    uint8_t leftReg = compileExpression(*expr.left);
    uint8_t rightReg = compileExpression(*expr.right);
    uint8_t resultReg = allocateRegister();
    
    TypeKind leftType = getExpressionType(*expr.left);
    TypeKind rightType = getExpressionType(*expr.right);
    
    // Auto-coerce Int to Float for mixed arithmetic (Int op Float or Float op Int)
    // This handles cases like `s * 1` or `1 + s` where s is float
    auto coerceIntRegToFloat = [&](uint8_t& reg, TypeKind& kind) {
        if (kind == TypeKind::Int) {
            uint8_t floatReg = allocateRegister();
            currentFunc_->emit(OpCode::INT_TO_FLOAT, floatReg, reg, 0);
            freeRegister(reg);
            reg = floatReg;
            kind = TypeKind::Float;
        }
    };
    // For pure arithmetic ops (+,-,*,/,%), coerce Int<->Float to Float
    bool isArithOp = (expr.op == TokenType::Plus || expr.op == TokenType::Minus ||
                      expr.op == TokenType::Multiply || expr.op == TokenType::Divide ||
                      expr.op == TokenType::Modulo);
    bool isCmpOp = (expr.op == TokenType::Greater || expr.op == TokenType::GreaterEqual ||
                    expr.op == TokenType::Less || expr.op == TokenType::LessEqual ||
                    expr.op == TokenType::Equal || expr.op == TokenType::NotEqual);
    if (isArithOp || isCmpOp) {
        if (leftType == TypeKind::Float && rightType == TypeKind::Int) {
            coerceIntRegToFloat(rightReg, rightType);
        } else if (leftType == TypeKind::Int && rightType == TypeKind::Float) {
            coerceIntRegToFloat(leftReg, leftType);
        }
    }
    
    // Select appropriate instruction based on types and operator
    switch (expr.op) {
        case TokenType::Plus:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::ADD_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::ADD_INT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec2 && rightType == TypeKind::Vec2) {
                currentFunc_->emit(OpCode::ADD_VEC2, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec2) {
                currentFunc_->emit(OpCode::ADD_FLOAT_VEC2, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec3 && rightType == TypeKind::Vec3) {
                currentFunc_->emit(OpCode::ADD_VEC3, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec3) {
                currentFunc_->emit(OpCode::ADD_FLOAT_VEC3, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec4 && rightType == TypeKind::Vec4) {
                currentFunc_->emit(OpCode::ADD_VEC4, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec4) {
                currentFunc_->emit(OpCode::ADD_FLOAT_VEC4, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '+' operator");
            }
            break;
        case TokenType::Minus:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::SUB_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::SUB_INT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec2 && rightType == TypeKind::Vec2) {
                currentFunc_->emit(OpCode::SUB_VEC2, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec2) {
                currentFunc_->emit(OpCode::SUB_FLOAT_VEC2, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec3 && rightType == TypeKind::Vec3) {
                currentFunc_->emit(OpCode::SUB_VEC3, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec3) {
                currentFunc_->emit(OpCode::SUB_FLOAT_VEC3, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec3 && rightType == TypeKind::Float) {
                // vec3 - float: subtract float from each component
                currentFunc_->emit(OpCode::SUB_VEC3_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec4 && rightType == TypeKind::Vec4) {
                currentFunc_->emit(OpCode::SUB_VEC4, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec4) {
                currentFunc_->emit(OpCode::SUB_FLOAT_VEC4, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '-' operator");
            }
            break;
        case TokenType::Multiply:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::MUL_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::MUL_INT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec2 && rightType == TypeKind::Vec2) {
                currentFunc_->emit(OpCode::MUL_VEC2, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec2 && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::MUL_VEC2_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec2) {
                // Float * Vec2 - same as Vec2 * Float, swap operands
                currentFunc_->emit(OpCode::MUL_VEC2_FLOAT, resultReg, rightReg, leftReg);
            } else if (leftType == TypeKind::Vec3 && rightType == TypeKind::Vec3) {
                currentFunc_->emit(OpCode::MUL_VEC3, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec3 && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::MUL_VEC3_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec3) {
                // Float * Vec3 - same as Vec3 * Float, swap operands
                currentFunc_->emit(OpCode::MUL_VEC3_FLOAT, resultReg, rightReg, leftReg);
            } else if (leftType == TypeKind::Vec4 && rightType == TypeKind::Vec4) {
                currentFunc_->emit(OpCode::MUL_VEC4, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec4 && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::MUL_VEC4_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Float && rightType == TypeKind::Vec4) {
                // Float * Vec4 - same as Vec4 * Float, swap operands
                currentFunc_->emit(OpCode::MUL_VEC4_FLOAT, resultReg, rightReg, leftReg);
            } else if (leftType == TypeKind::Mat2 && rightType == TypeKind::Mat2) {
                currentFunc_->emit(OpCode::MUL_MAT2_MAT2, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Mat3 && rightType == TypeKind::Mat3) {
                currentFunc_->emit(OpCode::MUL_MAT3_MAT3, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Mat4 && rightType == TypeKind::Mat4) {
                currentFunc_->emit(OpCode::MUL_MAT4_MAT4, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Mat2 && rightType == TypeKind::Vec2) {
                currentFunc_->emit(OpCode::MUL_MAT2_VEC2, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Mat3 && rightType == TypeKind::Vec3) {
                currentFunc_->emit(OpCode::MUL_MAT3_VEC3, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Mat4 && rightType == TypeKind::Vec4) {
                currentFunc_->emit(OpCode::MUL_MAT4_VEC4, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '*' operator");
            }
            break;
        case TokenType::Divide:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::DIV_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::DIV_INT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec2 && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::DIV_VEC2_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec3 && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::DIV_VEC3_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Vec4 && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::DIV_VEC4_FLOAT, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '/' operator");
            }
            break;
        case TokenType::Modulo:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::MOD_FLOAT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::MOD_INT, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '%' operator");
            }
            break;
        // Comparison operators
        case TokenType::Greater:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::CMP_GT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::CMP_GT, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '>' operator");
            }
            break;
        case TokenType::GreaterEqual:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::CMP_GE, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::CMP_GE, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '>=' operator");
            }
            break;
        case TokenType::Less:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::CMP_LT, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::CMP_LT, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '<' operator");
            }
            break;
        case TokenType::LessEqual:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::CMP_LE, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::CMP_LE, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '<=' operator");
            }
            break;
        case TokenType::Equal:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::CMP_EQ, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::CMP_EQ, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '==' operator");
            }
            break;
        case TokenType::NotEqual:
            if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
                currentFunc_->emit(OpCode::CMP_NE, resultReg, leftReg, rightReg);
            } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
                currentFunc_->emit(OpCode::CMP_NE, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '!=' operator");
            }
            break;
        // Logical operators
        case TokenType::And:
            if (leftType == TypeKind::Bool && rightType == TypeKind::Bool) {
                currentFunc_->emit(OpCode::LOGICAL_AND, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '&&' operator");
            }
            break;
        case TokenType::Or:
            if (leftType == TypeKind::Bool && rightType == TypeKind::Bool) {
                currentFunc_->emit(OpCode::LOGICAL_OR, resultReg, leftReg, rightReg);
            } else {
                setError("Invalid operand types for '||' operator");
            }
            break;
        default:
            setError("Unknown binary operator");
            break;
    }
    
    freeRegister(leftReg);
    freeRegister(rightReg);
    return resultReg;
}

uint8_t Compiler::compileUnary(const UnaryExpr& expr) {
    uint8_t operandReg = compileExpression(*expr.operand);
    
    TypeKind operandType = getExpressionType(*expr.operand);
    
    switch (expr.op) {
        case TokenType::Minus: {
            uint8_t resultReg = allocateRegister();
            if (operandType == TypeKind::Float) {
                currentFunc_->emit(OpCode::NEG_FLOAT, resultReg, operandReg, 0);
            } else if (operandType == TypeKind::Int) {
                // Emit 0 - operand using SUB_INT
                uint8_t zeroReg = allocateRegister();
                uint16_t zeroIdx = currentFunc_->addConstant(Value(0));
                currentFunc_->emit(OpCode::LOAD_CONST_INT, zeroReg, zeroIdx);
                currentFunc_->emit(OpCode::SUB_INT, resultReg, zeroReg, operandReg);
                freeRegister(zeroReg);
            } else if (operandType == TypeKind::Vec2) {
                currentFunc_->emit(OpCode::NEG_VEC2, resultReg, operandReg, 0);
            } else if (operandType == TypeKind::Vec3) {
                currentFunc_->emit(OpCode::NEG_VEC3, resultReg, operandReg, 0);
            } else if (operandType == TypeKind::Vec4) {
                currentFunc_->emit(OpCode::NEG_VEC4, resultReg, operandReg, 0);
            } else {
                setError("Invalid operand type for unary '-' operator");
            }
            freeRegister(operandReg);
            return resultReg;
        }
        case TokenType::Plus:
            // No-op, just return the operand register
            return operandReg;
        case TokenType::Not: {
            uint8_t resultReg = allocateRegister();
            if (operandType == TypeKind::Bool) {
                currentFunc_->emit(OpCode::LOGICAL_NOT, resultReg, operandReg, 0);
            } else {
                setError("Invalid operand type for '!' operator");
            }
            freeRegister(operandReg);
            return resultReg;
        }
        default:
            setError("Unknown unary operator");
            return operandReg;
    }
}

uint8_t Compiler::compileLiteral(const LiteralExpr& expr) {
    uint8_t reg = allocateRegister();
    
    // Keep int values as int (for loop counters and integer arithmetic)
    Value val = expr.value;
    
    // Check constant pool size limit
    if (currentFunc_->constants.size() >= MAX_CONSTANT_POOL_SIZE) {
        setError("Maximum constant pool size (" + std::to_string(MAX_CONSTANT_POOL_SIZE) + ") exceeded");
        return reg; // Return the already allocated register
    }
    
    uint16_t constIdx = currentFunc_->addConstant(val);
    
    OpCode loadOp;
    switch (val.kind()) {
        case TypeKind::Float: loadOp = OpCode::LOAD_CONST_FLOAT; break;
        case TypeKind::Int: loadOp = OpCode::LOAD_CONST_INT; break;
        case TypeKind::Bool: loadOp = OpCode::LOAD_CONST_BOOL; break;
        case TypeKind::Vec2: loadOp = OpCode::LOAD_CONST_VEC2; break;
        case TypeKind::Vec3: loadOp = OpCode::LOAD_CONST_VEC3; break;
        case TypeKind::Vec4: loadOp = OpCode::LOAD_CONST_VEC4; break;
        default: loadOp = OpCode::LOAD_CONST_FLOAT; break;
    }
    
    currentFunc_->emit(loadOp, reg, constIdx);
    return reg;
}

uint8_t Compiler::compileVariable(const VariableExpr& expr) {
    uint16_t localIdx = getLocal(expr.name);
    uint8_t reg = allocateRegister();
    currentFunc_->emit(OpCode::LOAD_LOCAL, reg, localIdx);
    return reg;
}

uint8_t Compiler::compileCall(const CallExpr& expr) {
    // Compile arguments
    std::vector<uint8_t> argRegs;
    for (const auto& arg : expr.arguments) {
        argRegs.push_back(compileExpression(*arg));
        if (hasError()) {
            // Free all argument registers compiled so far, then bail out.
            // resultReg has not been allocated yet so no leak there.
            for (uint8_t reg : argRegs) freeRegister(reg);
            return allocateRegister();  // return a dummy register so callers stay consistent
        }
    }
    
    uint8_t resultReg = allocateRegister();
    
    // Map function name to opcode
    if (expr.function == "floor" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_FLOOR, resultReg, argRegs[0], 0);
    } else if (expr.function == "ceil" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_CEIL, resultReg, argRegs[0], 0);
    } else if (expr.function == "fract" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_FRACT, resultReg, argRegs[0], 0);
    } else if (expr.function == "sin" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_SIN, resultReg, argRegs[0], 0);
    } else if (expr.function == "cos" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_COS, resultReg, argRegs[0], 0);
    } else if (expr.function == "tan" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_TAN, resultReg, argRegs[0], 0);
    } else if (expr.function == "asin" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_ASIN, resultReg, argRegs[0], 0);
    } else if (expr.function == "acos" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_ACOS, resultReg, argRegs[0], 0);
    } else if (expr.function == "atan" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_ATAN, resultReg, argRegs[0], 0);
    } else if (expr.function == "atan" && argRegs.size() == 2) {
        currentFunc_->emit(OpCode::CALL_ATAN2, resultReg, argRegs[0], argRegs[1]);
    } else if (expr.function == "radians" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_RADIANS, resultReg, argRegs[0], 0);
    } else if (expr.function == "degrees" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_DEGREES, resultReg, argRegs[0], 0);
    } else if (expr.function == "abs" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_ABS, resultReg, argRegs[0], 0);
    } else if (expr.function == "sign" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_SIGN, resultReg, argRegs[0], 0);
    } else if (expr.function == "sqrt" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_SQRT, resultReg, argRegs[0], 0);
    } else if (expr.function == "exp" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_EXP, resultReg, argRegs[0], 0);
    } else if (expr.function == "log" && argRegs.size() == 1) {
        currentFunc_->emit(OpCode::CALL_LOG, resultReg, argRegs[0], 0);
    } else if (expr.function == "pow" && argRegs.size() == 2) {
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Float) {
            currentFunc_->emit(OpCode::CALL_POW, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::CALL_POW_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::CALL_POW_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::CALL_POW_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("pow() requires float, vec2, vec3, or vec4 as first argument");
        }
    } else if (expr.function == "mod" && argRegs.size() == 2) {
        currentFunc_->emit(OpCode::CALL_MOD_FLOAT, resultReg, argRegs[0], argRegs[1]);
    } else if (expr.function == "dot" && argRegs.size() == 2) {
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::CALL_DOT_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::CALL_DOT_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::CALL_DOT_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("dot() requires vec2, vec3, or vec4 arguments");
        }
    } else if (expr.function == "cross" && argRegs.size() == 2) {
        currentFunc_->emit(OpCode::CALL_CROSS_VEC3, resultReg, argRegs[0], argRegs[1]);
    } else if (expr.function == "length" && argRegs.size() == 1) {
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::LENGTH_VEC2, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::LENGTH_VEC3, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::LENGTH_VEC4, resultReg, argRegs[0], 0);
        } else {
            setError("length() requires vec2, vec3, or vec4 argument");
        }
    } else if (expr.function == "distance" && argRegs.size() == 2) {
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::DISTANCE_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::DISTANCE_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::DISTANCE_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("distance() requires vec2, vec3, or vec4 arguments");
        }
    } else if (expr.function == "normalize" && argRegs.size() == 1) {
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::NORMALIZE_VEC2, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::NORMALIZE_VEC3, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::NORMALIZE_VEC4, resultReg, argRegs[0], 0);
        } else {
            setError("normalize() requires vec2, vec3, or vec4 argument");
        }
    } else if (expr.function == "reflect" && argRegs.size() == 2) {
        currentFunc_->emit(OpCode::REFLECT_VEC3, resultReg, argRegs[0], argRegs[1]);
    } else if (expr.function == "refract" && argRegs.size() == 3) {
        // refract(I, N, eta) - use LOAD_THIRD_PARAM for eta
        currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[2], 0);
        currentFunc_->emit(OpCode::REFRACT_VEC3, resultReg, argRegs[0], argRegs[1]);
    } else if (expr.function == "min" && argRegs.size() == 2) {
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Float) {
            currentFunc_->emit(OpCode::CALL_MIN_FLOAT, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::CALL_MIN_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::CALL_MIN_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::CALL_MIN_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("min() requires float, vec2, vec3, or vec4 arguments");
        }
    } else if (expr.function == "max" && argRegs.size() == 2) {
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Float) {
            currentFunc_->emit(OpCode::CALL_MAX_FLOAT, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::CALL_MAX_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::CALL_MAX_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::CALL_MAX_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("max() requires float, vec2, vec3, or vec4 arguments");
        }
    } else if (expr.function == "clamp" && argRegs.size() == 3) {
        // clamp(value, min, max) - use LOAD_THIRD_PARAM for max
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[2], 0);
        if (argType == TypeKind::Float) {
            currentFunc_->emit(OpCode::CALL_CLAMP_FLOAT, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::CALL_CLAMP_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::CALL_CLAMP_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::CALL_CLAMP_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("clamp() requires float, vec2, vec3, or vec4 as first argument");
        }
    } else if (expr.function == "mix" && argRegs.size() == 3) {
        // mix(a, b, t) - use LOAD_THIRD_PARAM for t
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[2], 0);
        if (argType == TypeKind::Float) {
            currentFunc_->emit(OpCode::CALL_MIX_FLOAT, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::CALL_MIX_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::CALL_MIX_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else if (argType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::CALL_MIX_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("Unsupported mix() argument type");
        }
    } else if (expr.function == "inverse" && argRegs.size() == 1) {
        // inverse(matrix) - matrix inverse
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Mat2) {
            currentFunc_->emit(OpCode::INVERSE_MAT2, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Mat3) {
            currentFunc_->emit(OpCode::INVERSE_MAT3, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Mat4) {
            currentFunc_->emit(OpCode::INVERSE_MAT4, resultReg, argRegs[0], 0);
        } else {
            setError("inverse() requires mat2, mat3, or mat4 argument");
        }
    } else if (expr.function == "transpose" && argRegs.size() == 1) {
        // transpose(matrix) - matrix transpose
        TypeKind argType = getExpressionType(*expr.arguments[0]);
        if (argType == TypeKind::Mat2) {
            currentFunc_->emit(OpCode::TRANSPOSE_MAT2, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Mat3) {
            currentFunc_->emit(OpCode::TRANSPOSE_MAT3, resultReg, argRegs[0], 0);
        } else if (argType == TypeKind::Mat4) {
            currentFunc_->emit(OpCode::TRANSPOSE_MAT4, resultReg, argRegs[0], 0);
        } else {
            setError("transpose() requires mat2, mat3, or mat4 argument");
        }
    } else if (expr.function == "noise1" && argRegs.size() == 1) {
        // noise1(float) -> float
        currentFunc_->emit(OpCode::NOISE1, resultReg, argRegs[0], 0);
    } else if (expr.function == "noise2" && argRegs.size() == 1) {
        // noise2(vec2) -> vec2
        currentFunc_->emit(OpCode::NOISE2, resultReg, argRegs[0], 0);
    } else if (expr.function == "noise3" && argRegs.size() == 1) {
        // noise3(vec3) -> vec3
        currentFunc_->emit(OpCode::NOISE3, resultReg, argRegs[0], 0);
    } else if (expr.function == "noise4" && argRegs.size() == 1) {
        // noise4(vec4) -> vec4
        currentFunc_->emit(OpCode::NOISE4, resultReg, argRegs[0], 0);
    } else if (expr.function == "step" && argRegs.size() == 2) {
        // step(edge, x) -> float
        currentFunc_->emit(OpCode::CALL_STEP_FLOAT, resultReg, argRegs[0], argRegs[1]);
    } else if (expr.function == "smoothstep" && argRegs.size() == 3) {
        // smoothstep(edge0, edge1, x) -> float
        // Load third parameter (x) to thirdParam slot
        currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[2], 0);
        currentFunc_->emit(OpCode::CALL_SMOOTHSTEP_FLOAT, resultReg, argRegs[0], argRegs[1]);
    } else {
        // Check if function name is recognized but argument count is wrong
        checkBuiltinFunctionArgCount(expr.function, argRegs.size());
        // If not a builtin with wrong arg count, report unknown function
        if (!hasError()) {
            setError("Unknown function: " + expr.function);
        }
    }
    
    // Free argument registers
    for (uint8_t reg : argRegs) {
        freeRegister(reg);
    }
    
    return resultReg;
}

uint8_t Compiler::compileConstructor(const ConstructorExpr& expr) {
    // Compile arguments
    std::vector<uint8_t> argRegs;
    for (const auto& arg : expr.arguments) {
        argRegs.push_back(compileExpression(*arg));
    }
    
    uint8_t resultReg = allocateRegister();
    
    // Handle scalar type conversions: float(x) and int(x)
    if (expr.type == TypeKind::Float) {
        if (argRegs.size() == 1) {
            TypeKind argType = getExpressionType(*expr.arguments[0]);
            if (argType == TypeKind::Int) {
                currentFunc_->emit(OpCode::INT_TO_FLOAT, resultReg, argRegs[0], 0);
            } else {
                // Already float, just move
                currentFunc_->emit(OpCode::MOV_FLOAT, resultReg, argRegs[0], 0);
            }
        } else {
            setError("float() constructor requires 1 argument");
        }
        for (uint8_t reg : argRegs) freeRegister(reg);
        return resultReg;
    }
    if (expr.type == TypeKind::Int) {
        if (argRegs.size() == 1) {
            TypeKind argType = getExpressionType(*expr.arguments[0]);
            if (argType == TypeKind::Float) {
                currentFunc_->emit(OpCode::FLOAT_TO_INT, resultReg, argRegs[0], 0);
            } else {
                // Already int: copy via register move (MOV_FLOAT copies Value as-is)
                currentFunc_->emit(OpCode::MOV_FLOAT, resultReg, argRegs[0], 0);
            }
        } else {
            setError("int() constructor requires 1 argument");
        }
        for (uint8_t reg : argRegs) freeRegister(reg);
        return resultReg;
    }
    
    // Create the value based on constructor type and arguments
    if (expr.type == TypeKind::Vec2) {
        if (argRegs.size() == 1) {
            // vec2(x) - construct from single float (replicated)
            currentFunc_->emit(OpCode::CONSTRUCT_VEC2, resultReg, argRegs[0], argRegs[0]);
        } else if (argRegs.size() == 2) {
            // vec2(x, y) - construct from two floats
            currentFunc_->emit(OpCode::CONSTRUCT_VEC2, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("vec2 constructor requires 1 or 2 arguments");
        }
    } else if (expr.type == TypeKind::Vec3) {
        if (argRegs.size() == 1) {
            // vec3(x) - construct from single float (replicated)
            currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[0], 0);
            currentFunc_->emit(OpCode::CONSTRUCT_VEC3, resultReg, argRegs[0], argRegs[0]);
        } else if (argRegs.size() == 3) {
            // vec3(x, y, z) - use LOAD_THIRD_PARAM for z
            currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[2], 0);
            currentFunc_->emit(OpCode::CONSTRUCT_VEC3, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("vec3 constructor requires 1 or 3 arguments");
        }
    } else if (expr.type == TypeKind::Vec4) {
        if (argRegs.size() == 1) {
            // vec4(x) - construct from single float (replicated)
            currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[0], 0);
            currentFunc_->emit(OpCode::LOAD_FOURTH_PARAM, 0, argRegs[0], 0);
            currentFunc_->emit(OpCode::CONSTRUCT_VEC4, resultReg, argRegs[0], argRegs[0]);
        } else if (argRegs.size() == 4) {
            // vec4(x, y, z, w) - use LOAD_THIRD_PARAM for z, LOAD_FOURTH_PARAM for w
            currentFunc_->emit(OpCode::LOAD_THIRD_PARAM, 0, argRegs[2], 0);
            currentFunc_->emit(OpCode::LOAD_FOURTH_PARAM, 0, argRegs[3], 0);
            currentFunc_->emit(OpCode::CONSTRUCT_VEC4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("vec4 constructor requires 1 or 4 arguments");
        }
    } else if (expr.type == TypeKind::Mat2) {
        if (argRegs.size() == 1) {
            // mat2(x) - diagonal matrix with x on diagonal
            // VM mapping: m00=rs1, m01=rs2, m10=extra[0], m11=extra[1]
            // Target: m00=v, m01=0, m10=0, m11=v
            uint8_t zeroReg = allocateRegister();
            uint16_t zeroIdx = currentFunc_->addConstant(Value(0.0f));
            currentFunc_->emit(OpCode::LOAD_CONST_FLOAT, zeroReg, zeroIdx);
            // extra[0] = m10 = 0, extra[1] = m11 = v
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 0, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 1, argRegs[0], 0);
            // rs1 = m00 = v, rs2 = m01 = 0
            currentFunc_->emit(OpCode::CONSTRUCT_MAT2, resultReg, argRegs[0], zeroReg);
            freeRegister(zeroReg);
        } else if (argRegs.size() == 4) {
            // mat2(m00, m01, m10, m11) - construct from four floats
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 0, argRegs[2], 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 1, argRegs[3], 0);
            currentFunc_->emit(OpCode::CONSTRUCT_MAT2, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("mat2 constructor requires 1 or 4 arguments");
        }
    } else if (expr.type == TypeKind::Mat3) {
        if (argRegs.size() == 1) {
            // mat3(x) - diagonal matrix with x on diagonal
            // VM mapping: m00=rs1, m01=rs2, extra[i] -> mat[(i+2)/3][(i+2)%3]
            // Target: m00=v, m01=0, m02=0, m10=0, m11=v, m12=0, m20=0, m21=0, m22=v
            uint8_t zeroReg = allocateRegister();
            uint16_t zeroIdx = currentFunc_->addConstant(Value(0.0f));
            currentFunc_->emit(OpCode::LOAD_CONST_FLOAT, zeroReg, zeroIdx);
            // extra[0] -> m[0][2]=0, extra[1] -> m[1][0]=0, extra[2] -> m[1][1]=v
            // extra[3] -> m[1][2]=0, extra[4] -> m[2][0]=0, extra[5] -> m[2][1]=0
            // extra[6] -> m[2][2]=v
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 0, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 1, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 2, argRegs[0], 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 3, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 4, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 5, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 6, argRegs[0], 0);
            // rs1 = m00 = v, rs2 = m01 = 0
            currentFunc_->emit(OpCode::CONSTRUCT_MAT3, resultReg, argRegs[0], zeroReg);
            freeRegister(zeroReg);
        } else if (argRegs.size() == 9) {
            // mat3(9 floats)
            for (size_t i = 2; i < 9; i++) {
                currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, static_cast<uint8_t>(i - 2), argRegs[i], 0);
            }
            currentFunc_->emit(OpCode::CONSTRUCT_MAT3, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("mat3 constructor requires 1 or 9 arguments");
        }
    } else if (expr.type == TypeKind::Mat4) {
        if (argRegs.size() == 1) {
            // mat4(x) - diagonal matrix with x on diagonal
            // VM mapping: m00=rs1, m01=rs2, extra[i] -> mat[(i+2)/4][(i+2)%4] for i=0..13
            // Target: m00=v, m01=0, m02=0, m03=0, m10=0, m11=v, m12=0, m13=0
            //         m20=0, m21=0, m22=v, m23=0, m30=0, m31=0, m32=0, m33=v
            uint8_t zeroReg = allocateRegister();
            uint16_t zeroIdx = currentFunc_->addConstant(Value(0.0f));
            currentFunc_->emit(OpCode::LOAD_CONST_FLOAT, zeroReg, zeroIdx);
            // extra[0] -> m[0][2]=0, extra[1] -> m[0][3]=0, extra[2] -> m[1][0]=0
            // extra[3] -> m[1][1]=v, extra[4] -> m[1][2]=0, extra[5] -> m[1][3]=0
            // extra[6] -> m[2][0]=0, extra[7] -> m[2][1]=0, extra[8] -> m[2][2]=v
            // extra[9] -> m[2][3]=0, extra[10] -> m[3][0]=0, extra[11] -> m[3][1]=0
            // extra[12] -> m[3][2]=0, extra[13] -> m[3][3]=v
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 0, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 1, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 2, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 3, argRegs[0], 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 4, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 5, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 6, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 7, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 8, argRegs[0], 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 9, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 10, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 11, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 12, zeroReg, 0);
            currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 13, argRegs[0], 0);
            // rs1 = m00 = v, rs2 = m01 = 0
            currentFunc_->emit(OpCode::CONSTRUCT_MAT4, resultReg, argRegs[0], zeroReg);
            freeRegister(zeroReg);
        } else if (argRegs.size() == 16) {
            // mat4(16 floats)
            for (size_t i = 2; i < 16; i++) {
                currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, static_cast<uint8_t>(i - 2), argRegs[i], 0);
            }
            currentFunc_->emit(OpCode::CONSTRUCT_MAT4, resultReg, argRegs[0], argRegs[1]);
        } else {
            setError("mat4 constructor requires 1 or 16 arguments");
        }
    } else {
        setError("Unknown constructor type");
    }
    
    // Free argument registers
    for (uint8_t reg : argRegs) {
        freeRegister(reg);
    }
    
    return resultReg;
}

uint8_t Compiler::compileMemberAccess(const MemberAccessExpr& expr) {
    uint8_t objectReg = compileExpression(*expr.object);
    uint8_t resultReg = allocateRegister();
    
    // Support XYZW and RGBA swizzles
    if (expr.member == "x" || expr.member == "r") {
        currentFunc_->emit(OpCode::MEMBER_X, resultReg, objectReg, 0);
    } else if (expr.member == "y" || expr.member == "g") {
        currentFunc_->emit(OpCode::MEMBER_Y, resultReg, objectReg, 0);
    } else if (expr.member == "z" || expr.member == "b") {
        currentFunc_->emit(OpCode::MEMBER_Z, resultReg, objectReg, 0);
    } else if (expr.member == "w" || expr.member == "a") {
        currentFunc_->emit(OpCode::MEMBER_W, resultReg, objectReg, 0);
    } else {
        setError("Unknown member: " + expr.member);
    }
    
    freeRegister(objectReg);
    return resultReg;
}

uint16_t Compiler::getOrCreateLocal(const std::string& name) {
    auto it = localVars_.find(name);
    if (it != localVars_.end()) {
        return it->second;
    }
    
    // Check local variable limit
    if (nextLocalIndex_ >= MAX_LOCAL_VARIABLES) {
        setError("Maximum number of local variables (" + std::to_string(MAX_LOCAL_VARIABLES) + ") exceeded");
        return 0;
    }
    
    uint16_t idx = nextLocalIndex_++;
    localVars_[name] = idx;
    return idx;
}

uint16_t Compiler::getLocal(const std::string& name) {
    auto it = localVars_.find(name);
    if (it != localVars_.end()) {
        return it->second;
    }
    
    setError("Undefined variable: " + name);
    return 0;
}

uint8_t Compiler::allocateRegister() {
    // Find first unused register using bitset
    for (int i = 0; i < 128; i++) {
        if (!usedRegisters_[i]) {
            usedRegisters_[i] = true;
            // Update high-water mark
            if (i > maxUsedRegister_) {
                maxUsedRegister_ = static_cast<uint8_t>(i);
            }
            return static_cast<uint8_t>(i);
        }
    }
    setError("Out of registers");
    return 0;
}

void Compiler::freeRegister(uint8_t reg) {
    // Mark register as free
    if (reg < 128) {
        usedRegisters_[reg] = false;
    }
}

uint8_t Compiler::compileIndexAccess(const IndexExpr& expr) {
    uint8_t objectReg = compileExpression(*expr.object);
    uint8_t resultReg = allocateRegister();
    
    // Get the type of the object being indexed
    TypeKind objectType = getExpressionType(*expr.object);
    
    if (expr.secondIndex) {
        // Matrix access: m[col][row] (first index is column, second is row in GLSL/GLM column-major)
        // Compile column and row indices - they should be integers
        uint8_t colReg = compileExpression(*expr.index);
        uint8_t rowReg = compileExpression(*expr.secondIndex.value());
        
        // Select appropriate matrix index instruction
        // Pass col in regSrc1 (first index), use LOAD_EXTRA_PARAM for row (second index)
        currentFunc_->emit(OpCode::LOAD_EXTRA_PARAM, 0, rowReg, 0);
        if (objectType == TypeKind::Mat2) {
            currentFunc_->emit(OpCode::MAT2_INDEX, resultReg, colReg, objectReg);
        } else if (objectType == TypeKind::Mat3) {
            currentFunc_->emit(OpCode::MAT3_INDEX, resultReg, colReg, objectReg);
        } else if (objectType == TypeKind::Mat4) {
            currentFunc_->emit(OpCode::MAT4_INDEX, resultReg, colReg, objectReg);
        } else {
            setError("Double index access only supported for matrix types");
        }
        
        freeRegister(rowReg);
        freeRegister(colReg);
    } else {
        // Vector access: v[i]
        uint8_t indexReg = compileExpression(*expr.index);
        if (objectType == TypeKind::Vec2) {
            currentFunc_->emit(OpCode::VEC2_INDEX, resultReg, indexReg, objectReg);
        } else if (objectType == TypeKind::Vec3) {
            currentFunc_->emit(OpCode::VEC3_INDEX, resultReg, indexReg, objectReg);
        } else if (objectType == TypeKind::Vec4) {
            currentFunc_->emit(OpCode::VEC4_INDEX, resultReg, indexReg, objectReg);
        } else {
            setError("Single index access only supported for vector types");
        }
        freeRegister(indexReg);
    }
    
    freeRegister(objectReg);
    return resultReg;
}

uint8_t Compiler::compileTernary(const TernaryExpr& expr) {
    // Compile condition
    uint8_t condReg = compileExpression(*expr.condition);
    
    // Emit JUMP_IF_FALSE with placeholder offset
    size_t jumpIfFalseIdx = currentFunc_->code.size();
    currentFunc_->emit(OpCode::JUMP_IF_FALSE, condReg, 0, 0);  // Placeholder
    
    // Condition register no longer needed
    freeRegister(condReg);
    
    // Compile then expression
    uint8_t thenReg = compileExpression(*expr.thenExpr);
    
    // Allocate result register
    uint8_t resultReg = allocateRegister();
    
    // Move then result to result register
    TypeKind thenType = getExpressionType(*expr.thenExpr);
    emitMove(resultReg, thenReg, thenType);
    freeRegister(thenReg);
    
    // Emit JUMP to skip else branch
    size_t jumpOverElseIdx = currentFunc_->code.size();
    currentFunc_->emit(OpCode::JUMP, 0, 0, 0);  // Placeholder
    
    // Backpatch the JUMP_IF_FALSE to jump to else branch
    size_t elseStartIdx = currentFunc_->code.size();
    patchJump(jumpIfFalseIdx, elseStartIdx);
    
    // Compile else expression
    uint8_t elseReg = compileExpression(*expr.elseExpr);
    
    // Move else result to result register
    TypeKind elseType = getExpressionType(*expr.elseExpr);
    emitMove(resultReg, elseReg, elseType);
    freeRegister(elseReg);
    
    // Backpatch the JUMP to skip over else branch
    size_t afterElseIdx = currentFunc_->code.size();
    patchJump(jumpOverElseIdx, afterElseIdx);
    
    return resultReg;
}

void Compiler::emitMove(uint8_t destReg, uint8_t srcReg, TypeKind type) {
    // Emit appropriate move instruction based on type
    switch (type) {
        case TypeKind::Float:
            currentFunc_->emit(OpCode::MOV_FLOAT, destReg, srcReg, 0);
            break;
        case TypeKind::Int:
            // No dedicated MOV_INT opcode yet; Value carries its own type tag so
            // MOV_FLOAT performs an identical register copy at the VM level.
            // If a MOV_INT opcode is added in the future, update this case.
            currentFunc_->emit(OpCode::MOV_FLOAT, destReg, srcReg, 0);
            break;
        case TypeKind::Bool:
            currentFunc_->emit(OpCode::MOV_BOOL, destReg, srcReg, 0);
            break;
        case TypeKind::Vec2:
            currentFunc_->emit(OpCode::MOV_VEC2, destReg, srcReg, 0);
            break;
        case TypeKind::Vec3:
            currentFunc_->emit(OpCode::MOV_VEC3, destReg, srcReg, 0);
            break;
        case TypeKind::Vec4:
            currentFunc_->emit(OpCode::MOV_VEC4, destReg, srcReg, 0);
            break;
        case TypeKind::Mat2:
            currentFunc_->emit(OpCode::MOV_MAT2, destReg, srcReg, 0);
            break;
        case TypeKind::Mat3:
            currentFunc_->emit(OpCode::MOV_MAT3, destReg, srcReg, 0);
            break;
        case TypeKind::Mat4:
            currentFunc_->emit(OpCode::MOV_MAT4, destReg, srcReg, 0);
            break;
        default:
            // Unknown type â€?fall back to float move
            currentFunc_->emit(OpCode::MOV_FLOAT, destReg, srcReg, 0);
            break;
    }
}

void Compiler::setError(const std::string& msg) {
    if (errorMsg_.empty()) {
        errorMsg_ = msg;
    }
}

void Compiler::patchJump(size_t patchIdx, size_t targetIdx) {
    // Guard: patchIdx must be a valid instruction slot
    if (!currentFunc_ || patchIdx >= currentFunc_->code.size()) {
        setError("patchJump: patchIdx " + std::to_string(patchIdx) +
                 " is out of bounds (code size = " +
                 std::to_string(currentFunc_ ? currentFunc_->code.size() : 0) + ")");
        return;
    }
    // Offset = targetIdx - patchIdx - 1  (PC is already past the JUMP instruction)
    int rawOffset = static_cast<int>(targetIdx) - static_cast<int>(patchIdx) - 1;
    if (rawOffset < -32768 || rawOffset > 32767) {
        setError("Jump offset " + std::to_string(rawOffset) +
                 " exceeds int16_t range [-32768, 32767]; code block is too large");
        return;
    }
    int16_t offset = static_cast<int16_t>(rawOffset);
    currentFunc_->code[patchIdx].regSrc1 = static_cast<uint8_t>(offset & 0xFF);
    currentFunc_->code[patchIdx].regSrc2 = static_cast<uint8_t>((offset >> 8) & 0xFF);
}

void Compiler::compileIf(const IfStmt& stmt) {
    // Check nesting depth limit
    currentNestingDepth_++;
    if (currentNestingDepth_ > MAX_NESTING_DEPTH) {
        setError("Maximum nesting depth (" + std::to_string(MAX_NESTING_DEPTH) + ") exceeded");
        currentNestingDepth_--;
        return;
    }
    if (currentNestingDepth_ > maxNestingDepth_) {
        maxNestingDepth_ = currentNestingDepth_;
    }
    
    // Compile condition
    uint8_t condReg = compileExpression(*stmt.condition);
    
    // Emit JUMP_IF_FALSE with placeholder offset
    // We'll backpatch the offset after compiling the then branch
    size_t jumpIfFalseIdx = currentFunc_->code.size();
    currentFunc_->emit(OpCode::JUMP_IF_FALSE, condReg, 0, 0);  // Placeholder
    
    // Condition register no longer needed after instruction is emitted
    freeRegister(condReg);
    
    // Compile then branch
    compileStatement(*stmt.thenBranch);
    if (hasError()) { currentNestingDepth_--; return; }
    
    // If there's an else branch, we need to jump over it after the then branch
    size_t jumpOverElseIdx = 0;
    if (stmt.elseBranch) {
        jumpOverElseIdx = currentFunc_->code.size();
        currentFunc_->emit(OpCode::JUMP, 0, 0, 0);  // Placeholder
    }
    
    // Backpatch the JUMP_IF_FALSE to jump to after the then branch (or to else)
    size_t afterThenIdx = currentFunc_->code.size();
    patchJump(jumpIfFalseIdx, afterThenIdx);
    
    // Compile else branch if present
    if (stmt.elseBranch) {
        compileStatement(*stmt.elseBranch);
        if (hasError()) { currentNestingDepth_--; return; }
        
        // Backpatch the JUMP to skip over else branch
        size_t afterElseIdx = currentFunc_->code.size();
        patchJump(jumpOverElseIdx, afterElseIdx);
    }
    
    currentNestingDepth_--;
}

void Compiler::compileFor(const ForStmt& stmt) {
    // Check nesting depth limit
    currentNestingDepth_++;
    if (currentNestingDepth_ > MAX_NESTING_DEPTH) {
        setError("Maximum nesting depth exceeded in for loop");
        currentNestingDepth_--;
        return;
    }
    if (currentNestingDepth_ > maxNestingDepth_) {
        maxNestingDepth_ = currentNestingDepth_;
    }
    
    // Push a new loop context for break/continue tracking
    loopStack_.push_back(LoopContext{});
    
    // Emit init
    if (stmt.init) {
        compileStatement(*stmt.init);
        if (hasError()) { loopStack_.pop_back(); currentNestingDepth_--; return; }
    }
    
    // loop_start: position before condition check
    size_t loopStartIdx = currentFunc_->code.size();
    
    // Emit condition check; if no condition, loop forever (until break)
    size_t jumpIfFalseIdx = SIZE_MAX;
    if (stmt.condition) {
        uint8_t condReg = compileExpression(*stmt.condition);
        jumpIfFalseIdx = currentFunc_->code.size();
        currentFunc_->emit(OpCode::JUMP_IF_FALSE, condReg, 0, 0);  // Placeholder
        freeRegister(condReg);
    }
    
    // Emit body
    if (stmt.body) {
        compileStatement(*stmt.body);
        if (hasError()) { loopStack_.pop_back(); currentNestingDepth_--; return; }
    }
    
    // update_target: continue patches jump here
    size_t updateTargetIdx = currentFunc_->code.size();
    
    // Patch all continue jumps to point here
    for (size_t patchIdx : loopStack_.back().continuePatches) {
        patchJump(patchIdx, updateTargetIdx);
    }
    
    // Emit update
    if (stmt.update) {
        compileStatement(*stmt.update);
        if (hasError()) { loopStack_.pop_back(); currentNestingDepth_--; return; }
    }
    
    // Emit backward jump to loop_start
    size_t backJumpIdx = currentFunc_->code.size();
    int rawBack = static_cast<int>(loopStartIdx) - static_cast<int>(backJumpIdx) - 1;
    if (rawBack < -32768 || rawBack > 32767) {
        setError("Backward jump offset " + std::to_string(rawBack) +
                 " exceeds int16_t range; loop body is too large");
        loopStack_.pop_back();
        currentNestingDepth_--;
        return;
    }
    int16_t backOffset = static_cast<int16_t>(rawBack);
    currentFunc_->emit(OpCode::JUMP, 0,
        static_cast<uint8_t>(backOffset & 0xFF),
        static_cast<uint8_t>((backOffset >> 8) & 0xFF));
    
    // loop_end: patch condition jump and break jumps here
    size_t loopEndIdx = currentFunc_->code.size();
    
    if (jumpIfFalseIdx != SIZE_MAX) {
        patchJump(jumpIfFalseIdx, loopEndIdx);
    }
    
    // Patch all break jumps to point to loop_end
    for (size_t patchIdx : loopStack_.back().breakPatches) {
        patchJump(patchIdx, loopEndIdx);
    }
    
    // Pop loop context
    loopStack_.pop_back();
    currentNestingDepth_--;
}

void Compiler::compileBreak(const BreakStmt&) {
    if (loopStack_.empty()) {
        setError("'break' used outside of loop");
        return;
    }
    // Emit JUMP placeholder; record index for patching
    size_t jumpIdx = currentFunc_->code.size();
    currentFunc_->emit(OpCode::JUMP, 0, 0, 0);
    loopStack_.back().breakPatches.push_back(jumpIdx);
}

void Compiler::compileContinue(const ContinueStmt&) {
    if (loopStack_.empty()) {
        setError("'continue' used outside of loop");
        return;
    }
    // Emit JUMP placeholder; record index for patching
    size_t jumpIdx = currentFunc_->code.size();
    currentFunc_->emit(OpCode::JUMP, 0, 0, 0);
    loopStack_.back().continuePatches.push_back(jumpIdx);
}

TypeKind Compiler::getExpressionType(const Expression& expr) {
    if (auto literal = dynamic_cast<const LiteralExpr*>(&expr)) {
        return literal->value.kind();  // preserve Int kind (no longer converted to Float)
    } else if (auto var = dynamic_cast<const VariableExpr*>(&expr)) {
        // Look up variable type from local declaration
        auto it = localVarTypes_.find(var->name);
        if (it != localVarTypes_.end()) {
            return it->second;
        }
        // Variable not found in type map.  This can happen if the variable was not
        // declared before use (getLocal will setError() when we actually emit code).
        // Fall back to Float so that the caller can continue; the real error will be
        // raised at the compileVariable / compileAssign site.
        assert(false && "getExpressionType: variable not in localVarTypes_ â€?declaration missing?");
        return TypeKind::Float;
    } else if (auto binary = dynamic_cast<const BinaryExpr*>(&expr)) {
        // Check for comparison and logical operators first
        // These return Bool regardless of operand types
        if (binary->op == TokenType::Greater || binary->op == TokenType::GreaterEqual ||
            binary->op == TokenType::Less || binary->op == TokenType::LessEqual ||
            binary->op == TokenType::Equal || binary->op == TokenType::NotEqual ||
            binary->op == TokenType::And || binary->op == TokenType::Or) {
            return TypeKind::Bool;
        }
        
        TypeKind leftType = getExpressionType(*binary->left);
        TypeKind rightType = getExpressionType(*binary->right);
        
        // For arithmetic operations, result type depends on operands
        if (leftType == TypeKind::Float && rightType == TypeKind::Float) {
            return TypeKind::Float;
        } else if (leftType == TypeKind::Int && rightType == TypeKind::Int) {
            return TypeKind::Int;
        } else if ((leftType == TypeKind::Float && rightType == TypeKind::Int) ||
                   (leftType == TypeKind::Int && rightType == TypeKind::Float)) {
            // Mixed int/float arithmetic coerces to float
            return TypeKind::Float;
        } else if (leftType == TypeKind::Vec2 && rightType == TypeKind::Vec2) {
            return TypeKind::Vec2;
        } else if (leftType == TypeKind::Vec3 && rightType == TypeKind::Vec3) {
            return TypeKind::Vec3;
        } else if (leftType == TypeKind::Vec4 && rightType == TypeKind::Vec4) {
            return TypeKind::Vec4;
        } else if ((leftType == TypeKind::Vec2 || leftType == TypeKind::Vec3 || leftType == TypeKind::Vec4) 
                   && rightType == TypeKind::Float) {
            return leftType;
        } else if (leftType == TypeKind::Float &&
                   (rightType == TypeKind::Vec2 || rightType == TypeKind::Vec3 || rightType == TypeKind::Vec4)) {
            // Float * Vec - return vector type
            return rightType;
        } else if (leftType == TypeKind::Mat2 && rightType == TypeKind::Mat2) {
            return TypeKind::Mat2;
        } else if (leftType == TypeKind::Mat3 && rightType == TypeKind::Mat3) {
            return TypeKind::Mat3;
        } else if (leftType == TypeKind::Mat4 && rightType == TypeKind::Mat4) {
            return TypeKind::Mat4;
        } else if (leftType == TypeKind::Mat2 && rightType == TypeKind::Vec2) {
            return TypeKind::Vec2;
        } else if (leftType == TypeKind::Mat3 && rightType == TypeKind::Vec3) {
            return TypeKind::Vec3;
        } else if (leftType == TypeKind::Mat4 && rightType == TypeKind::Vec4) {
            return TypeKind::Vec4;
        }
        
        // Default
        return TypeKind::Float;
    } else if (auto call = dynamic_cast<const CallExpr*>(&expr)) {
        // Return type depends on function
        if (call->function == "floor" || call->function == "ceil" || call->function == "fract" ||
            call->function == "sin" || call->function == "cos" || call->function == "tan" ||
            call->function == "asin" || call->function == "acos" || call->function == "atan" ||
            call->function == "radians" || call->function == "degrees" ||
            call->function == "abs" || call->function == "sign" ||
            call->function == "sqrt" || call->function == "exp" || call->function == "log" ||
            call->function == "dot" || call->function == "length" || call->function == "distance" ||
            call->function == "mod" ||
            call->function == "step" || call->function == "smoothstep") {
            return TypeKind::Float;
        } else if (call->function == "pow" || call->function == "min" ||
                   call->function == "max" || call->function == "clamp") {
            // These functions preserve the type of their first argument:
            //   pow(float,float)->float, pow(vec2,float)->vec2, pow(vec3,float)->vec3, ...
            //   min/max(vecN,vecN)->vecN, clamp(vecN,...)->vecN
            if (!call->arguments.empty()) {
                return getExpressionType(*call->arguments[0]);
            }
            return TypeKind::Float;
        } else if (call->function == "cross") {
            return TypeKind::Vec3;
        } else if (call->function == "normalize") {
            // Return type matches input type
            if (!call->arguments.empty()) {
                return getExpressionType(*call->arguments[0]);
            }
            return TypeKind::Vec3;
        } else if (call->function == "reflect" || call->function == "refract") {
            return TypeKind::Vec3;
        } else if (call->function == "mix") {
            // Return type matches first argument type
            if (!call->arguments.empty()) {
                return getExpressionType(*call->arguments[0]);
            }
            return TypeKind::Float;
        } else if (call->function == "inverse") {
            // Return type matches input matrix type
            if (!call->arguments.empty()) {
                return getExpressionType(*call->arguments[0]);
            }
            return TypeKind::Mat4;
        } else if (call->function == "transpose") {
            // Return type matches input matrix type
            if (!call->arguments.empty()) {
                return getExpressionType(*call->arguments[0]);
            }
            return TypeKind::Mat4;
        } else if (call->function == "noise1") {
            return TypeKind::Float;
        } else if (call->function == "noise2") {
            return TypeKind::Vec2;
        } else if (call->function == "noise3") {
            return TypeKind::Vec3;
        } else if (call->function == "noise4") {
            return TypeKind::Vec4;
        }
    } else if (auto unary = dynamic_cast<const UnaryExpr*>(&expr)) {
        // Unary operators preserve type (except Not which returns Bool)
        if (unary->op == TokenType::Not) {
            return TypeKind::Bool;
        }
        return getExpressionType(*unary->operand);
    } else if (auto index = dynamic_cast<const IndexExpr*>(&expr)) {
        // Array/vector indexing returns Float
        return TypeKind::Float;
    } else if (auto ctor = dynamic_cast<const ConstructorExpr*>(&expr)) {
        return ctor->type;
    } else if (auto member = dynamic_cast<const MemberAccessExpr*>(&expr)) {
        return TypeKind::Float; // Member access always returns float
    } else if (auto ternary = dynamic_cast<const TernaryExpr*>(&expr)) {
        // Ternary expression type is the type of the then/else branches (they should match)
        return getExpressionType(*ternary->thenExpr);
    }
    
    return TypeKind::Float;
}

void Compiler::checkBuiltinFunctionArgCount(const std::string& funcName, size_t actualCount) {
    // Map of builtin functions to their expected argument counts
    static const std::unordered_map<std::string, std::vector<size_t>> builtinArgCounts = {
        // 1-argument functions
        {"floor", {1}}, {"ceil", {1}}, {"fract", {1}},
        {"sin", {1}}, {"cos", {1}}, {"tan", {1}},
        {"asin", {1}}, {"acos", {1}}, {"atan", {1, 2}},
        {"radians", {1}}, {"degrees", {1}},
        {"abs", {1}}, {"sign", {1}}, {"sqrt", {1}},
        {"exp", {1}}, {"log", {1}},
        {"length", {1}}, {"normalize", {1}},
        {"noise1", {1}}, {"noise2", {1}}, {"noise3", {1}}, {"noise4", {1}},
        {"transpose", {1}}, {"inverse", {1}},
        // 2-argument functions
        {"pow", {2}}, {"mod", {2}}, {"dot", {2}}, {"cross", {2}}, {"distance", {2}},
        {"min", {2}}, {"max", {2}}, {"reflect", {2}},
        {"step", {2}},
        // 3-argument functions
        {"mix", {3}}, {"clamp", {3}}, {"smoothstep", {3}}, {"refract", {3}},
        // Variable argument constructors (handled separately)
    };
    
    auto it = builtinArgCounts.find(funcName);
    if (it != builtinArgCounts.end()) {
        const auto& expectedCounts = it->second;
        // Check if actual count matches any expected count
        bool countValid = false;
        for (size_t expected : expectedCounts) {
            if (actualCount == expected) {
                countValid = true;
                break;
            }
        }
        if (!countValid) {
            std::string expectedStr;
            for (size_t i = 0; i < expectedCounts.size(); ++i) {
                if (i > 0) expectedStr += " or ";
                expectedStr += std::to_string(expectedCounts[i]);
            }
            setError("Wrong number of arguments for '" + funcName + "' (expected " + 
                     expectedStr + ", got " + std::to_string(actualCount) + ")");
        }
    }
}

} // namespace bytecode
} // namespace mmrsl
