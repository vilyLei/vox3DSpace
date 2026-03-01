#pragma once

#include "../types.hpp"
#include <cstdint>
#include <vector>
#include <string>

namespace mmrsl {
namespace highPerf {

// Bytecode opcodes - type specialized for performance
enum class OpCode : uint8_t {
    // Load constants (operand = constant pool index)
    LOAD_CONST_FLOAT = 0,   // Load float from constant pool to register
    LOAD_CONST_INT,         // Load int from constant pool to register
    LOAD_CONST_VEC2,        // Load vec2 from constant pool to register
    LOAD_CONST_VEC3,        // Load vec3 from constant pool to register
    LOAD_CONST_VEC4,        // Load vec4 from constant pool to register
    LOAD_CONST_BOOL,        // Load bool from constant pool to register
    
    // Variable access (operand = local variable index)
    LOAD_LOCAL,             // Load local variable to register
    STORE_LOCAL,            // Store register to local variable
    
    // Float arithmetic (operands: reg_dest, reg_src1, reg_src2)
    ADD_FLOAT,
    SUB_FLOAT,
    MUL_FLOAT,
    DIV_FLOAT,
    MOD_FLOAT,
    NEG_FLOAT,              // Unary negation
    
    // Vector arithmetic - vec2
    ADD_VEC2,
    SUB_VEC2,
    MUL_VEC2,               // Component-wise multiply
    MUL_VEC2_FLOAT,         // vec2 * scalar
    DIV_VEC2_FLOAT,         // vec2 / scalar
    NEG_VEC2,
    
    // Vector arithmetic - vec3
    ADD_VEC3,
    SUB_VEC3,
    MUL_VEC3,
    MUL_VEC3_FLOAT,
    DIV_VEC3_FLOAT,
    NEG_VEC3,
    
    // Vector arithmetic - vec4
    ADD_VEC4,
    SUB_VEC4,
    MUL_VEC4,
    MUL_VEC4_FLOAT,
    DIV_VEC4_FLOAT,
    NEG_VEC4,
    
    // Built-in functions (operand = register index)
    CALL_FLOOR,             // floor(float) -> float
    CALL_CEIL,              // ceil(float) -> float
    CALL_FRACT,             // fract(float) -> float
    CALL_SIN,               // sin(float) -> float
    CALL_COS,               // cos(float) -> float
    CALL_TAN,               // tan(float) -> float
    CALL_ASIN,              // asin(float) -> float
    CALL_ACOS,              // acos(float) -> float
    CALL_ATAN,              // atan(float) -> float
    CALL_ATAN2,             // atan(float, float) -> float (atan2)
    CALL_RADIANS,           // radians(float) -> float (degrees to radians)
    CALL_DEGREES,           // degrees(float) -> float (radians to degrees)
    CALL_ABS,               // abs(float) -> float
    CALL_SIGN,              // sign(float) -> float
    CALL_SQRT,              // sqrt(float) -> float
    CALL_EXP,               // exp(float) -> float
    CALL_LOG,               // log(float) -> float
    CALL_POW,               // pow(float, float) -> float
    
    CALL_MOD_FLOAT,         // mod(float, float) -> float
    
    CALL_DOT_VEC2,          // dot(vec2, vec2) -> float
    CALL_DOT_VEC3,          // dot(vec3, vec3) -> float
    CALL_DOT_VEC4,          // dot(vec4, vec4) -> float
    CALL_CROSS_VEC3,        // cross(vec3, vec3) -> vec3
    
    // Vector operations
    LENGTH_VEC2,            // length(vec2) -> float
    LENGTH_VEC3,            // length(vec3) -> float
    LENGTH_VEC4,            // length(vec4) -> float
    DISTANCE_VEC2,          // distance(vec2, vec2) -> float
    DISTANCE_VEC3,          // distance(vec3, vec3) -> float
    DISTANCE_VEC4,          // distance(vec4, vec4) -> float
    NORMALIZE_VEC2,         // normalize(vec2) -> vec2
    NORMALIZE_VEC3,         // normalize(vec3) -> vec3
    NORMALIZE_VEC4,         // normalize(vec4) -> vec4
    REFLECT_VEC3,           // reflect(vec3 I, vec3 N) -> vec3
    REFRACT_VEC3,           // refract(vec3 I, vec3 N, float eta) -> vec3
    
    // Multi-operand parameter loading (for 3+ operand operations)
    LOAD_THIRD_PARAM,       // Load third parameter to temporary slot
    LOAD_FOURTH_PARAM,      // Load fourth parameter to temporary slot
    LOAD_EXTRA_PARAM,       // Load parameter to extraParams_[idx]
    
    CALL_MIN_FLOAT,         // min(float, float) -> float
    CALL_MAX_FLOAT,         // max(float, float) -> float
    
    // Clamp and mix functions
    CALL_CLAMP_FLOAT,       // clamp(float, float, float) -> float
    CALL_MIX_FLOAT,         // mix(float, float, float) -> float
    CALL_MIX_VEC2,          // mix(vec2, vec2, float) -> vec2
    CALL_MIX_VEC3,          // mix(vec3, vec3, float) -> vec3
    CALL_MIX_VEC4,          // mix(vec4, vec4, float) -> vec4
    
    // Step and smoothstep functions
    CALL_STEP_FLOAT,        // step(float, float) -> float
    CALL_SMOOTHSTEP_FLOAT,  // smoothstep(float, float, float) -> float
    
    // Member access (operand = register index, implicit member)
    MEMBER_X,               // Extract x component
    MEMBER_Y,               // Extract y component
    MEMBER_Z,               // Extract z component
    MEMBER_W,               // Extract w component
    
    // Matrix multiplication
    MUL_MAT2_MAT2,          // mat2 * mat2 -> mat2
    MUL_MAT3_MAT3,          // mat3 * mat3 -> mat3
    MUL_MAT4_MAT4,          // mat4 * mat4 -> mat4
    MUL_MAT2_VEC2,          // mat2 * vec2 -> vec2
    MUL_MAT3_VEC3,          // mat3 * vec3 -> vec3
    MUL_MAT4_VEC4,          // mat4 * vec4 -> vec4
    
    // Matrix element access (GLSL column-major: m[col][row])
    MAT2_INDEX,             // mat2[col][row] -> float
    MAT3_INDEX,             // mat3[col][row] -> float
    MAT4_INDEX,             // mat4[col][row] -> float
    
    // Vector element access (v[index] -> float)
    VEC2_INDEX,             // vec2[index] -> float
    VEC3_INDEX,             // vec3[index] -> float
    VEC4_INDEX,             // vec4[index] -> float
    
    // Vector constructors from components
    CONSTRUCT_VEC2,         // vec2(float, float) -> vec2
    CONSTRUCT_VEC3,         // vec3(float, float, float) -> vec3
    CONSTRUCT_VEC4,         // vec4(float, float, float, float) -> vec4
    
    // Matrix constructors
    CONSTRUCT_MAT2,         // mat2(float, float, float, float) -> mat2
    CONSTRUCT_MAT3,         // mat3(9 floats) -> mat3
    CONSTRUCT_MAT4,         // mat4(16 floats) -> mat4
    
    // Matrix operations
    INVERSE_MAT2,           // inverse(mat2) -> mat2
    INVERSE_MAT3,           // inverse(mat3) -> mat3
    INVERSE_MAT4,           // inverse(mat4) -> mat4
    TRANSPOSE_MAT2,         // transpose(mat2) -> mat2
    TRANSPOSE_MAT3,         // transpose(mat3) -> mat3
    TRANSPOSE_MAT4,         // transpose(mat4) -> mat4
    
    // Noise functions (GLSL style)
    NOISE1,                 // noise1(float) -> float
    NOISE2,                 // noise2(vec2) -> vec2
    NOISE3,                 // noise3(vec3) -> vec3
    NOISE4,                 // noise4(vec4) -> vec4
    
    // Comparison operations (result is bool)
    CMP_GT,                 // > (greater than)
    CMP_GE,                 // >= (greater or equal)
    CMP_LT,                 // < (less than)
    CMP_LE,                 // <= (less or equal)
    CMP_EQ,                 // == (equal)
    CMP_NE,                 // != (not equal)
    
    // Logical operations (result is bool)
    LOGICAL_AND,            // && (logical and)
    LOGICAL_OR,             // || (logical or)
    LOGICAL_NOT,            // ! (logical not)
    
    // Control flow
    RETURN,                 // Return value from register 0
    JUMP,                   // Unconditional jump to offset (stored in regSrc1 as 16-bit offset)
    JUMP_IF_FALSE,          // Jump if register value is false (bool)
    
    // Special
    NOP,                    // No operation
    HALT                    // Stop execution
};

// Instruction format: compact 32-bit
// For most instructions: [8-bit opcode][8-bit reg_dest][8-bit reg_src1][8-bit reg_src2]
// For load/store: [8-bit opcode][8-bit reg][16-bit index]
struct Instruction {
    OpCode opcode;
    uint8_t regDest;
    uint8_t regSrc1;
    uint8_t regSrc2;
    
    // Constructors for different instruction types
    Instruction(OpCode op, uint8_t rd, uint8_t rs1, uint8_t rs2)
        : opcode(op), regDest(rd), regSrc1(rs1), regSrc2(rs2) {}
    
    Instruction(OpCode op, uint8_t rd, uint16_t idx)
        : opcode(op), regDest(rd), regSrc1(static_cast<uint8_t>(idx & 0xFF)), 
          regSrc2(static_cast<uint8_t>((idx >> 8) & 0xFF)) {}
    
    explicit Instruction(OpCode op)
        : opcode(op), regDest(0), regSrc1(0), regSrc2(0) {}
    
    uint16_t getIndex() const {
        return static_cast<uint16_t>(regSrc1) | (static_cast<uint16_t>(regSrc2) << 8);
    }
};

static_assert(sizeof(Instruction) == 4, "Instruction must be 4 bytes");

// Bytecode function - compiled representation
struct BytecodeFunction {
    std::vector<Instruction> code;      // Instruction stream
    std::vector<Value> constants;       // Constant pool
    uint8_t numLocals;                  // Number of local variables
    uint8_t maxRegisters;               // Maximum registers used
    TypeKind returnType;                // Return type
    std::vector<TypeKind> paramTypes;   // Parameter types
    
    BytecodeFunction() : numLocals(0), maxRegisters(0), returnType(TypeKind::Void) {}
    
    // Add constant to pool, return index
    uint16_t addConstant(const Value& value);
    
    // Emit instruction
    void emit(const Instruction& inst);
    void emit(OpCode op, uint8_t rd, uint8_t rs1, uint8_t rs2);
    void emit(OpCode op, uint8_t rd, uint16_t idx);
    void emit(OpCode op);
    
    // Debug output
    std::string disassemble() const;
};

// Convert opcode to string for debugging
const char* opcodeToString(OpCode op);

} // namespace highPerf
} // namespace mmrsl
