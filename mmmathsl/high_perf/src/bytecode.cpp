#include "mmrsl/detail/bytecode.hpp"
#include "mmrsl/detail/compiler.hpp"
#include <sstream>
#include <iomanip>

namespace mmrsl {
namespace highPerf {

uint16_t BytecodeFunction::addConstant(const Value& value) {
    // Check if constant already exists
    for (size_t i = 0; i < constants.size(); ++i) {
        if (constants[i].kind() == value.kind()) {
            bool match = false;
            switch (value.kind()) {
                case TypeKind::Float:
                    match = (constants[i].asFloat() == value.asFloat());
                    break;
                case TypeKind::Int:
                    match = (constants[i].asInt() == value.asInt());
                    break;
                case TypeKind::Vec2:
                    match = (constants[i].asVec2() == value.asVec2());
                    break;
                case TypeKind::Vec3:
                    match = (constants[i].asVec3() == value.asVec3());
                    break;
                case TypeKind::Vec4:
                    match = (constants[i].asVec4() == value.asVec4());
                    break;
                case TypeKind::Mat2:
                    match = (constants[i].asMat2() == value.asMat2());
                    break;
                case TypeKind::Mat3:
                    match = (constants[i].asMat3() == value.asMat3());
                    break;
                case TypeKind::Mat4:
                    match = (constants[i].asMat4() == value.asMat4());
                    break;
                case TypeKind::Bool:
                    match = (constants[i].asBool() == value.asBool());
                    break;
                default:
                    break;
            }
            if (match) return static_cast<uint16_t>(i);
        }
    }
    
    // Add new constant
    uint16_t index = static_cast<uint16_t>(constants.size());
    constants.push_back(value);
    return index;
}

void BytecodeFunction::emit(const Instruction& inst) {
    if (code.size() >= MAX_BYTECODE_INSTRUCTIONS) {
        throw CompilerError("Bytecode instruction limit (" + std::to_string(MAX_BYTECODE_INSTRUCTIONS) + ") exceeded");
    }
    code.push_back(inst);
}

void BytecodeFunction::emit(OpCode op, uint8_t rd, uint8_t rs1, uint8_t rs2) {
    if (code.size() >= MAX_BYTECODE_INSTRUCTIONS) {
        throw CompilerError("Bytecode instruction limit (" + std::to_string(MAX_BYTECODE_INSTRUCTIONS) + ") exceeded");
    }
    code.emplace_back(op, rd, rs1, rs2);
}

void BytecodeFunction::emit(OpCode op, uint8_t rd, uint16_t idx) {
    if (code.size() >= MAX_BYTECODE_INSTRUCTIONS) {
        throw CompilerError("Bytecode instruction limit (" + std::to_string(MAX_BYTECODE_INSTRUCTIONS) + ") exceeded");
    }
    code.emplace_back(op, rd, idx);
}

void BytecodeFunction::emit(OpCode op) {
    if (code.size() >= MAX_BYTECODE_INSTRUCTIONS) {
        throw CompilerError("Bytecode instruction limit (" + std::to_string(MAX_BYTECODE_INSTRUCTIONS) + ") exceeded");
    }
    code.emplace_back(op);
}

const char* opcodeToString(OpCode op) {
    switch (op) {
        case OpCode::LOAD_CONST_FLOAT: return "LOAD_CONST_FLOAT";
        case OpCode::LOAD_CONST_INT: return "LOAD_CONST_INT";
        case OpCode::LOAD_CONST_VEC2: return "LOAD_CONST_VEC2";
        case OpCode::LOAD_CONST_VEC3: return "LOAD_CONST_VEC3";
        case OpCode::LOAD_CONST_VEC4: return "LOAD_CONST_VEC4";
        case OpCode::LOAD_CONST_BOOL: return "LOAD_CONST_BOOL";
        case OpCode::LOAD_LOCAL: return "LOAD_LOCAL";
        case OpCode::STORE_LOCAL: return "STORE_LOCAL";
        case OpCode::ADD_FLOAT: return "ADD_FLOAT";
        case OpCode::SUB_FLOAT: return "SUB_FLOAT";
        case OpCode::MUL_FLOAT: return "MUL_FLOAT";
        case OpCode::DIV_FLOAT: return "DIV_FLOAT";
        case OpCode::MOD_FLOAT: return "MOD_FLOAT";
        case OpCode::NEG_FLOAT: return "NEG_FLOAT";
        case OpCode::ADD_VEC2: return "ADD_VEC2";
        case OpCode::SUB_VEC2: return "SUB_VEC2";
        case OpCode::MUL_VEC2: return "MUL_VEC2";
        case OpCode::MUL_VEC2_FLOAT: return "MUL_VEC2_FLOAT";
        case OpCode::DIV_VEC2_FLOAT: return "DIV_VEC2_FLOAT";
        case OpCode::NEG_VEC2: return "NEG_VEC2";
        case OpCode::ADD_VEC3: return "ADD_VEC3";
        case OpCode::SUB_VEC3: return "SUB_VEC3";
        case OpCode::MUL_VEC3: return "MUL_VEC3";
        case OpCode::MUL_VEC3_FLOAT: return "MUL_VEC3_FLOAT";
        case OpCode::DIV_VEC3_FLOAT: return "DIV_VEC3_FLOAT";
        case OpCode::NEG_VEC3: return "NEG_VEC3";
        case OpCode::ADD_VEC4: return "ADD_VEC4";
        case OpCode::SUB_VEC4: return "SUB_VEC4";
        case OpCode::MUL_VEC4: return "MUL_VEC4";
        case OpCode::MUL_VEC4_FLOAT: return "MUL_VEC4_FLOAT";
        case OpCode::DIV_VEC4_FLOAT: return "DIV_VEC4_FLOAT";
        case OpCode::NEG_VEC4: return "NEG_VEC4";
        case OpCode::CALL_FLOOR: return "CALL_FLOOR";
        case OpCode::CALL_SIN: return "CALL_SIN";
        case OpCode::CALL_COS: return "CALL_COS";
        case OpCode::CALL_TAN: return "CALL_TAN";
        case OpCode::CALL_ABS: return "CALL_ABS";
        case OpCode::CALL_SQRT: return "CALL_SQRT";
        case OpCode::CALL_POW: return "CALL_POW";
        case OpCode::CALL_DOT_VEC2: return "CALL_DOT_VEC2";
        case OpCode::CALL_DOT_VEC3: return "CALL_DOT_VEC3";
        case OpCode::CALL_DOT_VEC4: return "CALL_DOT_VEC4";
        case OpCode::CALL_CROSS_VEC3: return "CALL_CROSS_VEC3";
        
        case OpCode::LENGTH_VEC2: return "LENGTH_VEC2";
        case OpCode::LENGTH_VEC3: return "LENGTH_VEC3";
        case OpCode::LENGTH_VEC4: return "LENGTH_VEC4";
        case OpCode::NORMALIZE_VEC2: return "NORMALIZE_VEC2";
        case OpCode::NORMALIZE_VEC3: return "NORMALIZE_VEC3";
        case OpCode::NORMALIZE_VEC4: return "NORMALIZE_VEC4";
        case OpCode::REFLECT_VEC3: return "REFLECT_VEC3";
        case OpCode::REFRACT_VEC3: return "REFRACT_VEC3";
        
        case OpCode::LOAD_THIRD_PARAM: return "LOAD_THIRD_PARAM";
        case OpCode::LOAD_FOURTH_PARAM: return "LOAD_FOURTH_PARAM";
        case OpCode::LOAD_EXTRA_PARAM: return "LOAD_EXTRA_PARAM";
        case OpCode::CALL_MIN_FLOAT: return "CALL_MIN_FLOAT";
        case OpCode::CALL_MAX_FLOAT: return "CALL_MAX_FLOAT";
        case OpCode::CALL_CLAMP_FLOAT: return "CALL_CLAMP_FLOAT";
        case OpCode::CALL_MIX_FLOAT: return "CALL_MIX_FLOAT";
        case OpCode::CALL_MIX_VEC2: return "CALL_MIX_VEC2";
        case OpCode::CALL_MIX_VEC3: return "CALL_MIX_VEC3";
        case OpCode::CALL_MIX_VEC4: return "CALL_MIX_VEC4";
        
        // New built-in functions
        case OpCode::CALL_CEIL: return "CALL_CEIL";
        case OpCode::CALL_FRACT: return "CALL_FRACT";
        case OpCode::CALL_ASIN: return "CALL_ASIN";
        case OpCode::CALL_ACOS: return "CALL_ACOS";
        case OpCode::CALL_ATAN: return "CALL_ATAN";
        case OpCode::CALL_RADIANS: return "CALL_RADIANS";
        case OpCode::CALL_DEGREES: return "CALL_DEGREES";
        case OpCode::CALL_SIGN: return "CALL_SIGN";
        case OpCode::CALL_EXP: return "CALL_EXP";
        case OpCode::CALL_LOG: return "CALL_LOG";
        case OpCode::CALL_MOD_FLOAT: return "CALL_MOD_FLOAT";
        case OpCode::CALL_STEP_FLOAT: return "CALL_STEP_FLOAT";
        case OpCode::CALL_SMOOTHSTEP_FLOAT: return "CALL_SMOOTHSTEP_FLOAT";
        case OpCode::DISTANCE_VEC2: return "DISTANCE_VEC2";
        case OpCode::DISTANCE_VEC3: return "DISTANCE_VEC3";
        case OpCode::DISTANCE_VEC4: return "DISTANCE_VEC4";
        
        // Matrix multiplication
        case OpCode::MUL_MAT2_MAT2: return "MUL_MAT2_MAT2";
        case OpCode::MUL_MAT3_MAT3: return "MUL_MAT3_MAT3";
        case OpCode::MUL_MAT4_MAT4: return "MUL_MAT4_MAT4";
        case OpCode::MUL_MAT2_VEC2: return "MUL_MAT2_VEC2";
        case OpCode::MUL_MAT3_VEC3: return "MUL_MAT3_VEC3";
        case OpCode::MUL_MAT4_VEC4: return "MUL_MAT4_VEC4";
        
        case OpCode::MEMBER_X: return "MEMBER_X";
        case OpCode::MEMBER_Y: return "MEMBER_Y";
        case OpCode::MEMBER_Z: return "MEMBER_Z";
        case OpCode::MEMBER_W: return "MEMBER_W";
        case OpCode::MAT2_INDEX: return "MAT2_INDEX";
        case OpCode::MAT3_INDEX: return "MAT3_INDEX";
        case OpCode::MAT4_INDEX: return "MAT4_INDEX";
        case OpCode::VEC2_INDEX: return "VEC2_INDEX";
        case OpCode::VEC3_INDEX: return "VEC3_INDEX";
        case OpCode::VEC4_INDEX: return "VEC4_INDEX";
        case OpCode::CONSTRUCT_VEC2: return "CONSTRUCT_VEC2";
        case OpCode::CONSTRUCT_VEC3: return "CONSTRUCT_VEC3";
        case OpCode::CONSTRUCT_VEC4: return "CONSTRUCT_VEC4";
        
        case OpCode::CONSTRUCT_MAT2: return "CONSTRUCT_MAT2";
        case OpCode::CONSTRUCT_MAT3: return "CONSTRUCT_MAT3";
        case OpCode::CONSTRUCT_MAT4: return "CONSTRUCT_MAT4";
        
        case OpCode::INVERSE_MAT2: return "INVERSE_MAT2";
        case OpCode::INVERSE_MAT3: return "INVERSE_MAT3";
        case OpCode::INVERSE_MAT4: return "INVERSE_MAT4";
        case OpCode::TRANSPOSE_MAT2: return "TRANSPOSE_MAT2";
        case OpCode::TRANSPOSE_MAT3: return "TRANSPOSE_MAT3";
        case OpCode::TRANSPOSE_MAT4: return "TRANSPOSE_MAT4";
        
        case OpCode::NOISE1: return "NOISE1";
        case OpCode::NOISE2: return "NOISE2";
        case OpCode::NOISE3: return "NOISE3";
        case OpCode::NOISE4: return "NOISE4";
        
        case OpCode::CMP_GT: return "CMP_GT";
        case OpCode::CMP_GE: return "CMP_GE";
        case OpCode::CMP_LT: return "CMP_LT";
        case OpCode::CMP_LE: return "CMP_LE";
        case OpCode::CMP_EQ: return "CMP_EQ";
        case OpCode::CMP_NE: return "CMP_NE";
        case OpCode::LOGICAL_AND: return "LOGICAL_AND";
        case OpCode::LOGICAL_OR: return "LOGICAL_OR";
        case OpCode::LOGICAL_NOT: return "LOGICAL_NOT";
        
        case OpCode::RETURN: return "RETURN";
        case OpCode::JUMP: return "JUMP";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::ADD_INT: return "ADD_INT";
        case OpCode::SUB_INT: return "SUB_INT";
        case OpCode::MUL_INT: return "MUL_INT";
        case OpCode::DIV_INT: return "DIV_INT";
        case OpCode::MOD_INT: return "MOD_INT";
        case OpCode::INT_TO_FLOAT: return "INT_TO_FLOAT";
        case OpCode::FLOAT_TO_INT: return "FLOAT_TO_INT";
        case OpCode::NOP: return "NOP";
        case OpCode::HALT: return "HALT";
    }
    return "UNKNOWN";
}

std::string BytecodeFunction::disassemble() const {
    std::ostringstream oss;
    oss << "=== Bytecode Function ===\n";
    oss << "Constants:\n";
    for (size_t i = 0; i < constants.size(); ++i) {
        oss << "  [" << i << "] " << constants[i].toString() << "\n";
    }
    
    oss << "\nCode:\n";
    for (size_t i = 0; i < code.size(); ++i) {
        const auto& inst = code[i];
        oss << std::setw(4) << i << ": " << opcodeToString(inst.opcode);
        
        // Format based on opcode type
        switch (inst.opcode) {
            case OpCode::LOAD_CONST_FLOAT:
            case OpCode::LOAD_CONST_INT:
            case OpCode::LOAD_CONST_VEC2:
            case OpCode::LOAD_CONST_VEC3:
            case OpCode::LOAD_CONST_VEC4:
            case OpCode::LOAD_CONST_BOOL:
            case OpCode::LOAD_LOCAL:
            case OpCode::STORE_LOCAL:
                oss << " r" << (int)inst.regDest << ", [" << inst.getIndex() << "]";
                break;
            case OpCode::ADD_FLOAT:
            case OpCode::SUB_FLOAT:
            case OpCode::MUL_FLOAT:
            case OpCode::DIV_FLOAT:
            case OpCode::MOD_FLOAT:
            case OpCode::ADD_VEC2:
            case OpCode::SUB_VEC2:
            case OpCode::MUL_VEC2:
            case OpCode::ADD_VEC3:
            case OpCode::SUB_VEC3:
            case OpCode::MUL_VEC3:
            case OpCode::ADD_VEC4:
            case OpCode::SUB_VEC4:
            case OpCode::MUL_VEC4:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            case OpCode::MUL_VEC2_FLOAT:
            case OpCode::DIV_VEC2_FLOAT:
            case OpCode::MUL_VEC3_FLOAT:
            case OpCode::DIV_VEC3_FLOAT:
            case OpCode::MUL_VEC4_FLOAT:
            case OpCode::DIV_VEC4_FLOAT:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            case OpCode::NEG_FLOAT:
            case OpCode::NEG_VEC2:
            case OpCode::NEG_VEC3:
            case OpCode::NEG_VEC4:
            case OpCode::CALL_FLOOR:
            case OpCode::CALL_CEIL:
            case OpCode::CALL_FRACT:
            case OpCode::CALL_SIN:
            case OpCode::CALL_COS:
            case OpCode::CALL_TAN:
            case OpCode::CALL_ASIN:
            case OpCode::CALL_ACOS:
            case OpCode::CALL_ATAN:
            case OpCode::CALL_RADIANS:
            case OpCode::CALL_DEGREES:
            case OpCode::CALL_ABS:
            case OpCode::CALL_SIGN:
            case OpCode::CALL_SQRT:
            case OpCode::CALL_EXP:
            case OpCode::CALL_LOG:
            case OpCode::INVERSE_MAT2:
            case OpCode::INVERSE_MAT3:
            case OpCode::INVERSE_MAT4:
            case OpCode::TRANSPOSE_MAT2:
            case OpCode::TRANSPOSE_MAT3:
            case OpCode::TRANSPOSE_MAT4:
            case OpCode::NOISE1:
            case OpCode::NOISE2:
            case OpCode::NOISE3:
            case OpCode::NOISE4:
            case OpCode::MEMBER_X:
            case OpCode::MEMBER_Y:
            case OpCode::MEMBER_Z:
            case OpCode::MEMBER_W:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1;
                break;
            case OpCode::CALL_DOT_VEC2:
            case OpCode::CALL_DOT_VEC3:
            case OpCode::CALL_DOT_VEC4:
            case OpCode::CALL_CROSS_VEC3:
            case OpCode::CALL_MOD_FLOAT:
            case OpCode::CALL_STEP_FLOAT:
            case OpCode::DISTANCE_VEC2:
            case OpCode::DISTANCE_VEC3:
            case OpCode::DISTANCE_VEC4:
            case OpCode::REFLECT_VEC3:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            case OpCode::CALL_SMOOTHSTEP_FLOAT:
                // thirdParam contains the third argument
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2 << ", thirdParam";
                break;
            case OpCode::REFRACT_VEC3:
                // eta is in thirdParam_ slot
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2 << ", thirdParam";
                break;
            case OpCode::LOAD_THIRD_PARAM:
                oss << " r" << (int)inst.regSrc1;
                break;
            case OpCode::LOAD_FOURTH_PARAM:
                oss << " r" << (int)inst.regSrc1;
                break;
            case OpCode::LOAD_EXTRA_PARAM:
                oss << " [" << (int)inst.regDest << "], r" << (int)inst.regSrc1;
                break;
            // Matrix multiplication (dest, src1, src2)
            case OpCode::MUL_MAT2_MAT2:
            case OpCode::MUL_MAT3_MAT3:
            case OpCode::MUL_MAT4_MAT4:
            case OpCode::MUL_MAT2_VEC2:
            case OpCode::MUL_MAT3_VEC3:
            case OpCode::MUL_MAT4_VEC4:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            // Comparison operations (dest, src1, src2)
            case OpCode::CMP_GT:
            case OpCode::CMP_GE:
            case OpCode::CMP_LT:
            case OpCode::CMP_LE:
            case OpCode::CMP_EQ:
            case OpCode::CMP_NE:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            // Logical operations
            case OpCode::LOGICAL_AND:
            case OpCode::LOGICAL_OR:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            case OpCode::LOGICAL_NOT:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1;
                break;
            // Function calls with 2 operands
            case OpCode::CALL_MIN_FLOAT:
            case OpCode::CALL_MAX_FLOAT:
            case OpCode::CALL_POW:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            // Function calls with 3 operands (use thirdParam)
            case OpCode::CALL_CLAMP_FLOAT:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2 << ", thirdParam";
                break;
            // Mix functions
            case OpCode::CALL_MIX_FLOAT:
            case OpCode::CALL_MIX_VEC2:
            case OpCode::CALL_MIX_VEC3:
            case OpCode::CALL_MIX_VEC4:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2 << ", thirdParam";
                break;
            // Vector length and normalize (unary)
            case OpCode::LENGTH_VEC2:
            case OpCode::LENGTH_VEC3:
            case OpCode::LENGTH_VEC4:
            case OpCode::NORMALIZE_VEC2:
            case OpCode::NORMALIZE_VEC3:
            case OpCode::NORMALIZE_VEC4:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1;
                break;
            // Constructors (dest, src1, src2)
            case OpCode::CONSTRUCT_VEC2:
            case OpCode::CONSTRUCT_VEC3:
            case OpCode::CONSTRUCT_VEC4:
            case OpCode::CONSTRUCT_MAT2:
            case OpCode::CONSTRUCT_MAT3:
            case OpCode::CONSTRUCT_MAT4:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", r" << (int)inst.regSrc2;
                break;
            // Matrix index (dest, src1 for col, extraParam for row)
            case OpCode::MAT2_INDEX:
            case OpCode::MAT3_INDEX:
            case OpCode::MAT4_INDEX:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << ", extraParam";
                break;
            // Vector index (dest, src1 for index, src2 for vector)
            case OpCode::VEC2_INDEX:
            case OpCode::VEC3_INDEX:
            case OpCode::VEC4_INDEX:
                oss << " r" << (int)inst.regDest << ", r" << (int)inst.regSrc1 << "(index), r" << (int)inst.regSrc2 << "(vec)";
                break;
            // Jump instructions
            case OpCode::JUMP: {
                int16_t offset = static_cast<int16_t>((inst.regSrc2 << 8) | inst.regSrc1);
                oss << " offset=" << offset << " (-> " << (int)(i + 1 + offset) << ")";
                break;
            }
            case OpCode::JUMP_IF_FALSE: {
                int16_t offset = static_cast<int16_t>((inst.regSrc2 << 8) | inst.regSrc1);
                oss << " r" << (int)inst.regDest << ", offset=" << offset << " (-> " << (int)(i + 1 + offset) << ")";
                break;
            }
            default:
                break;
        }
        oss << "\n";
    }
    
    return oss.str();
}

} // namespace highPerf
} // namespace mmrsl
