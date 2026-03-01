#include "mmrsl/detail/interpreter.hpp"

namespace mmrsl {

// ==================== Environment ====================

void Environment::define(const std::string& name, const Value& value) {
    variables_[name] = value;
}

void Environment::assign(const std::string& name, const Value& value) {
    if (variables_.find(name) == variables_.end()) {
        throw RuntimeError("Undefined variable: " + name);
    }
    variables_[name] = value;
}

Value Environment::get(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        throw RuntimeError("Undefined variable: " + name);
    }
    return it->second;
}

bool Environment::has(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void Environment::clear() {
    variables_.clear();
}

// ==================== Interpreter ====================

Interpreter::Interpreter() = default;

Value Interpreter::execute(const FunctionDecl& function, const std::vector<Value>& arguments) {
    env_.clear();
    isReturning_ = false;  // Reset return flag at function entry
    
    // Bind parameters
    if (arguments.size() != function.parameters.size()) {
        throw RuntimeError("Argument count mismatch: expected " + 
            std::to_string(function.parameters.size()) + ", got " + 
            std::to_string(arguments.size()));
    }
    
    for (size_t i = 0; i < arguments.size(); ++i) {
        env_.define(function.parameters[i].name, arguments[i]);
    }
    
    // Execute body
    return executeCompound(*function.body);
}

Value Interpreter::execute(const Program& program, const std::vector<Value>& arguments) {
    if (!program.function) {
        throw RuntimeError("No function to execute");
    }
    return execute(*program.function, arguments);
}

Value Interpreter::executeStatement(const Statement& stmt) {
    if (auto varDecl = dynamic_cast<const VarDeclStmt*>(&stmt)) {
        return executeVarDecl(*varDecl);
    }
    if (auto assign = dynamic_cast<const AssignStmt*>(&stmt)) {
        return executeAssign(*assign);
    }
    if (auto ret = dynamic_cast<const ReturnStmt*>(&stmt)) {
        return executeReturn(*ret);
    }
    if (auto compound = dynamic_cast<const CompoundStmt*>(&stmt)) {
        return executeCompound(*compound);
    }
    if (auto ifStmt = dynamic_cast<const IfStmt*>(&stmt)) {
        return executeIf(*ifStmt);
    }
    
    throw RuntimeError("Unknown statement type");
}

Value Interpreter::executeVarDecl(const VarDeclStmt& stmt) {
    Value value;
    if (stmt.initializer) {
        value = evaluateExpression(*stmt.initializer);
    } else {
        // Default initialization
        switch (stmt.type) {
            case TypeKind::Int: value = Value(0); break;
            case TypeKind::Float: value = Value(0.0f); break;
            case TypeKind::Bool: value = Value(false); break;
            case TypeKind::Vec2: value = Value(Vec2(0.0f)); break;
            case TypeKind::Vec3: value = Value(Vec3(0.0f)); break;
            case TypeKind::Vec4: value = Value(Vec4(0.0f)); break;
            case TypeKind::Mat2: value = Value(Mat2()); break;
            case TypeKind::Mat3: value = Value(Mat3()); break;
            case TypeKind::Mat4: value = Value(Mat4()); break;
            default: value = Value(); break;
        }
    }
    
    env_.define(stmt.name, value);
    return value;
}

Value Interpreter::executeAssign(const AssignStmt& stmt) {
    Value value = evaluateExpression(*stmt.value);
    env_.assign(stmt.name, value);
    return value;
}

Value Interpreter::executeReturn(const ReturnStmt& stmt) {
    isReturning_ = true;
    return evaluateExpression(*stmt.value);
}

Value Interpreter::executeCompound(const CompoundStmt& stmt) {
    Value result;
    for (const auto& s : stmt.statements) {
        result = executeStatement(*s);
        // If return flag is set, propagate the value up
        if (isReturning_) {
            return result;
        }
    }
    return result;
}

Value Interpreter::executeIf(const IfStmt& stmt) {
    Value condValue = evaluateExpression(*stmt.condition);
    if (!condValue.isBool()) {
        throw RuntimeError("If condition must be boolean");
    }
    Value result;
    if (condValue.asBool()) {
        result = executeStatement(*stmt.thenBranch);
    } else if (stmt.elseBranch) {
        result = executeStatement(*stmt.elseBranch);
    }
    // If return flag was set in the branch, propagate it up
    if (isReturning_) {
        return result;
    }
    return Value();  // void
}

Value Interpreter::evaluateExpression(const Expression& expr) {
    if (auto binary = dynamic_cast<const BinaryExpr*>(&expr)) {
        return evaluateBinary(*binary);
    }
    if (auto unary = dynamic_cast<const UnaryExpr*>(&expr)) {
        return evaluateUnary(*unary);
    }
    if (auto literal = dynamic_cast<const LiteralExpr*>(&expr)) {
        return evaluateLiteral(*literal);
    }
    if (auto var = dynamic_cast<const VariableExpr*>(&expr)) {
        return evaluateVariable(*var);
    }
    if (auto call = dynamic_cast<const CallExpr*>(&expr)) {
        return evaluateCall(*call);
    }
    if (auto ctor = dynamic_cast<const ConstructorExpr*>(&expr)) {
        return evaluateConstructor(*ctor);
    }
    if (auto member = dynamic_cast<const MemberAccessExpr*>(&expr)) {
        return evaluateMemberAccess(*member);
    }
    if (auto index = dynamic_cast<const IndexExpr*>(&expr)) {
        return evaluateIndex(*index);
    }
    if (auto ternary = dynamic_cast<const TernaryExpr*>(&expr)) {
        return evaluateTernary(*ternary);
    }
    
    throw RuntimeError("Unknown expression type");
}

Value Interpreter::evaluateTernary(const TernaryExpr& expr) {
    Value condValue = evaluateExpression(*expr.condition);
    if (!condValue.isBool()) {
        throw RuntimeError("Ternary condition must be boolean");
    }
    
    if (condValue.asBool()) {
        return evaluateExpression(*expr.thenExpr);
    } else {
        return evaluateExpression(*expr.elseExpr);
    }
}

Value Interpreter::evaluateBinary(const BinaryExpr& expr) {
    Value left = evaluateExpression(*expr.left);
    Value right = evaluateExpression(*expr.right);
    
    // Handle vector/matrix operations
    switch (expr.op) {
        case TokenType::Plus:
            // Scalar + Scalar
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() + right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() + right.asInt());
            }
            // Scalar + Vector
            if (left.isFloat() && right.isVec2()) {
                return Value(Vec2(left.asFloat()) + right.asVec2());
            }
            if (left.isFloat() && right.isVec3()) {
                return Value(Vec3(left.asFloat()) + right.asVec3());
            }
            if (left.isFloat() && right.isVec4()) {
                return Value(Vec4(left.asFloat()) + right.asVec4());
            }
            // Vector + Scalar
            if (left.isVec2() && right.isFloat()) {
                return Value(left.asVec2() + Vec2(right.asFloat()));
            }
            if (left.isVec3() && right.isFloat()) {
                return Value(left.asVec3() + Vec3(right.asFloat()));
            }
            if (left.isVec4() && right.isFloat()) {
                return Value(left.asVec4() + Vec4(right.asFloat()));
            }
            // vec2 + vec2
            if (left.isVec2() && right.isVec2()) {
                return Value(left.asVec2() + right.asVec2());
            }
            // vec3 + vec3
            if (left.isVec3() && right.isVec3()) {
                return Value(left.asVec3() + right.asVec3());
            }
            // vec4 + vec4
            if (left.isVec4() && right.isVec4()) {
                return Value(left.asVec4() + right.asVec4());
            }
            // mat2 + mat2
            if (left.isMat2() && right.isMat2()) {
                return Value(left.asMat2() + right.asMat2());
            }
            // mat3 + mat3
            if (left.isMat3() && right.isMat3()) {
                return Value(left.asMat3() + right.asMat3());
            }
            // mat4 + mat4
            if (left.isMat4() && right.isMat4()) {
                return Value(left.asMat4() + right.asMat4());
            }
            throw RuntimeError("Invalid operands for +");
            
        case TokenType::Minus:
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() - right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() - right.asInt());
            }
            // Scalar - Vector
            if (left.isFloat() && right.isVec2()) {
                return Value(Vec2(left.asFloat()) - right.asVec2());
            }
            if (left.isFloat() && right.isVec3()) {
                return Value(Vec3(left.asFloat()) - right.asVec3());
            }
            if (left.isFloat() && right.isVec4()) {
                return Value(Vec4(left.asFloat()) - right.asVec4());
            }
            // Vector - Scalar
            if (left.isVec2() && right.isFloat()) {
                return Value(left.asVec2() - Vec2(right.asFloat()));
            }
            if (left.isVec3() && right.isFloat()) {
                return Value(left.asVec3() - Vec3(right.asFloat()));
            }
            if (left.isVec4() && right.isFloat()) {
                return Value(left.asVec4() - Vec4(right.asFloat()));
            }
            if (left.isVec2() && right.isVec2()) {
                return Value(left.asVec2() - right.asVec2());
            }
            if (left.isVec3() && right.isVec3()) {
                return Value(left.asVec3() - right.asVec3());
            }
            if (left.isVec4() && right.isVec4()) {
                return Value(left.asVec4() - right.asVec4());
            }
            // mat2 - mat2
            if (left.isMat2() && right.isMat2()) {
                return Value(left.asMat2() - right.asMat2());
            }
            // mat3 - mat3
            if (left.isMat3() && right.isMat3()) {
                return Value(left.asMat3() - right.asMat3());
            }
            // mat4 - mat4
            if (left.isMat4() && right.isMat4()) {
                return Value(left.asMat4() - right.asMat4());
            }
            throw RuntimeError("Invalid operands for -");
            
        case TokenType::Multiply:
            // Scalar * Scalar
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() * right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() * right.asInt());
            }
            // Scalar * Vector
            if (left.isFloat() && right.isVec2()) {
                return Value(left.asFloat() * right.asVec2());
            }
            if (left.isFloat() && right.isVec3()) {
                return Value(left.asFloat() * right.asVec3());
            }
            if (left.isFloat() && right.isVec4()) {
                return Value(left.asFloat() * right.asVec4());
            }
            // Vector * Scalar
            if (left.isVec2() && right.isFloat()) {
                return Value(left.asVec2() * right.asFloat());
            }
            if (left.isVec3() && right.isFloat()) {
                return Value(left.asVec3() * right.asFloat());
            }
            if (left.isVec4() && right.isFloat()) {
                return Value(left.asVec4() * right.asFloat());
            }
            // Vector * Vector (component-wise)
            if (left.isVec2() && right.isVec2()) {
                return Value(left.asVec2() * right.asVec2());
            }
            if (left.isVec3() && right.isVec3()) {
                return Value(left.asVec3() * right.asVec3());
            }
            if (left.isVec4() && right.isVec4()) {
                return Value(left.asVec4() * right.asVec4());
            }
            // Scalar * Matrix
            if (left.isFloat() && right.isMat2()) {
                return Value(left.asFloat() * right.asMat2());
            }
            if (left.isFloat() && right.isMat3()) {
                return Value(left.asFloat() * right.asMat3());
            }
            if (left.isFloat() && right.isMat4()) {
                return Value(left.asFloat() * right.asMat4());
            }
            // Matrix * Scalar
            if (left.isMat2() && right.isFloat()) {
                return Value(left.asMat2() * right.asFloat());
            }
            if (left.isMat3() && right.isFloat()) {
                return Value(left.asMat3() * right.asFloat());
            }
            if (left.isMat4() && right.isFloat()) {
                return Value(left.asMat4() * right.asFloat());
            }
            // Matrix * Matrix
            if (left.isMat2() && right.isMat2()) {
                return Value(left.asMat2() * right.asMat2());
            }
            if (left.isMat3() && right.isMat3()) {
                return Value(left.asMat3() * right.asMat3());
            }
            if (left.isMat4() && right.isMat4()) {
                return Value(left.asMat4() * right.asMat4());
            }
            // Matrix * Vector
            if (left.isMat2() && right.isVec2()) {
                return Value(left.asMat2() * right.asVec2());
            }
            if (left.isMat3() && right.isVec3()) {
                return Value(left.asMat3() * right.asVec3());
            }
            if (left.isMat4() && right.isVec4()) {
                return Value(left.asMat4() * right.asVec4());
            }
            throw RuntimeError("Invalid operands for *");
            
        case TokenType::Divide:
            if (left.isFloat() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(left.asFloat() / divisor);
            }
            if (left.isInt() && right.isInt()) {
                int divisor = right.asInt();
                if (divisor == 0) throw RuntimeError("Division by zero");
                return Value(left.asInt() / divisor);
            }
            if (left.isVec2() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(left.asVec2() / divisor);
            }
            if (left.isVec3() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(left.asVec3() / divisor);
            }
            if (left.isVec4() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(left.asVec4() / divisor);
            }
            // Matrix / Scalar
            if (left.isMat2() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(left.asMat2() / divisor);
            }
            if (left.isMat3() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(left.asMat3() / divisor);
            }
            if (left.isMat4() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(left.asMat4() / divisor);
            }
            throw RuntimeError("Invalid operands for /");
            
        case TokenType::Modulo:
            if (left.isFloat() && right.isFloat()) {
                float divisor = right.asFloat();
                if (std::abs(divisor) < 1e-12f) throw RuntimeError("Division by zero");
                return Value(std::fmod(left.asFloat(), divisor));
            }
            if (left.isInt() && right.isInt()) {
                int divisor = right.asInt();
                if (divisor == 0) throw RuntimeError("Division by zero");
                return Value(left.asInt() % divisor);
            }
            throw RuntimeError("Invalid operands for %");
            
        // Comparison operators
        case TokenType::Greater:
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() > right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() > right.asInt());
            }
            throw RuntimeError("Invalid operands for >");
            
        case TokenType::GreaterEqual:
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() >= right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() >= right.asInt());
            }
            throw RuntimeError("Invalid operands for >=");
            
        case TokenType::Less:
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() < right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() < right.asInt());
            }
            throw RuntimeError("Invalid operands for <");
            
        case TokenType::LessEqual:
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() <= right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() <= right.asInt());
            }
            throw RuntimeError("Invalid operands for <=");
            
        case TokenType::Equal:
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() == right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() == right.asInt());
            }
            if (left.isBool() && right.isBool()) {
                return Value(left.asBool() == right.asBool());
            }
            throw RuntimeError("Invalid operands for ==");
            
        case TokenType::NotEqual:
            if (left.isFloat() && right.isFloat()) {
                return Value(left.asFloat() != right.asFloat());
            }
            if (left.isInt() && right.isInt()) {
                return Value(left.asInt() != right.asInt());
            }
            if (left.isBool() && right.isBool()) {
                return Value(left.asBool() != right.asBool());
            }
            throw RuntimeError("Invalid operands for !=");
            
        // Logical operators
        case TokenType::And:
            if (left.isBool() && right.isBool()) {
                return Value(left.asBool() && right.asBool());
            }
            throw RuntimeError("Invalid operands for &&");
            
        case TokenType::Or:
            if (left.isBool() && right.isBool()) {
                return Value(left.asBool() || right.asBool());
            }
            throw RuntimeError("Invalid operands for ||");
            
        default:
            throw RuntimeError("Unknown binary operator");
    }
}

Value Interpreter::evaluateUnary(const UnaryExpr& expr) {
    Value operand = evaluateExpression(*expr.operand);
    
    switch (expr.op) {
        case TokenType::Minus:
            if (operand.isFloat()) {
                return Value(-operand.asFloat());
            }
            if (operand.isInt()) {
                return Value(-operand.asInt());
            }
            if (operand.isVec2()) {
                return Value(-operand.asVec2());
            }
            if (operand.isVec3()) {
                return Value(-operand.asVec3());
            }
            if (operand.isVec4()) {
                return Value(-operand.asVec4());
            }
            throw RuntimeError("Invalid operand for unary -");
            
        case TokenType::Plus:
            return operand;  // No-op
            
        case TokenType::Not:
            if (operand.isBool()) {
                return Value(!operand.asBool());
            }
            throw RuntimeError("Invalid operand for !");
            
        default:
            throw RuntimeError("Unknown unary operator");
    }
}

Value Interpreter::evaluateLiteral(const LiteralExpr& expr) {
    return expr.value;
}

Value Interpreter::evaluateVariable(const VariableExpr& expr) {
    return env_.get(expr.name);
}

Value Interpreter::evaluateCall(const CallExpr& expr) {
    std::vector<Value> args;
    for (const auto& arg : expr.arguments) {
        args.push_back(evaluateExpression(*arg));
    }
    return callBuiltin(expr.function, args);
}

Value Interpreter::evaluateConstructor(const ConstructorExpr& expr) {
    std::vector<Value> args;
    for (const auto& arg : expr.arguments) {
        args.push_back(evaluateExpression(*arg));
    }
    
    switch (expr.type) {
        case TypeKind::Vec2: {
            if (args.size() == 2 && args[0].isFloat() && args[1].isFloat()) {
                return Value(Vec2(args[0].asFloat(), args[1].asFloat()));
            }
            if (args.size() == 1 && args[0].isFloat()) {
                return Value(Vec2(args[0].asFloat()));
            }
            throw RuntimeError("Invalid vec2 constructor arguments");
        }
        case TypeKind::Vec3: {
            if (args.size() == 3 && args[0].isFloat() && args[1].isFloat() && args[2].isFloat()) {
                return Value(Vec3(args[0].asFloat(), args[1].asFloat(), args[2].asFloat()));
            }
            if (args.size() == 1 && args[0].isFloat()) {
                return Value(Vec3(args[0].asFloat()));
            }
            throw RuntimeError("Invalid vec3 constructor arguments");
        }
        case TypeKind::Vec4: {
            if (args.size() == 4) {
                return Value(Vec4(args[0].asFloat(), args[1].asFloat(), 
                                       args[2].asFloat(), args[3].asFloat()));
            }
            if (args.size() == 1 && args[0].isFloat()) {
                return Value(Vec4(args[0].asFloat()));
            }
            throw RuntimeError("Invalid vec4 constructor arguments");
        }
        case TypeKind::Mat2: {
            if (args.size() == 4) {
                return Value(Mat2(
                    args[0].asFloat(), args[1].asFloat(),
                    args[2].asFloat(), args[3].asFloat()
                ));
            }
            if (args.size() == 1 && args[0].isFloat()) {
                float v = args[0].asFloat();
                return Value(Mat2(v, 0.0f, 0.0f, v));  // Diagonal matrix
            }
            throw RuntimeError("Invalid mat2 constructor arguments (expected 4 floats or 1 float)");
        }
        case TypeKind::Mat3: {
            if (args.size() == 9) {
                return Value(Mat3(
                    args[0].asFloat(), args[1].asFloat(), args[2].asFloat(),
                    args[3].asFloat(), args[4].asFloat(), args[5].asFloat(),
                    args[6].asFloat(), args[7].asFloat(), args[8].asFloat()
                ));
            }
            if (args.size() == 1 && args[0].isFloat()) {
                float v = args[0].asFloat();
                return Value(Mat3(
                    v, 0.0f, 0.0f,
                    0.0f, v, 0.0f,
                    0.0f, 0.0f, v
                ));  // Diagonal matrix
            }
            throw RuntimeError("Invalid mat3 constructor arguments (expected 9 floats or 1 float)");
        }
        case TypeKind::Mat4: {
            if (args.size() == 16) {
                return Value(Mat4(
                    args[0].asFloat(), args[1].asFloat(), args[2].asFloat(), args[3].asFloat(),
                    args[4].asFloat(), args[5].asFloat(), args[6].asFloat(), args[7].asFloat(),
                    args[8].asFloat(), args[9].asFloat(), args[10].asFloat(), args[11].asFloat(),
                    args[12].asFloat(), args[13].asFloat(), args[14].asFloat(), args[15].asFloat()
                ));
            }
            if (args.size() == 1 && args[0].isFloat()) {
                float v = args[0].asFloat();
                return Value(Mat4(
                    v, 0.0f, 0.0f, 0.0f,
                    0.0f, v, 0.0f, 0.0f,
                    0.0f, 0.0f, v, 0.0f,
                    0.0f, 0.0f, 0.0f, v
                ));  // Diagonal matrix
            }
            throw RuntimeError("Invalid mat4 constructor arguments (expected 16 floats or 1 float)");
        }
        default:
            throw RuntimeError("Constructor not implemented for this type");
    }
}

Value Interpreter::evaluateMemberAccess(const MemberAccessExpr& expr) {
    Value object = evaluateExpression(*expr.object);
    
    if (object.isVec2()) {
        Vec2 v = object.asVec2();
        if (expr.member == "x" || expr.member == "r") return Value(v.x);
        if (expr.member == "y" || expr.member == "g") return Value(v.y);
        throw RuntimeError("vec2 does not have member '" + expr.member + "'");
    }
    
    if (object.isVec3()) {
        Vec3 v = object.asVec3();
        if (expr.member == "x" || expr.member == "r") return Value(v.x);
        if (expr.member == "y" || expr.member == "g") return Value(v.y);
        if (expr.member == "z" || expr.member == "b") return Value(v.z);
        throw RuntimeError("vec3 does not have member '" + expr.member + "'");
    }
    
    if (object.isVec4()) {
        Vec4 v = object.asVec4();
        if (expr.member == "x" || expr.member == "r") return Value(v.x);
        if (expr.member == "y" || expr.member == "g") return Value(v.y);
        if (expr.member == "z" || expr.member == "b") return Value(v.z);
        if (expr.member == "w" || expr.member == "a") return Value(v.w);
        throw RuntimeError("vec4 does not have member '" + expr.member + "'");
    }
    
    throw RuntimeError("Member access not supported for this type");
}

Value Interpreter::evaluateIndex(const IndexExpr& expr) {
    Value object = evaluateExpression(*expr.object);
    Value indexVal = evaluateExpression(*expr.index);
    
    if (!indexVal.isInt()) {
        throw RuntimeError("Index must be integer");
    }
    int idx = indexVal.asInt();
    
    // Handle matrix indexing with second index (m[i][j])
    if (expr.secondIndex) {
        Value idx2Val = evaluateExpression(*expr.secondIndex.value());
        if (!idx2Val.isInt()) {
            throw RuntimeError("Second index must be integer");
        }
        int idx2 = idx2Val.asInt();
        
        if (object.isMat2()) {
            Mat2 m = object.asMat2();
            if (idx < 0 || idx > 1 || idx2 < 0 || idx2 > 1) {
                throw RuntimeError("mat2 index out of bounds");
            }
            return Value(m[idx][idx2]);
        }
        if (object.isMat3()) {
            Mat3 m = object.asMat3();
            if (idx < 0 || idx > 2 || idx2 < 0 || idx2 > 2) {
                throw RuntimeError("mat3 index out of bounds");
            }
            return Value(m[idx][idx2]);
        }
        if (object.isMat4()) {
            Mat4 m = object.asMat4();
            if (idx < 0 || idx > 3 || idx2 < 0 || idx2 > 3) {
                throw RuntimeError("mat4 index out of bounds");
            }
            return Value(m[idx][idx2]);
        }
        throw RuntimeError("Double indexing not supported for this type");
    }
    
    // Single index (vector component access or matrix column access)
    if (object.isVec2()) {
        Vec2 v = object.asVec2();
        if (idx < 0 || idx > 1) throw RuntimeError("vec2 index out of bounds");
        return Value(idx == 0 ? v.x : v.y);
    }
    if (object.isVec3()) {
        Vec3 v = object.asVec3();
        if (idx < 0 || idx > 2) throw RuntimeError("vec3 index out of bounds");
        return Value(idx == 0 ? v.x : (idx == 1 ? v.y : v.z));
    }
    if (object.isVec4()) {
        Vec4 v = object.asVec4();
        if (idx < 0 || idx > 3) throw RuntimeError("vec4 index out of bounds");
        if (idx == 0) return Value(v.x);
        if (idx == 1) return Value(v.y);
        if (idx == 2) return Value(v.z);
        return Value(v.w);
    }
    if (object.isMat2()) {
        Mat2 m = object.asMat2();
        if (idx < 0 || idx > 1) throw RuntimeError("mat2 column index out of bounds");
        return Value(m[idx]);
    }
    if (object.isMat3()) {
        Mat3 m = object.asMat3();
        if (idx < 0 || idx > 2) throw RuntimeError("mat3 column index out of bounds");
        return Value(m[idx]);
    }
    if (object.isMat4()) {
        Mat4 m = object.asMat4();
        if (idx < 0 || idx > 3) throw RuntimeError("mat4 column index out of bounds");
        return Value(m[idx]);
    }
    
    throw RuntimeError("Indexing not supported for this type");
}

Value Interpreter::callBuiltin(const std::string& name, const std::vector<Value>& args) {
    if (name == "floor") {
        if (args.size() != 1) {
            throw RuntimeError("floor() takes exactly 1 argument");
        }
        if (args[0].isFloat()) {
            return Value(std::floor(args[0].asFloat()));
        }
        throw RuntimeError("floor() requires float argument");
    }
    
    if (name == "sin") {
        if (args.size() != 1) {
            throw RuntimeError("sin() takes exactly 1 argument");
        }
        if (args[0].isFloat()) {
            return Value(std::sin(args[0].asFloat()));
        }
        throw RuntimeError("sin() requires float argument");
    }
    
    if (name == "cos") {
        if (args.size() != 1) {
            throw RuntimeError("cos() takes exactly 1 argument");
        }
        if (args[0].isFloat()) {
            return Value(std::cos(args[0].asFloat()));
        }
        throw RuntimeError("cos() requires float argument");
    }
    
    if (name == "tan") {
        if (args.size() != 1) {
            throw RuntimeError("tan() takes exactly 1 argument");
        }
        if (args[0].isFloat()) {
            return Value(std::tan(args[0].asFloat()));
        }
        throw RuntimeError("tan() requires float argument");
    }
    
    if (name == "abs") {
        if (args.size() != 1) {
            throw RuntimeError("abs() takes exactly 1 argument");
        }
        if (args[0].isFloat()) {
            return Value(std::abs(args[0].asFloat()));
        }
        if (args[0].isInt()) {
            return Value(std::abs(args[0].asInt()));
        }
        throw RuntimeError("abs() requires numeric argument");
    }
    
    if (name == "sqrt") {
        if (args.size() != 1) {
            throw RuntimeError("sqrt() takes exactly 1 argument");
        }
        if (args[0].isFloat()) {
            return Value(std::sqrt(args[0].asFloat()));
        }
        throw RuntimeError("sqrt() requires float argument");
    }
    
    if (name == "min") {
        if (args.size() != 2) {
            throw RuntimeError("min() takes exactly 2 arguments");
        }
        // Scalar min
        if (args[0].isFloat() && args[1].isFloat()) {
            return Value(std::min(args[0].asFloat(), args[1].asFloat()));
        }
        // vec2 min
        if (args[0].isVec2() && args[1].isVec2()) {
            Vec2 a = args[0].asVec2();
            Vec2 b = args[1].asVec2();
            return Value(Vec2(std::min(a.x, b.x), std::min(a.y, b.y)));
        }
        // vec3 min
        if (args[0].isVec3() && args[1].isVec3()) {
            Vec3 a = args[0].asVec3();
            Vec3 b = args[1].asVec3();
            return Value(Vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)));
        }
        // vec4 min
        if (args[0].isVec4() && args[1].isVec4()) {
            Vec4 a = args[0].asVec4();
            Vec4 b = args[1].asVec4();
            return Value(Vec4(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)));
        }
        throw RuntimeError("min() requires compatible arguments");
    }
    
    if (name == "max") {
        if (args.size() != 2) {
            throw RuntimeError("max() takes exactly 2 arguments");
        }
        // Scalar max
        if (args[0].isFloat() && args[1].isFloat()) {
            return Value(std::max(args[0].asFloat(), args[1].asFloat()));
        }
        // vec2 max
        if (args[0].isVec2() && args[1].isVec2()) {
            Vec2 a = args[0].asVec2();
            Vec2 b = args[1].asVec2();
            return Value(Vec2(std::max(a.x, b.x), std::max(a.y, b.y)));
        }
        // vec3 max
        if (args[0].isVec3() && args[1].isVec3()) {
            Vec3 a = args[0].asVec3();
            Vec3 b = args[1].asVec3();
            return Value(Vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)));
        }
        // vec4 max
        if (args[0].isVec4() && args[1].isVec4()) {
            Vec4 a = args[0].asVec4();
            Vec4 b = args[1].asVec4();
            return Value(Vec4(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)));
        }
        throw RuntimeError("max() requires compatible arguments");
    }
    
    if (name == "dot") {
        if (args.size() != 2) {
            throw RuntimeError("dot() takes exactly 2 arguments");
        }
        // vec2 dot vec2
        if (args[0].isVec2() && args[1].isVec2()) {
            Vec2 a = args[0].asVec2();
            Vec2 b = args[1].asVec2();
            return Value(a.x * b.x + a.y * b.y);
        }
        // vec3 dot vec3
        if (args[0].isVec3() && args[1].isVec3()) {
            Vec3 a = args[0].asVec3();
            Vec3 b = args[1].asVec3();
            return Value(a.x * b.x + a.y * b.y + a.z * b.z);
        }
        // vec4 dot vec4
        if (args[0].isVec4() && args[1].isVec4()) {
            Vec4 a = args[0].asVec4();
            Vec4 b = args[1].asVec4();
            return Value(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
        }
        throw RuntimeError("dot() requires vectors of the same type");
    }
    
    if (name == "cross") {
        if (args.size() != 2) {
            throw RuntimeError("cross() takes exactly 2 arguments");
        }
        // vec3 cross vec3
        if (args[0].isVec3() && args[1].isVec3()) {
            Vec3 a = args[0].asVec3();
            Vec3 b = args[1].asVec3();
            return Value(Vec3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            ));
        }
        throw RuntimeError("cross() requires vec3 arguments");
    }
    
    // Additional math functions
    if (name == "ceil") {
        if (args.size() != 1) throw RuntimeError("ceil() takes exactly 1 argument");
        if (args[0].isFloat()) return Value(std::ceil(args[0].asFloat()));
        throw RuntimeError("ceil() requires float argument");
    }
    
    if (name == "fract") {
        if (args.size() != 1) throw RuntimeError("fract() takes exactly 1 argument");
        if (args[0].isFloat()) {
            float v = args[0].asFloat();
            return Value(v - std::floor(v));
        }
        throw RuntimeError("fract() requires float argument");
    }
    
    if (name == "asin") {
        if (args.size() != 1) throw RuntimeError("asin() takes exactly 1 argument");
        if (args[0].isFloat()) return Value(std::asin(args[0].asFloat()));
        throw RuntimeError("asin() requires float argument");
    }
    
    if (name == "acos") {
        if (args.size() != 1) throw RuntimeError("acos() takes exactly 1 argument");
        if (args[0].isFloat()) return Value(std::acos(args[0].asFloat()));
        throw RuntimeError("acos() requires float argument");
    }
    
    if (name == "atan") {
        if (args.size() == 1) {
            if (args[0].isFloat()) return Value(std::atan(args[0].asFloat()));
            throw RuntimeError("atan() requires float argument");
        }
        if (args.size() == 2) {
            if (args[0].isFloat() && args[1].isFloat()) {
                return Value(std::atan2(args[0].asFloat(), args[1].asFloat()));
            }
            throw RuntimeError("atan() requires float arguments");
        }
        throw RuntimeError("atan() takes 1 or 2 arguments");
    }
    
    if (name == "radians") {
        if (args.size() != 1) throw RuntimeError("radians() takes exactly 1 argument");
        if (args[0].isFloat()) return Value(args[0].asFloat() * DEG_TO_RAD);
        throw RuntimeError("radians() requires float argument");
    }
    
    if (name == "degrees") {
        if (args.size() != 1) throw RuntimeError("degrees() takes exactly 1 argument");
        if (args[0].isFloat()) return Value(args[0].asFloat() * RAD_TO_DEG);
        throw RuntimeError("degrees() requires float argument");
    }
    
    if (name == "sign") {
        if (args.size() != 1) throw RuntimeError("sign() takes exactly 1 argument");
        if (args[0].isFloat()) {
            float v = args[0].asFloat();
            return Value(v > 0.0f ? 1.0f : (v < 0.0f ? -1.0f : 0.0f));
        }
        throw RuntimeError("sign() requires float argument");
    }
    
    if (name == "exp") {
        if (args.size() != 1) throw RuntimeError("exp() takes exactly 1 argument");
        if (args[0].isFloat()) return Value(std::exp(args[0].asFloat()));
        throw RuntimeError("exp() requires float argument");
    }
    
    if (name == "log") {
        if (args.size() != 1) throw RuntimeError("log() takes exactly 1 argument");
        if (args[0].isFloat()) return Value(std::log(args[0].asFloat()));
        throw RuntimeError("log() requires float argument");
    }
    
    if (name == "pow") {
        if (args.size() != 2) throw RuntimeError("pow() takes exactly 2 arguments");
        if (args[0].isFloat() && args[1].isFloat()) {
            return Value(std::pow(args[0].asFloat(), args[1].asFloat()));
        }
        throw RuntimeError("pow() requires float arguments");
    }
    
    if (name == "mod") {
        if (args.size() != 2) throw RuntimeError("mod() takes exactly 2 arguments");
        if (args[0].isFloat() && args[1].isFloat()) {
            float x = args[0].asFloat();
            float y = args[1].asFloat();
            if (std::abs(y) < 1e-12f) throw RuntimeError("Division by zero");
            // GLSL mod: x - y * floor(x / y)
            return Value(x - y * std::floor(x / y));
        }
        throw RuntimeError("mod() requires float arguments");
    }
    
    // Vector functions
    if (name == "length") {
        if (args.size() != 1) throw RuntimeError("length() takes exactly 1 argument");
        if (args[0].isVec2()) {
            Vec2 v = args[0].asVec2();
            return Value(std::sqrt(v.x*v.x + v.y*v.y));
        }
        if (args[0].isVec3()) {
            Vec3 v = args[0].asVec3();
            return Value(std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
        }
        if (args[0].isVec4()) {
            Vec4 v = args[0].asVec4();
            return Value(std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w));
        }
        throw RuntimeError("length() requires vector argument");
    }
    
    if (name == "normalize") {
        if (args.size() != 1) throw RuntimeError("normalize() takes exactly 1 argument");
        if (args[0].isVec2()) {
            Vec2 v = args[0].asVec2();
            float len = std::sqrt(v.x*v.x + v.y*v.y);
            if (len < 1e-12f) throw RuntimeError("Cannot normalize zero vector");
            return Value(Vec2(v.x/len, v.y/len));
        }
        if (args[0].isVec3()) {
            Vec3 v = args[0].asVec3();
            float len = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
            if (len < 1e-12f) throw RuntimeError("Cannot normalize zero vector");
            return Value(Vec3(v.x/len, v.y/len, v.z/len));
        }
        if (args[0].isVec4()) {
            Vec4 v = args[0].asVec4();
            float len = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
            if (len < 1e-12f) throw RuntimeError("Cannot normalize zero vector");
            return Value(Vec4(v.x/len, v.y/len, v.z/len, v.w/len));
        }
        throw RuntimeError("normalize() requires vector argument");
    }
    
    if (name == "distance") {
        if (args.size() != 2) throw RuntimeError("distance() takes exactly 2 arguments");
        if (args[0].isVec2() && args[1].isVec2()) {
            Vec2 a = args[0].asVec2();
            Vec2 b = args[1].asVec2();
            float dx = a.x - b.x, dy = a.y - b.y;
            return Value(std::sqrt(dx*dx + dy*dy));
        }
        if (args[0].isVec3() && args[1].isVec3()) {
            Vec3 a = args[0].asVec3();
            Vec3 b = args[1].asVec3();
            float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
            return Value(std::sqrt(dx*dx + dy*dy + dz*dz));
        }
        if (args[0].isVec4() && args[1].isVec4()) {
            Vec4 a = args[0].asVec4();
            Vec4 b = args[1].asVec4();
            float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z, dw = a.w - b.w;
            return Value(std::sqrt(dx*dx + dy*dy + dz*dz + dw*dw));
        }
        throw RuntimeError("distance() requires vectors of the same type");
    }
    
    if (name == "reflect") {
        if (args.size() != 2) throw RuntimeError("reflect() takes exactly 2 arguments");
        if (args[0].isVec3() && args[1].isVec3()) {
            Vec3 I = args[0].asVec3();
            Vec3 N = args[1].asVec3();
            float dot = I.x*N.x + I.y*N.y + I.z*N.z;
            return Value(Vec3(I.x - 2.0f*dot*N.x, I.y - 2.0f*dot*N.y, I.z - 2.0f*dot*N.z));
        }
        throw RuntimeError("reflect() requires vec3 arguments");
    }
    
    // Interpolation functions
    if (name == "clamp") {
        if (args.size() != 3) throw RuntimeError("clamp() takes exactly 3 arguments");
        // Scalar clamp
        if (args[0].isFloat() && args[1].isFloat() && args[2].isFloat()) {
            float x = args[0].asFloat();
            float minVal = args[1].asFloat();
            float maxVal = args[2].asFloat();
            return Value(x < minVal ? minVal : (x > maxVal ? maxVal : x));
        }
        // vec2 clamp
        if (args[0].isVec2() && args[1].isFloat() && args[2].isFloat()) {
            Vec2 v = args[0].asVec2();
            float minVal = args[1].asFloat();
            float maxVal = args[2].asFloat();
            return Value(Vec2(
                v.x < minVal ? minVal : (v.x > maxVal ? maxVal : v.x),
                v.y < minVal ? minVal : (v.y > maxVal ? maxVal : v.y)
            ));
        }
        // vec3 clamp
        if (args[0].isVec3() && args[1].isFloat() && args[2].isFloat()) {
            Vec3 v = args[0].asVec3();
            float minVal = args[1].asFloat();
            float maxVal = args[2].asFloat();
            return Value(Vec3(
                v.x < minVal ? minVal : (v.x > maxVal ? maxVal : v.x),
                v.y < minVal ? minVal : (v.y > maxVal ? maxVal : v.y),
                v.z < minVal ? minVal : (v.z > maxVal ? maxVal : v.z)
            ));
        }
        // vec4 clamp
        if (args[0].isVec4() && args[1].isFloat() && args[2].isFloat()) {
            Vec4 v = args[0].asVec4();
            float minVal = args[1].asFloat();
            float maxVal = args[2].asFloat();
            return Value(Vec4(
                v.x < minVal ? minVal : (v.x > maxVal ? maxVal : v.x),
                v.y < minVal ? minVal : (v.y > maxVal ? maxVal : v.y),
                v.z < minVal ? minVal : (v.z > maxVal ? maxVal : v.z),
                v.w < minVal ? minVal : (v.w > maxVal ? maxVal : v.w)
            ));
        }
        throw RuntimeError("clamp() requires compatible arguments");
    }
    
    if (name == "mix") {
        if (args.size() != 3) throw RuntimeError("mix() takes exactly 3 arguments");
        // float mix
        if (args[0].isFloat() && args[1].isFloat() && args[2].isFloat()) {
            float a = args[0].asFloat();
            float b = args[1].asFloat();
            float t = args[2].asFloat();
            return Value(a + t * (b - a));
        }
        // vec3 mix
        if (args[0].isVec3() && args[1].isVec3() && args[2].isFloat()) {
            Vec3 a = args[0].asVec3();
            Vec3 b = args[1].asVec3();
            float t = args[2].asFloat();
            return Value(Vec3(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z)));
        }
        // vec2 mix
        if (args[0].isVec2() && args[1].isVec2() && args[2].isFloat()) {
            Vec2 a = args[0].asVec2();
            Vec2 b = args[1].asVec2();
            float t = args[2].asFloat();
            return Value(Vec2(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y)));
        }
        // vec4 mix
        if (args[0].isVec4() && args[1].isVec4() && args[2].isFloat()) {
            Vec4 a = args[0].asVec4();
            Vec4 b = args[1].asVec4();
            float t = args[2].asFloat();
            return Value(Vec4(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z), a.w + t * (b.w - a.w)));
        }
        throw RuntimeError("mix() requires compatible arguments");
    }
    
    if (name == "step") {
        if (args.size() != 2) throw RuntimeError("step() takes exactly 2 arguments");
        if (args[0].isFloat() && args[1].isFloat()) {
            float edge = args[0].asFloat();
            float x = args[1].asFloat();
            return Value(x < edge ? 0.0f : 1.0f);
        }
        throw RuntimeError("step() requires float arguments");
    }
    
    if (name == "smoothstep") {
        if (args.size() != 3) throw RuntimeError("smoothstep() takes exactly 3 arguments");
        if (args[0].isFloat() && args[1].isFloat() && args[2].isFloat()) {
            float edge0 = args[0].asFloat();
            float edge1 = args[1].asFloat();
            float x = args[2].asFloat();
            // Handle edge0 == edge1 case to avoid division by zero
            if (x <= edge0) return Value(0.0f);
            if (x >= edge1) return Value(1.0f);
            float t = (x - edge0) / (edge1 - edge0);
            return Value(t * t * (3.0f - 2.0f * t));
        }
        throw RuntimeError("smoothstep() requires float arguments");
    }
    
    // Refract function
    if (name == "refract") {
        if (args.size() != 3) throw RuntimeError("refract() takes exactly 3 arguments");
        if (args[0].isVec3() && args[1].isVec3() && args[2].isFloat()) {
            Vec3 I = args[0].asVec3();
            Vec3 N = args[1].asVec3();
            float eta = args[2].asFloat();
            
            float dotNI = N.x*I.x + N.y*I.y + N.z*I.z;
            float k = 1.0f - eta*eta * (1.0f - dotNI*dotNI);
            if (k < 0.0f) return Value(Vec3(0.0f, 0.0f, 0.0f)); // Total internal reflection
            
            return Value(Vec3(
                eta*I.x - (eta*dotNI + std::sqrt(k))*N.x,
                eta*I.y - (eta*dotNI + std::sqrt(k))*N.y,
                eta*I.z - (eta*dotNI + std::sqrt(k))*N.z
            ));
        }
        throw RuntimeError("refract() requires vec3, vec3, float arguments");
    }
    
    // Matrix functions
    if (name == "inverse") {
        if (args.size() != 1) throw RuntimeError("inverse() takes exactly 1 argument");
        if (args[0].isMat2()) {
            Mat2 m = args[0].asMat2();
            float det = m[0][0]*m[1][1] - m[1][0]*m[0][1];
            if (std::abs(det) < 1e-6f) throw RuntimeError("Cannot invert singular matrix");
            return Value(Mat2(
                m[1][1]/det, -m[0][1]/det,
                -m[1][0]/det, m[0][0]/det
            ));
        }
        if (args[0].isMat3()) {
            Mat3 m = args[0].asMat3();
            // Compute determinant
            float det = m[0][0]*(m[1][1]*m[2][2] - m[2][1]*m[1][2])
                      - m[1][0]*(m[0][1]*m[2][2] - m[2][1]*m[0][2])
                      + m[2][0]*(m[0][1]*m[1][2] - m[1][1]*m[0][2]);
            if (std::abs(det) < 1e-6f) throw RuntimeError("Cannot invert singular matrix");
            
            Mat3 adj;
            adj[0][0] =  (m[1][1]*m[2][2] - m[2][1]*m[1][2])/det;
            adj[0][1] = -(m[0][1]*m[2][2] - m[2][1]*m[0][2])/det;
            adj[0][2] =  (m[0][1]*m[1][2] - m[1][1]*m[0][2])/det;
            adj[1][0] = -(m[1][0]*m[2][2] - m[2][0]*m[1][2])/det;
            adj[1][1] =  (m[0][0]*m[2][2] - m[2][0]*m[0][2])/det;
            adj[1][2] = -(m[0][0]*m[1][2] - m[1][0]*m[0][2])/det;
            adj[2][0] =  (m[1][0]*m[2][1] - m[2][0]*m[1][1])/det;
            adj[2][1] = -(m[0][0]*m[2][1] - m[2][0]*m[0][1])/det;
            adj[2][2] =  (m[0][0]*m[1][1] - m[1][0]*m[0][1])/det;
            return Value(adj);
        }
        if (args[0].isMat4()) {
            Mat4 m = args[0].asMat4();
            // Compute 4x4 inverse using adjugate method
            Mat4 inv;
            
            // Helper lambda to compute 3x3 minor determinant
            auto minorDet = [&](int r0, int r1, int r2, int c0, int c1, int c2) -> float {
                return m[c0][r0] * (m[c1][r1] * m[c2][r2] - m[c2][r1] * m[c1][r2])
                     - m[c1][r0] * (m[c0][r1] * m[c2][r2] - m[c2][r1] * m[c0][r2])
                     + m[c2][r0] * (m[c0][r1] * m[c1][r2] - m[c1][r1] * m[c0][r2]);
            };
            
            // Compute cofactor matrix (transpose of adjugate)
            inv[0][0] =  minorDet(1,2,3, 1,2,3); inv[0][1] = -minorDet(0,2,3, 1,2,3); inv[0][2] =  minorDet(0,1,3, 1,2,3); inv[0][3] = -minorDet(0,1,2, 1,2,3);
            inv[1][0] = -minorDet(1,2,3, 0,2,3); inv[1][1] =  minorDet(0,2,3, 0,2,3); inv[1][2] = -minorDet(0,1,3, 0,2,3); inv[1][3] =  minorDet(0,1,2, 0,2,3);
            inv[2][0] =  minorDet(1,2,3, 0,1,3); inv[2][1] = -minorDet(0,2,3, 0,1,3); inv[2][2] =  minorDet(0,1,3, 0,1,3); inv[2][3] = -minorDet(0,1,2, 0,1,3);
            inv[3][0] = -minorDet(1,2,3, 0,1,2); inv[3][1] =  minorDet(0,2,3, 0,1,2); inv[3][2] = -minorDet(0,1,3, 0,1,2); inv[3][3] =  minorDet(0,1,2, 0,1,2);
            
            // Compute determinant
            float det = m[0][0]*inv[0][0] + m[1][0]*inv[0][1] + m[2][0]*inv[0][2] + m[3][0]*inv[0][3];
            if (std::abs(det) < 1e-6f) throw RuntimeError("Cannot invert singular matrix");
            
            // Divide by determinant
            float invDet = 1.0f / det;
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    inv[col][row] *= invDet;
                }
            }
            
            return Value(inv);
        }
        throw RuntimeError("inverse() requires mat2, mat3, or mat4 argument");
    }
    
    if (name == "transpose") {
        if (args.size() != 1) throw RuntimeError("transpose() takes exactly 1 argument");
        if (args[0].isMat2()) {
            Mat2 m = args[0].asMat2();
            return Value(Mat2(
                m[0][0], m[1][0],
                m[0][1], m[1][1]
            ));
        }
        if (args[0].isMat3()) {
            Mat3 m = args[0].asMat3();
            return Value(Mat3(
                m[0][0], m[1][0], m[2][0],
                m[0][1], m[1][1], m[2][1],
                m[0][2], m[1][2], m[2][2]
            ));
        }
        if (args[0].isMat4()) {
            Mat4 m = args[0].asMat4();
            return Value(Mat4(
                m[0][0], m[1][0], m[2][0], m[3][0],
                m[0][1], m[1][1], m[2][1], m[3][1],
                m[0][2], m[1][2], m[2][2], m[3][2],
                m[0][3], m[1][3], m[2][3], m[3][3]
            ));
        }
        throw RuntimeError("transpose() requires matrix argument");
    }
    
    // Noise functions (simplified implementation)
    // Helper to extract fractional part [0, 1) like GLSL fract()
    auto fract = [](float x) -> float {
        return x - std::floor(x);
    };
    
    if (name == "noise1") {
        if (args.size() != 1) throw RuntimeError("noise1() takes exactly 1 argument");
        if (args[0].isFloat()) {
            float x = args[0].asFloat();
            // Simple hash-based noise
            int n = static_cast<int>(std::floor(x));
            float frac = x - n;
            float r1 = fract(std::sin(n * 12.9898f) * 43758.5453f);
            float r2 = fract(std::sin((n + 1) * 12.9898f) * 43758.5453f);
            return Value(r1 + frac * (r2 - r1));
        }
        throw RuntimeError("noise1() requires float argument");
    }
    
    if (name == "noise2") {
        if (args.size() != 1) throw RuntimeError("noise2() takes exactly 1 argument");
        if (args[0].isVec2()) {
            Vec2 v = args[0].asVec2();
            float r = fract(std::sin(v.x * 12.9898f + v.y * 78.233f) * 43758.5453f);
            // Return Vec2 like VM: (n, n * 0.5 + 0.25)
            return Value(Vec2(r, r * 0.5f + 0.25f));
        }
        throw RuntimeError("noise2() requires vec2 argument");
    }
    
    if (name == "noise3") {
        if (args.size() != 1) throw RuntimeError("noise3() takes exactly 1 argument");
        if (args[0].isVec3()) {
            Vec3 v = args[0].asVec3();
            float r = fract(std::sin(v.x * 12.9898f + v.y * 78.233f + v.z * 43.123f) * 43758.5453f);
            // Return Vec3 like VM: (n, n * 0.7, n * 0.3)
            return Value(Vec3(r, r * 0.7f, r * 0.3f));
        }
        throw RuntimeError("noise3() requires vec3 argument");
    }
    
    if (name == "noise4") {
        if (args.size() != 1) throw RuntimeError("noise4() takes exactly 1 argument");
        if (args[0].isVec4()) {
            Vec4 v = args[0].asVec4();
            float r = fract(std::sin(v.x * 12.9898f + v.y * 78.233f + v.z * 43.123f + v.w * 23.456f) * 43758.5453f);
            // Return Vec4 like VM: (n, n * 0.8, n * 0.6, n * 0.4)
            return Value(Vec4(r, r * 0.8f, r * 0.6f, r * 0.4f));
        }
        throw RuntimeError("noise4() requires vec4 argument");
    }
    
    throw RuntimeError("Unknown function: " + name);
}

float Interpreter::toFloat(const Value& value) {
    if (value.isFloat()) return value.asFloat();
    if (value.isInt()) return static_cast<float>(value.asInt());
    throw RuntimeError("Cannot convert to float");
}

int Interpreter::toInt(const Value& value) {
    if (value.isInt()) return value.asInt();
    if (value.isFloat()) return static_cast<int>(value.asFloat());
    throw RuntimeError("Cannot convert to int");
}

} // namespace mmrsl
