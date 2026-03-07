#pragma once

#include "bytecode.hpp"
#include <array>
#include <stdexcept>

namespace mmrsl {
namespace bytecode {

// VM execution limits for DoS protection
// Max VM instructions executed per function call (DoS protection).
// Each loop iteration typically emits ~10 bytecode instructions (condition check,
// body, update, back-jump, etc.).  With MAX_LOOP_ITERATIONS = 1,000,000 (see
// interpreter.hpp), a worst-case loop could execute up to ~10,000,000 instructions.
// Both limits should be set consistently so neither fires before the other on
// legitimate workloads.
constexpr uint64_t MAX_INSTRUCTIONS = 10'000'000;

// VM execution error
class VMError : public std::runtime_error {
public:
    explicit VMError(const std::string& msg) : std::runtime_error(msg) {}
};

// Virtual Machine - register-based execution
class VM {
public:
    static constexpr size_t MAX_REGISTERS = 128;
    static constexpr size_t MAX_LOCALS = 128;
    
    VM();
    
    // Execute a compiled bytecode function
    Value execute(const BytecodeFunction& func, const std::vector<Value>& args);
    
    // Reset VM state
    void reset();
    
    // Get last error
    const std::string& getLastError() const { return lastError_; }
    
private:
    // Register file
    std::array<Value, MAX_REGISTERS> registers_;
    
    // Local variables (including parameters)
    std::array<Value, MAX_LOCALS> locals_;
    
    // Program counter
    size_t pc_;
    
    // Current function being executed
    const BytecodeFunction* currentFunc_;
    
    // Error state
    std::string lastError_;
    
    // DoS protection: instruction counter
    uint64_t instructionCount_;
    uint64_t maxInstructions_;
    
    // Temporary slots for multi-operand instructions
    Value thirdParam_;
    Value fourthParam_;
    std::array<Value, 16> extraParams_;  // For matrix constructors (up to 16 extra params)
    
    // Load constant to register
    void loadConstant(uint8_t reg, uint16_t idx);
    
    // Load/store local variable
    void loadLocal(uint8_t reg, uint16_t idx);
    void storeLocal(uint8_t reg, uint16_t idx);
    
    // Arithmetic operations
    void addFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void divFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void modFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void negFloat(uint8_t rd, uint8_t rs);
    
    void addVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulVec2Float(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void divVec2Float(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void addFloatVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subFloatVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void negVec2(uint8_t rd, uint8_t rs);
    
    void addVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulVec3Float(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void divVec3Float(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subVec3Float(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void addFloatVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subFloatVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void negVec3(uint8_t rd, uint8_t rs);
    
    void addVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulVec4Float(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void divVec4Float(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void addFloatVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subFloatVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void negVec4(uint8_t rd, uint8_t rs);
    
    // Built-in functions
    void callFloor(uint8_t rd, uint8_t rs);
    void callCeil(uint8_t rd, uint8_t rs);
    void callFract(uint8_t rd, uint8_t rs);
    void callSin(uint8_t rd, uint8_t rs);
    void callCos(uint8_t rd, uint8_t rs);
    void callTan(uint8_t rd, uint8_t rs);
    void callAsin(uint8_t rd, uint8_t rs);
    void callAcos(uint8_t rd, uint8_t rs);
    void callAtan(uint8_t rd, uint8_t rs);
    void callAtan2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callRadians(uint8_t rd, uint8_t rs);
    void callDegrees(uint8_t rd, uint8_t rs);
    void callAbs(uint8_t rd, uint8_t rs);
    void callSign(uint8_t rd, uint8_t rs);
    void callSqrt(uint8_t rd, uint8_t rs);
    void callExp(uint8_t rd, uint8_t rs);
    void callLog(uint8_t rd, uint8_t rs);
    void callPow(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callPowVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callPowVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callPowVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callModFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    void callDotVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callDotVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callDotVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callCrossVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Vector operations
    void lengthVec2(uint8_t rd, uint8_t rs);
    void lengthVec3(uint8_t rd, uint8_t rs);
    void lengthVec4(uint8_t rd, uint8_t rs);
    void distanceVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void distanceVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void distanceVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void normalizeVec2(uint8_t rd, uint8_t rs);
    void normalizeVec3(uint8_t rd, uint8_t rs);
    void normalizeVec4(uint8_t rd, uint8_t rs);
    void reflectVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void refractVec3(uint8_t rd, uint8_t rs1, uint8_t rs2, const Value& etaVal);
    
    void callMinFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMaxFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMinVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMaxVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMinVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMaxVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMinVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMaxVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    void callClampFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callClampVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callClampVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callClampVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMixFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMixVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMixVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callMixVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Step and smoothstep functions
    void callStepFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void callSmoothStepFloat(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Matrix multiplication
    void mulMat2Mat2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulMat3Mat3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulMat4Mat4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulMat2Vec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulMat3Vec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulMat4Vec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Matrix element access
    void mat2Index(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mat3Index(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mat4Index(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void vec2Index(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void vec3Index(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void vec4Index(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Matrix constructors
    void constructVec2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void constructVec3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void constructVec4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void constructMat2(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void constructMat3(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void constructMat4(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Matrix inverse
    void inverseMat2(uint8_t rd, uint8_t rs);
    void inverseMat3(uint8_t rd, uint8_t rs);
    void inverseMat4(uint8_t rd, uint8_t rs);
    
    // Matrix transpose
    void transposeMat2(uint8_t rd, uint8_t rs);
    void transposeMat3(uint8_t rd, uint8_t rs);
    void transposeMat4(uint8_t rd, uint8_t rs);
    
    // Noise functions
    void noise1(uint8_t rd, uint8_t rs);
    void noise2(uint8_t rd, uint8_t rs);
    void noise3(uint8_t rd, uint8_t rs);
    void noise4(uint8_t rd, uint8_t rs);
    
    // Comparison operations (result is bool)
    void cmpGt(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void cmpGe(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void cmpLt(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void cmpLe(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void cmpEq(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void cmpNe(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Logical operations
    void logicalAnd(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void logicalOr(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void logicalNot(uint8_t rd, uint8_t rs1);
    
    // Member access
    void memberX(uint8_t rd, uint8_t rs);
    void memberY(uint8_t rd, uint8_t rs);
    void memberZ(uint8_t rd, uint8_t rs);
    void memberW(uint8_t rd, uint8_t rs);
    
    // Int arithmetic
    void addInt(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void subInt(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void mulInt(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void divInt(uint8_t rd, uint8_t rs1, uint8_t rs2);
    void modInt(uint8_t rd, uint8_t rs1, uint8_t rs2);
    
    // Type conversion
    void intToFloat(uint8_t rd, uint8_t rs);
    void floatToInt(uint8_t rd, uint8_t rs);
    
    // Error handling
    void setError(const std::string& msg);
};

} // namespace bytecode
} // namespace mmrsl
