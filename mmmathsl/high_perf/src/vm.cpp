#include "mmrsl/detail/vm.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace mmrsl {
namespace highPerf {

// ============================================================================
// POD to GLM conversion helpers (internal use only)
// ============================================================================

inline glm::vec2 toGlm(const Vec2& v) { return glm::vec2(v.x, v.y); }
inline glm::vec3 toGlm(const Vec3& v) { return glm::vec3(v.x, v.y, v.z); }
inline glm::vec4 toGlm(const Vec4& v) { return glm::vec4(v.x, v.y, v.z, v.w); }

inline glm::mat2 toGlm(const Mat2& m) {
    return glm::mat2(
        m[0][0], m[0][1],
        m[1][0], m[1][1]
    );
}

inline glm::mat3 toGlm(const Mat3& m) {
    return glm::mat3(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]
    );
}

inline glm::mat4 toGlm(const Mat4& m) {
    return glm::mat4(
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3]
    );
}

inline Vec2 fromGlm(const glm::vec2& v) { return Vec2(v.x, v.y); }
inline Vec3 fromGlm(const glm::vec3& v) { return Vec3(v.x, v.y, v.z); }
inline Vec4 fromGlm(const glm::vec4& v) { return Vec4(v.x, v.y, v.z, v.w); }

inline Mat2 fromGlm(const glm::mat2& m) {
    return Mat2(
        m[0][0], m[0][1],
        m[1][0], m[1][1]
    );
}

inline Mat3 fromGlm(const glm::mat3& m) {
    return Mat3(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]
    );
}

inline Mat4 fromGlm(const glm::mat4& m) {
    return Mat4(
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3]
    );
}

VM::VM() : pc_(0), currentFunc_(nullptr), instructionCount_(0), maxInstructions_(MAX_INSTRUCTIONS) {
    reset();
}

void VM::reset() {
    pc_ = 0;
    currentFunc_ = nullptr;
    lastError_.clear();
    instructionCount_ = 0;
    registers_.fill(Value());
    locals_.fill(Value());
    thirdParam_ = Value();
    fourthParam_ = Value();
    extraParams_.fill(Value());
}

Value VM::execute(const BytecodeFunction& func, const std::vector<Value>& args) {
    reset();
    currentFunc_ = &func;
    
    // Bind arguments to local variables
    for (size_t i = 0; i < args.size() && i < MAX_LOCALS; ++i) {
        locals_[i] = args[i];
    }
    
    // Execute instructions
    while (pc_ < func.code.size()) {
        const Instruction& inst = func.code[pc_++];
        
        // Check instruction limit for DoS protection
        if (++instructionCount_ > maxInstructions_) {
            lastError_ = "Maximum instruction count (" + std::to_string(maxInstructions_) + ") exceeded";
            return Value();
        }
        
        switch (inst.opcode) {
            // Load constants
            case OpCode::LOAD_CONST_FLOAT:
            case OpCode::LOAD_CONST_INT:
            case OpCode::LOAD_CONST_VEC2:
            case OpCode::LOAD_CONST_VEC3:
            case OpCode::LOAD_CONST_VEC4:
            case OpCode::LOAD_CONST_BOOL:
                loadConstant(inst.regDest, inst.getIndex());
                break;
                
            // Variable access
            case OpCode::LOAD_LOCAL:
                loadLocal(inst.regDest, inst.getIndex());
                break;
            case OpCode::STORE_LOCAL:
                storeLocal(inst.regDest, inst.getIndex());
                break;
                
            // Float arithmetic
            case OpCode::ADD_FLOAT:
                addFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_FLOAT:
                subFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_FLOAT:
                mulFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::DIV_FLOAT:
                divFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MOD_FLOAT:
                modFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::NEG_FLOAT:
                negFloat(inst.regDest, inst.regSrc1);
                break;
                
            // Vec2 arithmetic
            case OpCode::ADD_VEC2:
                addVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_VEC2:
                subVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_VEC2:
                mulVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_VEC2_FLOAT:
                mulVec2Float(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::DIV_VEC2_FLOAT:
                divVec2Float(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::ADD_FLOAT_VEC2:
                addFloatVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_FLOAT_VEC2:
                subFloatVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::NEG_VEC2:
                negVec2(inst.regDest, inst.regSrc1);
                break;
                
            // Vec3 arithmetic
            case OpCode::ADD_VEC3:
                addVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_VEC3:
                subVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_VEC3:
                mulVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_VEC3_FLOAT:
                mulVec3Float(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::DIV_VEC3_FLOAT:
                divVec3Float(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_VEC3_FLOAT:
                subVec3Float(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::ADD_FLOAT_VEC3:
                addFloatVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_FLOAT_VEC3:
                subFloatVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::NEG_VEC3:
                negVec3(inst.regDest, inst.regSrc1);
                break;
                
            // Vec4 arithmetic
            case OpCode::ADD_VEC4:
                addVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_VEC4:
                subVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_VEC4:
                mulVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_VEC4_FLOAT:
                mulVec4Float(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::DIV_VEC4_FLOAT:
                divVec4Float(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::ADD_FLOAT_VEC4:
                addFloatVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::SUB_FLOAT_VEC4:
                subFloatVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::NEG_VEC4:
                negVec4(inst.regDest, inst.regSrc1);
                break;
                
            // Built-in functions
            case OpCode::CALL_FLOOR:
                callFloor(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_CEIL:
                callCeil(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_FRACT:
                callFract(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_SIN:
                callSin(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_COS:
                callCos(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_TAN:
                callTan(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_ASIN:
                callAsin(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_ACOS:
                callAcos(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_ATAN:
                callAtan(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_ATAN2:
                callAtan2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_RADIANS:
                callRadians(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_DEGREES:
                callDegrees(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_ABS:
                callAbs(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_SIGN:
                callSign(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_SQRT:
                callSqrt(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_EXP:
                callExp(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_LOG:
                callLog(inst.regDest, inst.regSrc1);
                break;
            case OpCode::CALL_POW:
                callPow(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_POW_VEC2:
                callPowVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_POW_VEC3:
                callPowVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_POW_VEC4:
                callPowVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MOD_FLOAT:
                callModFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_DOT_VEC2:
                callDotVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_DOT_VEC3:
                callDotVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_DOT_VEC4:
                callDotVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_CROSS_VEC3:
                callCrossVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::LENGTH_VEC2:
                lengthVec2(inst.regDest, inst.regSrc1);
                break;
            case OpCode::LENGTH_VEC3:
                lengthVec3(inst.regDest, inst.regSrc1);
                break;
            case OpCode::LENGTH_VEC4:
                lengthVec4(inst.regDest, inst.regSrc1);
                break;
            case OpCode::DISTANCE_VEC2:
                distanceVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::DISTANCE_VEC3:
                distanceVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::DISTANCE_VEC4:
                distanceVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::NORMALIZE_VEC2:
                normalizeVec2(inst.regDest, inst.regSrc1);
                break;
            case OpCode::NORMALIZE_VEC3:
                normalizeVec3(inst.regDest, inst.regSrc1);
                break;
            case OpCode::NORMALIZE_VEC4:
                normalizeVec4(inst.regDest, inst.regSrc1);
                break;
            case OpCode::REFLECT_VEC3:
                reflectVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::REFRACT_VEC3:
                // thirdParam_ contains eta
                refractVec3(inst.regDest, inst.regSrc1, inst.regSrc2, thirdParam_);
                break;
            case OpCode::LOAD_THIRD_PARAM:
                thirdParam_ = registers_[inst.regSrc1];
                break;
            case OpCode::LOAD_FOURTH_PARAM:
                fourthParam_ = registers_[inst.regSrc1];
                break;
            case OpCode::LOAD_EXTRA_PARAM:
                // regDest contains the index into extraParams_
                if (inst.regDest < extraParams_.size()) {
                    extraParams_[inst.regDest] = registers_[inst.regSrc1];
                } else {
                    setError("LOAD_EXTRA_PARAM index out of bounds");
                }
                break;
            case OpCode::CALL_MIN_FLOAT:
                callMinFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MAX_FLOAT:
                callMaxFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MIN_VEC2:
                callMinVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MAX_VEC2:
                callMaxVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MIN_VEC3:
                callMinVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MAX_VEC3:
                callMaxVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MIN_VEC4:
                callMinVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MAX_VEC4:
                callMaxVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_CLAMP_FLOAT:
                callClampFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_CLAMP_VEC2:
                callClampVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_CLAMP_VEC3:
                callClampVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_CLAMP_VEC4:
                callClampVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MIX_FLOAT:
                callMixFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MIX_VEC2:
                callMixVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MIX_VEC3:
                callMixVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_MIX_VEC4:
                callMixVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
                
            // Step and smoothstep functions
            case OpCode::CALL_STEP_FLOAT:
                callStepFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CALL_SMOOTHSTEP_FLOAT:
                callSmoothStepFloat(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
                
            // Member access
            case OpCode::MEMBER_X:
                memberX(inst.regDest, inst.regSrc1);
                break;
            case OpCode::MEMBER_Y:
                memberY(inst.regDest, inst.regSrc1);
                break;
            case OpCode::MEMBER_Z:
                memberZ(inst.regDest, inst.regSrc1);
                break;
            case OpCode::MEMBER_W:
                memberW(inst.regDest, inst.regSrc1);
                break;
                
            // Matrix multiplication
            case OpCode::MUL_MAT2_MAT2:
                mulMat2Mat2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_MAT3_MAT3:
                mulMat3Mat3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_MAT4_MAT4:
                mulMat4Mat4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_MAT2_VEC2:
                mulMat2Vec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_MAT3_VEC3:
                mulMat3Vec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MUL_MAT4_VEC4:
                mulMat4Vec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
                
            // Matrix element access
            case OpCode::MAT2_INDEX:
                mat2Index(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MAT3_INDEX:
                mat3Index(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::MAT4_INDEX:
                mat4Index(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
                
            // Vector element access
            case OpCode::VEC2_INDEX:
                vec2Index(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::VEC3_INDEX:
                vec3Index(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::VEC4_INDEX:
                vec4Index(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
                
            // Vector constructors
            case OpCode::CONSTRUCT_VEC2:
                constructVec2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CONSTRUCT_VEC3:
                constructVec3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CONSTRUCT_VEC4:
                constructVec4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CONSTRUCT_MAT2:
                constructMat2(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CONSTRUCT_MAT3:
                constructMat3(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CONSTRUCT_MAT4:
                constructMat4(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            
            // Matrix inverse operations
            case OpCode::INVERSE_MAT2:
                inverseMat2(inst.regDest, inst.regSrc1);
                break;
            case OpCode::INVERSE_MAT3:
                inverseMat3(inst.regDest, inst.regSrc1);
                break;
            case OpCode::INVERSE_MAT4:
                inverseMat4(inst.regDest, inst.regSrc1);
                break;
            
            // Matrix transpose operations
            case OpCode::TRANSPOSE_MAT2:
                transposeMat2(inst.regDest, inst.regSrc1);
                break;
            case OpCode::TRANSPOSE_MAT3:
                transposeMat3(inst.regDest, inst.regSrc1);
                break;
            case OpCode::TRANSPOSE_MAT4:
                transposeMat4(inst.regDest, inst.regSrc1);
                break;
            
            // Noise functions
            case OpCode::NOISE1:
                noise1(inst.regDest, inst.regSrc1);
                break;
            case OpCode::NOISE2:
                noise2(inst.regDest, inst.regSrc1);
                break;
            case OpCode::NOISE3:
                noise3(inst.regDest, inst.regSrc1);
                break;
            case OpCode::NOISE4:
                noise4(inst.regDest, inst.regSrc1);
                break;
            
            // Comparison operations (result is bool)
            case OpCode::CMP_GT:
                cmpGt(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CMP_GE:
                cmpGe(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CMP_LT:
                cmpLt(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CMP_LE:
                cmpLe(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CMP_EQ:
                cmpEq(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::CMP_NE:
                cmpNe(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            
            // Logical operations
            case OpCode::LOGICAL_AND:
                logicalAnd(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::LOGICAL_OR:
                logicalOr(inst.regDest, inst.regSrc1, inst.regSrc2);
                break;
            case OpCode::LOGICAL_NOT:
                logicalNot(inst.regDest, inst.regSrc1);
                break;
                
            // Move operations
            case OpCode::MOV_FLOAT:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
            case OpCode::MOV_BOOL:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
            case OpCode::MOV_VEC2:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
            case OpCode::MOV_VEC3:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
            case OpCode::MOV_VEC4:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
            case OpCode::MOV_MAT2:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
            case OpCode::MOV_MAT3:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
            case OpCode::MOV_MAT4:
                registers_[inst.regDest] = registers_[inst.regSrc1];
                break;
                
            // Control flow
            case OpCode::RETURN:
                return registers_[0];
            case OpCode::JUMP: {
                // Jump offset stored in regSrc1 and regSrc2 as 16-bit signed offset
                int16_t offset = static_cast<int16_t>((inst.regSrc2 << 8) | inst.regSrc1);
                pc_ = static_cast<size_t>(static_cast<int>(pc_) + offset);
                break;
            }
            case OpCode::JUMP_IF_FALSE: {
                // Check condition register (bool)
                bool condition = registers_[inst.regDest].asBool();
                if (!condition) {
                    // Jump offset stored in regSrc1 and regSrc2
                    int16_t offset = static_cast<int16_t>((inst.regSrc2 << 8) | inst.regSrc1);
                    pc_ = static_cast<size_t>(static_cast<int>(pc_) + offset);
                }
                break;
            }
            case OpCode::HALT:
                return registers_[0];
            case OpCode::NOP:
                break;
            default:
                setError("Unknown opcode: " + std::to_string(static_cast<int>(inst.opcode)));
                break;
        }
        
        // Check for errors after executing instruction
        if (!lastError_.empty()) {
            throw VMError(lastError_);
        }
    }
    
    return registers_[0];
}

// Load constant to register
void VM::loadConstant(uint8_t reg, uint16_t idx) {
    if (!currentFunc_ || idx >= currentFunc_->constants.size()) {
        setError("Invalid constant index");
        return;
    }
    registers_[reg] = currentFunc_->constants[idx];
}

// Load/store local variable
void VM::loadLocal(uint8_t reg, uint16_t idx) {
    if (idx >= MAX_LOCALS) {
        setError("Invalid local variable index");
        return;
    }
    registers_[reg] = locals_[idx];
}

void VM::storeLocal(uint8_t reg, uint16_t idx) {
    if (idx >= MAX_LOCALS) {
        setError("Invalid local variable index");
        return;
    }
    locals_[idx] = registers_[reg];
}

// Float arithmetic
void VM::addFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asFloat() + registers_[rs2].asFloat());
}

void VM::subFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asFloat() - registers_[rs2].asFloat());
}

void VM::mulFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asFloat() * registers_[rs2].asFloat());
}

void VM::divFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float divisor = registers_[rs2].asFloat();
    if (std::abs(divisor) < EPSILON) {
        setError("Division by zero");
        return;
    }
    registers_[rd] = Value(registers_[rs1].asFloat() / divisor);
}

void VM::modFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float divisor = registers_[rs2].asFloat();
    if (std::abs(divisor) < EPSILON) {
        setError("Modulo by zero");
        return;
    }
    registers_[rd] = Value(std::fmod(registers_[rs1].asFloat(), divisor));
}

void VM::negFloat(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(-registers_[rs].asFloat());
}

// Vec2 arithmetic
void VM::addVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec2() + registers_[rs2].asVec2());
}

void VM::addFloatVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // scalar + vec2: rs1 = scalar, rs2 = vec2
    registers_[rd] = Value(Vec2(registers_[rs1].asFloat()) + registers_[rs2].asVec2());
}

void VM::subVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec2() - registers_[rs2].asVec2());
}

void VM::subFloatVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // scalar - vec2: rs1 = scalar, rs2 = vec2
    registers_[rd] = Value(Vec2(registers_[rs1].asFloat()) - registers_[rs2].asVec2());
}

void VM::mulVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec2() * registers_[rs2].asVec2());
}

void VM::mulVec2Float(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec2() * registers_[rs2].asFloat());
}

void VM::divVec2Float(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float divisor = registers_[rs2].asFloat();
    if (std::abs(divisor) < EPSILON) {
        setError("Division by zero");
        return;
    }
    registers_[rd] = Value(registers_[rs1].asVec2() / divisor);
}

void VM::negVec2(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(-registers_[rs].asVec2());
}

// Vec3 arithmetic
void VM::addVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec3() + registers_[rs2].asVec3());
}

void VM::addFloatVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // scalar + vec3: rs1 = scalar, rs2 = vec3
    registers_[rd] = Value(Vec3(registers_[rs1].asFloat()) + registers_[rs2].asVec3());
}

void VM::subFloatVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // scalar - vec3: rs1 = scalar, rs2 = vec3
    registers_[rd] = Value(Vec3(registers_[rs1].asFloat()) - registers_[rs2].asVec3());
}

void VM::subVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec3() - registers_[rs2].asVec3());
}

void VM::mulVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec3() * registers_[rs2].asVec3());
}

void VM::mulVec3Float(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec3() * registers_[rs2].asFloat());
}

void VM::divVec3Float(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float divisor = registers_[rs2].asFloat();
    if (std::abs(divisor) < EPSILON) {
        setError("Division by zero");
        return;
    }
    registers_[rd] = Value(registers_[rs1].asVec3() / divisor);
}

void VM::subVec3Float(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // vec3 - scalar: rs1 = vec3, rs2 = scalar
    registers_[rd] = Value(registers_[rs1].asVec3() - Vec3(registers_[rs2].asFloat()));
}

void VM::negVec3(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(-registers_[rs].asVec3());
}

// Vec4 arithmetic
void VM::addVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec4() + registers_[rs2].asVec4());
}

void VM::addFloatVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // scalar + vec4: rs1 = scalar, rs2 = vec4
    registers_[rd] = Value(Vec4(registers_[rs1].asFloat()) + registers_[rs2].asVec4());
}

void VM::subVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec4() - registers_[rs2].asVec4());
}

void VM::subFloatVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // scalar - vec4: rs1 = scalar, rs2 = vec4
    registers_[rd] = Value(Vec4(registers_[rs1].asFloat()) - registers_[rs2].asVec4());
}

void VM::mulVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec4() * registers_[rs2].asVec4());
}

void VM::mulVec4Float(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(registers_[rs1].asVec4() * registers_[rs2].asFloat());
}

void VM::divVec4Float(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float divisor = registers_[rs2].asFloat();
    if (std::abs(divisor) < EPSILON) {
        setError("Division by zero");
        return;
    }
    registers_[rd] = Value(registers_[rs1].asVec4() / divisor);
}

void VM::negVec4(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(-registers_[rs].asVec4());
}

// Built-in functions
void VM::callFloor(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::floor(registers_[rs].asFloat()));
}

void VM::callCeil(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::ceil(registers_[rs].asFloat()));
}

void VM::callFract(uint8_t rd, uint8_t rs) {
    float x = registers_[rs].asFloat();
    registers_[rd] = Value(x - std::floor(x));
}

void VM::callSin(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::sin(registers_[rs].asFloat()));
}

void VM::callCos(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::cos(registers_[rs].asFloat()));
}

void VM::callTan(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::tan(registers_[rs].asFloat()));
}

void VM::callAsin(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::asin(registers_[rs].asFloat()));
}

void VM::callAcos(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::acos(registers_[rs].asFloat()));
}

void VM::callAtan(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::atan(registers_[rs].asFloat()));
}

void VM::callAtan2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(std::atan2(registers_[rs1].asFloat(), registers_[rs2].asFloat()));
}

void VM::callRadians(uint8_t rd, uint8_t rs) {
    // radians(deg) = deg * (π / 180)
    registers_[rd] = Value(registers_[rs].asFloat() * DEG_TO_RAD);
}

void VM::callDegrees(uint8_t rd, uint8_t rs) {
    // degrees(rad) = rad * (180 / π)
    registers_[rd] = Value(registers_[rs].asFloat() * RAD_TO_DEG);
}

void VM::callAbs(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::abs(registers_[rs].asFloat()));
}

void VM::callSign(uint8_t rd, uint8_t rs) {
    float x = registers_[rs].asFloat();
    float result = (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f);
    registers_[rd] = Value(result);
}

void VM::callSqrt(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::sqrt(registers_[rs].asFloat()));
}

void VM::callExp(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::exp(registers_[rs].asFloat()));
}

void VM::callLog(uint8_t rd, uint8_t rs) {
    registers_[rd] = Value(std::log(registers_[rs].asFloat()));
}

void VM::callPow(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float base = registers_[rs1].asFloat();
    float exponent = registers_[rs2].asFloat();
    registers_[rd] = Value(std::pow(base, exponent));
}

void VM::callPowVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec2 base = registers_[rs1].asVec2();
    float exponent = registers_[rs2].asFloat();
    registers_[rd] = Value(Vec2(
        std::pow(base.x, exponent),
        std::pow(base.y, exponent)
    ));
}

void VM::callPowVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec3 base = registers_[rs1].asVec3();
    float exponent = registers_[rs2].asFloat();
    registers_[rd] = Value(Vec3(
        std::pow(base.x, exponent),
        std::pow(base.y, exponent),
        std::pow(base.z, exponent)
    ));
}

void VM::callPowVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec4 base = registers_[rs1].asVec4();
    float exponent = registers_[rs2].asFloat();
    registers_[rd] = Value(Vec4(
        std::pow(base.x, exponent),
        std::pow(base.y, exponent),
        std::pow(base.z, exponent),
        std::pow(base.w, exponent)
    ));
}

void VM::callModFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float x = registers_[rs1].asFloat();
    float y = registers_[rs2].asFloat();
    if (std::abs(y) < EPSILON) {
        setError("mod() division by zero");
        return;
    }
    registers_[rd] = Value(x - y * std::floor(x / y));
}

void VM::callDotVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec2 a = registers_[rs1].asVec2();
    Vec2 b = registers_[rs2].asVec2();
    registers_[rd] = Value(a.x * b.x + a.y * b.y);
}

void VM::callDotVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec3 a = registers_[rs1].asVec3();
    Vec3 b = registers_[rs2].asVec3();
    registers_[rd] = Value(a.x * b.x + a.y * b.y + a.z * b.z);
}

void VM::callDotVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec4 a = registers_[rs1].asVec4();
    Vec4 b = registers_[rs2].asVec4();
    registers_[rd] = Value(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

void VM::callCrossVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec3 a = registers_[rs1].asVec3();
    Vec3 b = registers_[rs2].asVec3();
    // cross(a, b) = (a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x)
    registers_[rd] = Value(Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    ));
}

void VM::lengthVec2(uint8_t rd, uint8_t rs) {
    Vec2 v = registers_[rs].asVec2();
    registers_[rd] = Value(std::sqrt(v.x * v.x + v.y * v.y));
}

void VM::lengthVec3(uint8_t rd, uint8_t rs) {
    Vec3 v = registers_[rs].asVec3();
    registers_[rd] = Value(std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

void VM::lengthVec4(uint8_t rd, uint8_t rs) {
    Vec4 v = registers_[rs].asVec4();
    registers_[rd] = Value(std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));
}

void VM::distanceVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec2 a = registers_[rs1].asVec2();
    Vec2 b = registers_[rs2].asVec2();
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    registers_[rd] = Value(std::sqrt(dx * dx + dy * dy));
}

void VM::distanceVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec3 a = registers_[rs1].asVec3();
    Vec3 b = registers_[rs2].asVec3();
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    registers_[rd] = Value(std::sqrt(dx * dx + dy * dy + dz * dz));
}

void VM::distanceVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec4 a = registers_[rs1].asVec4();
    Vec4 b = registers_[rs2].asVec4();
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    float dw = a.w - b.w;
    registers_[rd] = Value(std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw));
}

void VM::normalizeVec2(uint8_t rd, uint8_t rs) {
    Vec2 v = registers_[rs].asVec2();
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len > 0.0f) {
        registers_[rd] = Value(Vec2(v.x / len, v.y / len));
    } else {
        registers_[rd] = Value(Vec2(0.0f, 0.0f));
    }
}

void VM::normalizeVec3(uint8_t rd, uint8_t rs) {
    Vec3 v = registers_[rs].asVec3();
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len > 0.0f) {
        registers_[rd] = Value(Vec3(v.x / len, v.y / len, v.z / len));
    } else {
        registers_[rd] = Value(Vec3(0.0f, 0.0f, 0.0f));
    }
}

void VM::normalizeVec4(uint8_t rd, uint8_t rs) {
    Vec4 v = registers_[rs].asVec4();
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    if (len > 0.0f) {
        registers_[rd] = Value(Vec4(v.x / len, v.y / len, v.z / len, v.w / len));
    } else {
        registers_[rd] = Value(Vec4(0.0f, 0.0f, 0.0f, 0.0f));
    }
}

void VM::reflectVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec3 I = registers_[rs1].asVec3();
    Vec3 N = registers_[rs2].asVec3();
    // reflect(I, N) = I - 2.0 * dot(N, I) * N
    float dotNI = N.x * I.x + N.y * I.y + N.z * I.z;
    registers_[rd] = Value(Vec3(
        I.x - 2.0f * dotNI * N.x,
        I.y - 2.0f * dotNI * N.y,
        I.z - 2.0f * dotNI * N.z
    ));
}

void VM::refractVec3(uint8_t rd, uint8_t rs1, uint8_t rs2, const Value& etaVal) {
    Vec3 I = registers_[rs1].asVec3();
    Vec3 N = registers_[rs2].asVec3();
    float eta = etaVal.asFloat();
    // refract(I, N, eta)
    float dotNI = N.x * I.x + N.y * I.y + N.z * I.z;
    float k = 1.0f - eta * eta * (1.0f - dotNI * dotNI);
    if (k < 0.0f) {
        registers_[rd] = Value(Vec3(0.0f, 0.0f, 0.0f));
    } else {
        float sqrtK = std::sqrt(k);
        registers_[rd] = Value(Vec3(
            eta * I.x - (eta * dotNI + sqrtK) * N.x,
            eta * I.y - (eta * dotNI + sqrtK) * N.y,
            eta * I.z - (eta * dotNI + sqrtK) * N.z
        ));
    }
}

void VM::callMinFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(std::min(registers_[rs1].asFloat(), registers_[rs2].asFloat()));
}

void VM::callMaxFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(std::max(registers_[rs1].asFloat(), registers_[rs2].asFloat()));
}

void VM::callMinVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec2 a = registers_[rs1].asVec2();
    Vec2 b = registers_[rs2].asVec2();
    registers_[rd] = Value(Vec2(
        std::min(a.x, b.x),
        std::min(a.y, b.y)
    ));
}

void VM::callMaxVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec2 a = registers_[rs1].asVec2();
    Vec2 b = registers_[rs2].asVec2();
    registers_[rd] = Value(Vec2(
        std::max(a.x, b.x),
        std::max(a.y, b.y)
    ));
}

void VM::callMinVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec3 a = registers_[rs1].asVec3();
    Vec3 b = registers_[rs2].asVec3();
    registers_[rd] = Value(Vec3(
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z)
    ));
}

void VM::callMaxVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec3 a = registers_[rs1].asVec3();
    Vec3 b = registers_[rs2].asVec3();
    registers_[rd] = Value(Vec3(
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z)
    ));
}

void VM::callMinVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec4 a = registers_[rs1].asVec4();
    Vec4 b = registers_[rs2].asVec4();
    registers_[rd] = Value(Vec4(
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z),
        std::min(a.w, b.w)
    ));
}

void VM::callMaxVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    Vec4 a = registers_[rs1].asVec4();
    Vec4 b = registers_[rs2].asVec4();
    registers_[rd] = Value(Vec4(
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z),
        std::max(a.w, b.w)
    ));
}

void VM::callClampFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // clamp(value, min, max)
    // rs1 = value, rs2 = min, thirdParam_ = max
    float value = registers_[rs1].asFloat();
    float minVal = registers_[rs2].asFloat();
    float maxVal = thirdParam_.asFloat();
    // Use std::min and std::max for C++14 compatibility
    registers_[rd] = Value(std::min(std::max(value, minVal), maxVal));
}

void VM::callClampVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // clamp(vec, min, max) - min/max are scalar
    Vec2 value = registers_[rs1].asVec2();
    float minVal = registers_[rs2].asFloat();
    float maxVal = thirdParam_.asFloat();
    registers_[rd] = Value(Vec2(
        std::min(std::max(value.x, minVal), maxVal),
        std::min(std::max(value.y, minVal), maxVal)
    ));
}

void VM::callClampVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // clamp(vec, min, max) - min/max are scalar
    Vec3 value = registers_[rs1].asVec3();
    float minVal = registers_[rs2].asFloat();
    float maxVal = thirdParam_.asFloat();
    registers_[rd] = Value(Vec3(
        std::min(std::max(value.x, minVal), maxVal),
        std::min(std::max(value.y, minVal), maxVal),
        std::min(std::max(value.z, minVal), maxVal)
    ));
}

void VM::callClampVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // clamp(vec, min, max) - min/max are scalar
    Vec4 value = registers_[rs1].asVec4();
    float minVal = registers_[rs2].asFloat();
    float maxVal = thirdParam_.asFloat();
    registers_[rd] = Value(Vec4(
        std::min(std::max(value.x, minVal), maxVal),
        std::min(std::max(value.y, minVal), maxVal),
        std::min(std::max(value.z, minVal), maxVal),
        std::min(std::max(value.w, minVal), maxVal)
    ));
}

void VM::callMixFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // mix(a, b, t) = a * (1-t) + b * t
    // rs1 = a, rs2 = b, thirdParam_ = t
    float a = registers_[rs1].asFloat();
    float b = registers_[rs2].asFloat();
    float t = thirdParam_.asFloat();
    registers_[rd] = Value(a * (1.0f - t) + b * t);
}

void VM::callMixVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // mix(a, b, t) for vec2
    Vec2 a = registers_[rs1].asVec2();
    Vec2 b = registers_[rs2].asVec2();
    float t = thirdParam_.asFloat();
    registers_[rd] = Value(a * (1.0f - t) + b * t);
}

void VM::callMixVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // mix(a, b, t) for vec3
    Vec3 a = registers_[rs1].asVec3();
    Vec3 b = registers_[rs2].asVec3();
    float t = thirdParam_.asFloat();
    registers_[rd] = Value(a * (1.0f - t) + b * t);
}

void VM::callMixVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // mix(a, b, t) for vec4
    Vec4 a = registers_[rs1].asVec4();
    Vec4 b = registers_[rs2].asVec4();
    float t = thirdParam_.asFloat();
    registers_[rd] = Value(a * (1.0f - t) + b * t);
}

// Step and smoothstep functions
void VM::callStepFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // step(edge, x) = (x < edge) ? 0.0 : 1.0
    float edge = registers_[rs1].asFloat();
    float x = registers_[rs2].asFloat();
    registers_[rd] = Value(x < edge ? 0.0f : 1.0f);
}

void VM::callSmoothStepFloat(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // smoothstep(edge0, edge1, x) - Hermite interpolation
    // rs1 = edge0, rs2 = edge1, thirdParam_ = x
    float edge0 = registers_[rs1].asFloat();
    float edge1 = registers_[rs2].asFloat();
    float x = thirdParam_.asFloat();
    
    if (x <= edge0) {
        registers_[rd] = Value(0.0f);
    } else if (x >= edge1) {
        registers_[rd] = Value(1.0f);
    } else {
        float t = (x - edge0) / (edge1 - edge0);
        t = t * t * (3.0f - 2.0f * t);  // Hermite smoothing
        registers_[rd] = Value(t);
    }
}

// Matrix multiplication
void VM::mulMat2Mat2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(fromGlm(toGlm(registers_[rs1].asMat2()) * toGlm(registers_[rs2].asMat2())));
}

void VM::mulMat3Mat3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(fromGlm(toGlm(registers_[rs1].asMat3()) * toGlm(registers_[rs2].asMat3())));
}

void VM::mulMat4Mat4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(fromGlm(toGlm(registers_[rs1].asMat4()) * toGlm(registers_[rs2].asMat4())));
}

void VM::mulMat2Vec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(fromGlm(toGlm(registers_[rs1].asMat2()) * toGlm(registers_[rs2].asVec2())));
}

void VM::mulMat3Vec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(fromGlm(toGlm(registers_[rs1].asMat3()) * toGlm(registers_[rs2].asVec3())));
}

void VM::mulMat4Vec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    registers_[rd] = Value(fromGlm(toGlm(registers_[rs1].asMat4()) * toGlm(registers_[rs2].asVec4())));
}

// Matrix element access
void VM::mat2Index(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // rs1 contains first index (column), rs2 contains matrix, extraParams_[0] contains second index (row)
    Mat2 m = registers_[rs2].asMat2();
    int col = static_cast<int>(registers_[rs1].asFloat());
    int row = static_cast<int>(extraParams_[0].asFloat());
    if (row < 0 || row >= 2 || col < 0 || col >= 2) {
        setError("mat2 index out of bounds");
        return;
    }
    registers_[rd] = Value(m[col][row]); // Column-major: m[col][row]
}

void VM::mat3Index(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // rs1 contains first index (column), rs2 contains matrix, extraParams_[0] contains second index (row)
    Mat3 m = registers_[rs2].asMat3();
    int col = static_cast<int>(registers_[rs1].asFloat());
    int row = static_cast<int>(extraParams_[0].asFloat());
    if (row < 0 || row >= 3 || col < 0 || col >= 3) {
        setError("mat3 index out of bounds");
        return;
    }
    registers_[rd] = Value(m[col][row]); // Column-major: m[col][row]
}

void VM::mat4Index(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // rs1 contains first index (column), rs2 contains matrix, extraParams_[0] contains second index (row)
    Mat4 m = registers_[rs2].asMat4();
    int col = static_cast<int>(registers_[rs1].asFloat());
    int row = static_cast<int>(extraParams_[0].asFloat());
    if (row < 0 || row >= 4 || col < 0 || col >= 4) {
        setError("mat4 index out of bounds");
        return;
    }
    registers_[rd] = Value(m[col][row]); // Column-major: m[col][row]
}

// Vector element access
void VM::vec2Index(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // rs1 contains index, rs2 contains vector
    Vec2 v = registers_[rs2].asVec2();
    int idx = static_cast<int>(registers_[rs1].asFloat());
    if (idx < 0 || idx >= 2) {
        setError("vec2 index out of bounds");
        return;
    }
    registers_[rd] = Value(idx == 0 ? v.x : v.y);
}

void VM::vec3Index(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // rs1 contains index, rs2 contains vector
    Vec3 v = registers_[rs2].asVec3();
    int idx = static_cast<int>(registers_[rs1].asFloat());
    if (idx < 0 || idx >= 3) {
        setError("vec3 index out of bounds");
        return;
    }
    registers_[rd] = Value(idx == 0 ? v.x : (idx == 1 ? v.y : v.z));
}

void VM::vec4Index(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // rs1 contains index, rs2 contains vector
    Vec4 v = registers_[rs2].asVec4();
    int idx = static_cast<int>(registers_[rs1].asFloat());
    if (idx < 0 || idx >= 4) {
        setError("vec4 index out of bounds");
        return;
    }
    registers_[rd] = Value(idx == 0 ? v.x : (idx == 1 ? v.y : (idx == 2 ? v.z : v.w)));
}

// Vector constructors
void VM::constructVec2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float x = registers_[rs1].asFloat();
    float y = registers_[rs2].asFloat();
    registers_[rd] = Value(Vec2(x, y));
}

void VM::constructVec3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // vec3(x, y, z) - x from rs1, y from rs2, z from thirdParam_
    float x = registers_[rs1].asFloat();
    float y = registers_[rs2].asFloat();
    float z = thirdParam_.asFloat();
    registers_[rd] = Value(Vec3(x, y, z));
}

void VM::constructVec4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // vec4(x, y, z, w) - x from rs1, y from rs2, z from thirdParam_, w from fourthParam_
    float x = registers_[rs1].asFloat();
    float y = registers_[rs2].asFloat();
    float z = thirdParam_.asFloat();
    float w = fourthParam_.asFloat();
    registers_[rd] = Value(Vec4(x, y, z, w));
}

void VM::constructMat2(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // mat2 requires 4 floats: rs1, rs2, extraParams_[0], extraParams_[1]
    float m00 = registers_[rs1].asFloat();
    float m01 = registers_[rs2].asFloat();
    float m10 = extraParams_[0].asFloat();
    float m11 = extraParams_[1].asFloat();
    registers_[rd] = Value(Mat2(m00, m01, m10, m11));
}

void VM::constructMat3(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // mat3 requires 9 floats: rs1, rs2, extraParams_[0..6]
    Mat3 mat;
    mat[0][0] = registers_[rs1].asFloat();
    mat[0][1] = registers_[rs2].asFloat();
    for (int i = 0; i < 7; i++) {
        mat[(i + 2) / 3][(i + 2) % 3] = extraParams_[i].asFloat();
    }
    registers_[rd] = Value(mat);
}

void VM::constructMat4(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    // mat4 requires 16 floats: rs1, rs2, extraParams_[0..13]
    Mat4 mat;
    mat[0][0] = registers_[rs1].asFloat();
    mat[0][1] = registers_[rs2].asFloat();
    for (int i = 0; i < 14; i++) {
        mat[(i + 2) / 4][(i + 2) % 4] = extraParams_[i].asFloat();
    }
    registers_[rd] = Value(mat);
}

// Matrix inverse operations
void VM::inverseMat2(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isMat2()) {
        Mat2 m = v.asMat2();
        float det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
        if (std::abs(det) < MATRIX_EPSILON) {
            setError("Matrix is singular (determinant is zero)");
            return;
        }
        // Inverse of 2x2 matrix
        float invDet = 1.0f / det;
        registers_[rd] = Value(Mat2(
            m[1][1] * invDet, -m[0][1] * invDet,
            -m[1][0] * invDet, m[0][0] * invDet
        ));
    } else {
        setError("inverse() requires mat2 argument");
    }
}

void VM::inverseMat3(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isMat3()) {
        Mat3 m = v.asMat3();
        // Use GLM for 3x3 inverse (complex calculation)
        glm::mat3 glmM = toGlm(m);
        float det = glm::determinant(glmM);
        if (std::abs(det) < MATRIX_EPSILON) {
            setError("Matrix is singular (determinant is zero)");
            return;
        }
        registers_[rd] = Value(fromGlm(glm::inverse(glmM)));
    } else {
        setError("inverse() requires mat3 argument");
    }
}

void VM::inverseMat4(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isMat4()) {
        Mat4 m = v.asMat4();
        // Use GLM for 4x4 inverse (complex calculation)
        glm::mat4 glmM = toGlm(m);
        float det = glm::determinant(glmM);
        if (std::abs(det) < MATRIX_EPSILON) {
            setError("Matrix is singular (determinant is zero)");
            return;
        }
        registers_[rd] = Value(fromGlm(glm::inverse(glmM)));
    } else {
        setError("inverse() requires mat4 argument");
    }
}

// Matrix transpose operations
void VM::transposeMat2(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isMat2()) {
        Mat2 m = v.asMat2();
        // Transpose: swap rows and columns
        registers_[rd] = Value(Mat2(
            m[0][0], m[1][0],  // Column 0 of transpose = Row 0 of original
            m[0][1], m[1][1]   // Column 1 of transpose = Row 1 of original
        ));
    } else {
        setError("transpose() requires mat2 argument");
    }
}

void VM::transposeMat3(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isMat3()) {
        Mat3 m = v.asMat3();
        registers_[rd] = Value(Mat3(
            m[0][0], m[1][0], m[2][0],
            m[0][1], m[1][1], m[2][1],
            m[0][2], m[1][2], m[2][2]
        ));
    } else {
        setError("transpose() requires mat3 argument");
    }
}

void VM::transposeMat4(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isMat4()) {
        Mat4 m = v.asMat4();
        registers_[rd] = Value(Mat4(
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]
        ));
    } else {
        setError("transpose() requires mat4 argument");
    }
}

// Noise functions - Simple hash-based noise for testing
// In production, use proper Perlin/Simplex noise implementation

// Simple hash function for noise generation
static float noiseHash(float n) {
    float v = std::sin(n * 12.9898f) * 43758.5453f;
    v = v - std::floor(v); // Extract fractional part to get [0, 1)
    return v;
}

static float noiseHash2(Vec2 p) {
    float v = std::sin(p.x * 12.9898f + p.y * 78.233f) * 43758.5453f;
    v = v - std::floor(v);
    return v;
}

static float noiseHash3(Vec3 p) {
    float v = std::sin(p.x * 12.9898f + p.y * 78.233f + p.z * 43.123f) * 43758.5453f;
    v = v - std::floor(v);
    return v;
}

static float noiseHash4(Vec4 p) {
    float v = std::sin(p.x * 12.9898f + p.y * 78.233f + p.z * 43.123f + p.w * 23.456f) * 43758.5453f;
    v = v - std::floor(v);
    return v;
}

void VM::noise1(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isFloat()) {
        float n = v.asFloat();
        // Simple value noise with interpolation
        float i = std::floor(n);
        float f = n - i;
        float a = noiseHash(i);
        float b = noiseHash(i + 1.0f);
        // Smooth interpolation
        f = f * f * (3.0f - 2.0f * f);
        registers_[rd] = Value(a + (b - a) * f);
    } else {
        setError("noise1() requires float argument");
    }
}

void VM::noise2(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isVec2()) {
        Vec2 p = v.asVec2();
        Vec2 i = Vec2(std::floor(p.x), std::floor(p.y));
        Vec2 f = Vec2(p.x - i.x, p.y - i.y);
        // Smooth interpolation (Hermite curve)
        f = Vec2(
            f.x * f.x * (3.0f - 2.0f * f.x),
            f.y * f.y * (3.0f - 2.0f * f.y)
        );
        
        // Four corners
        float a = noiseHash2(i);
        float b = noiseHash2(i + Vec2(1.0f, 0.0f));
        float c = noiseHash2(i + Vec2(0.0f, 1.0f));
        float d = noiseHash2(i + Vec2(1.0f, 1.0f));
        
        // Standard bilinear interpolation with two-step lerp
        float u = a + (b - a) * f.x;   // Bottom edge interpolation
        float v2 = c + (d - c) * f.x;  // Top edge interpolation (renamed to avoid shadowing)
        float n = u + (v2 - u) * f.y;  // Vertical interpolation
        
        registers_[rd] = Value(Vec2(n, n * 0.5f + 0.25f));
    } else {
        setError("noise2() requires vec2 argument");
    }
}

void VM::noise3(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isVec3()) {
        Vec3 p = v.asVec3();
        Vec3 i = Vec3(std::floor(p.x), std::floor(p.y), std::floor(p.z));
        Vec3 f = Vec3(p.x - i.x, p.y - i.y, p.z - i.z);
        f = Vec3(
            f.x * f.x * (3.0f - 2.0f * f.x),
            f.y * f.y * (3.0f - 2.0f * f.y),
            f.z * f.z * (3.0f - 2.0f * f.z)
        );
        
        // Sample 8 corners
        float n000 = noiseHash3(i);
        float n100 = noiseHash3(i + Vec3(1.0f, 0.0f, 0.0f));
        float n010 = noiseHash3(i + Vec3(0.0f, 1.0f, 0.0f));
        float n110 = noiseHash3(i + Vec3(1.0f, 1.0f, 0.0f));
        float n001 = noiseHash3(i + Vec3(0.0f, 0.0f, 1.0f));
        float n101 = noiseHash3(i + Vec3(1.0f, 0.0f, 1.0f));
        float n011 = noiseHash3(i + Vec3(0.0f, 1.0f, 1.0f));
        float n111 = noiseHash3(i + Vec3(1.0f, 1.0f, 1.0f));
        
        // Trilinear interpolation
        float nx00 = n000 + (n100 - n000) * f.x;
        float nx10 = n010 + (n110 - n010) * f.x;
        float nx01 = n001 + (n101 - n001) * f.x;
        float nx11 = n011 + (n111 - n011) * f.x;
        float nxy0 = nx00 + (nx10 - nx00) * f.y;
        float nxy1 = nx01 + (nx11 - nx01) * f.y;
        float nxyz = nxy0 + (nxy1 - nxy0) * f.z;
        
        registers_[rd] = Value(Vec3(nxyz, nxyz * 0.7f, nxyz * 0.3f));
    } else {
        setError("noise3() requires vec3 argument");
    }
}

void VM::noise4(uint8_t rd, uint8_t rs) {
    const Value& v4 = registers_[rs];
    if (v4.isVec4()) {
        Vec4 p = v4.asVec4();
        Vec4 i = Vec4(std::floor(p.x), std::floor(p.y), std::floor(p.z), std::floor(p.w));
        Vec4 f = Vec4(p.x - i.x, p.y - i.y, p.z - i.z, p.w - i.w);
        // Smooth interpolation (Hermite curve)
        f = Vec4(
            f.x * f.x * (3.0f - 2.0f * f.x),
            f.y * f.y * (3.0f - 2.0f * f.y),
            f.z * f.z * (3.0f - 2.0f * f.z),
            f.w * f.w * (3.0f - 2.0f * f.w)
        );
        
        // Sample 16 corners of the 4D hypercube
        float n0000 = noiseHash4(i);
        float n1000 = noiseHash4(i + Vec4(1.0f, 0.0f, 0.0f, 0.0f));
        float n0100 = noiseHash4(i + Vec4(0.0f, 1.0f, 0.0f, 0.0f));
        float n1100 = noiseHash4(i + Vec4(1.0f, 1.0f, 0.0f, 0.0f));
        float n0010 = noiseHash4(i + Vec4(0.0f, 0.0f, 1.0f, 0.0f));
        float n1010 = noiseHash4(i + Vec4(1.0f, 0.0f, 1.0f, 0.0f));
        float n0110 = noiseHash4(i + Vec4(0.0f, 1.0f, 1.0f, 0.0f));
        float n1110 = noiseHash4(i + Vec4(1.0f, 1.0f, 1.0f, 0.0f));
        float n0001 = noiseHash4(i + Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        float n1001 = noiseHash4(i + Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        float n0101 = noiseHash4(i + Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        float n1101 = noiseHash4(i + Vec4(1.0f, 1.0f, 0.0f, 1.0f));
        float n0011 = noiseHash4(i + Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        float n1011 = noiseHash4(i + Vec4(1.0f, 0.0f, 1.0f, 1.0f));
        float n0111 = noiseHash4(i + Vec4(0.0f, 1.0f, 1.0f, 1.0f));
        float n1111 = noiseHash4(i + Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        // 4D hyperlinear interpolation (4 layers of lerp)
        // Layer 1: interpolate along x
        float nx000 = n0000 + (n1000 - n0000) * f.x;
        float nx100 = n0100 + (n1100 - n0100) * f.x;
        float nx010 = n0010 + (n1010 - n0010) * f.x;
        float nx110 = n0110 + (n1110 - n0110) * f.x;
        float nx001 = n0001 + (n1001 - n0001) * f.x;
        float nx101 = n0101 + (n1101 - n0101) * f.x;
        float nx011 = n0011 + (n1011 - n0011) * f.x;
        float nx111 = n0111 + (n1111 - n0111) * f.x;
        
        // Layer 2: interpolate along y
        float nxy00 = nx000 + (nx100 - nx000) * f.y;
        float nxy10 = nx010 + (nx110 - nx010) * f.y;
        float nxy01 = nx001 + (nx101 - nx001) * f.y;
        float nxy11 = nx011 + (nx111 - nx011) * f.y;
        
        // Layer 3: interpolate along z
        float nxyz0 = nxy00 + (nxy10 - nxy00) * f.z;
        float nxyz1 = nxy01 + (nxy11 - nxy01) * f.z;
        
        // Layer 4: interpolate along w
        float nxyzw = nxyz0 + (nxyz1 - nxyz0) * f.w;
        
        registers_[rd] = Value(Vec4(nxyzw, nxyzw * 0.8f, nxyzw * 0.6f, nxyzw * 0.4f));
    } else {
        setError("noise4() requires vec4 argument");
    }
}

// Member access
void VM::memberX(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isVec2()) registers_[rd] = Value(v.asVec2().x);
    else if (v.isVec3()) registers_[rd] = Value(v.asVec3().x);
    else if (v.isVec4()) registers_[rd] = Value(v.asVec4().x);
    else setError("Cannot access .x on non-vector type");
}

void VM::memberY(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isVec2()) registers_[rd] = Value(v.asVec2().y);
    else if (v.isVec3()) registers_[rd] = Value(v.asVec3().y);
    else if (v.isVec4()) registers_[rd] = Value(v.asVec4().y);
    else setError("Cannot access .y on non-vector type");
}

void VM::memberZ(uint8_t rd, uint8_t rs) {
    const Value& v = registers_[rs];
    if (v.isVec3()) registers_[rd] = Value(v.asVec3().z);
    else if (v.isVec4()) registers_[rd] = Value(v.asVec4().z);
    else setError("Cannot access .z on non-vec3/vec4 type");
}

void VM::memberW(uint8_t rd, uint8_t rs) {
    if (registers_[rs].isVec4()) {
        registers_[rd] = Value(registers_[rs].asVec4().w);
    } else {
        setError("Cannot access .w on non-vec4 type");
    }
}

// Comparison operations (result is bool)
void VM::cmpGt(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float a = registers_[rs1].asFloat();
    float b = registers_[rs2].asFloat();
    registers_[rd] = Value(a > b);
}

void VM::cmpGe(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float a = registers_[rs1].asFloat();
    float b = registers_[rs2].asFloat();
    registers_[rd] = Value(a >= b);
}

void VM::cmpLt(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float a = registers_[rs1].asFloat();
    float b = registers_[rs2].asFloat();
    registers_[rd] = Value(a < b);
}

void VM::cmpLe(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float a = registers_[rs1].asFloat();
    float b = registers_[rs2].asFloat();
    registers_[rd] = Value(a <= b);
}

void VM::cmpEq(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float a = registers_[rs1].asFloat();
    float b = registers_[rs2].asFloat();
    registers_[rd] = Value(a == b);
}

void VM::cmpNe(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    float a = registers_[rs1].asFloat();
    float b = registers_[rs2].asFloat();
    registers_[rd] = Value(a != b);
}

void VM::logicalAnd(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    bool a = registers_[rs1].asBool();
    bool b = registers_[rs2].asBool();
    registers_[rd] = Value(a && b);
}

void VM::logicalOr(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    bool a = registers_[rs1].asBool();
    bool b = registers_[rs2].asBool();
    registers_[rd] = Value(a || b);
}

void VM::logicalNot(uint8_t rd, uint8_t rs1) {
    bool a = registers_[rs1].asBool();
    registers_[rd] = Value(!a);
}

void VM::setError(const std::string& msg) {
    lastError_ = msg;
}

} // namespace highPerf
} // namespace mmrsl
