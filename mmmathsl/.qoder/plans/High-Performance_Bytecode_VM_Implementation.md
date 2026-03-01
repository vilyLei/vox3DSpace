# High-Performance Bytecode VM Implementation Plan

## Project Structure
```
mmrsl/
├── simple/                    # Original AST interpreter (保留)
│   ├── include/mmrsl/         # Simple mode public headers
│   │   ├── math_parser.hpp    # Simple mode public API
│   │   ├── interpreter.hpp    # AST interpreter
│   │   ├── lexer.hpp          # Lexer (shared)
│   │   ├── parser.hpp         # Parser (shared)
│   │   └── types.hpp          # Type definitions (shared)
│   ├── src/
│   │   ├── math_parser.cpp
│   │   ├── interpreter.cpp
│   │   ├── lexer.cpp
│   │   ├── parser.cpp
│   │   └── types.cpp
│   └── tests/
│       └── test_main.cpp
├── high_perf/                 # New bytecode VM version
│   ├── include/mmrsl/         # HP mode public header only
│   │   └── hp_math_parser.hpp # HP mode public API
│   ├── src/
│   │   ├── bytecode.cpp       # Bytecode implementation
│   │   ├── compiler.cpp       # AST to bytecode compiler
│   │   ├── vm.cpp             # Virtual machine
│   │   └── hp_math_parser.cpp # HP mode public API impl
│   └── tests/
│       ├── test_hp_main.cpp   # Main tests + performance
│       ├── test_hp_builtin.cpp
│       ├── test_hp_complex.cpp
│       └── test_hp_stress.cpp
├── include/mmrsl/             # Unified public API (PIMPL)
│   ├── mmrsl.hpp              # Main public header
│   ├── types.hpp              # Public types (Vec2/3/4, Mat2/3/4, Value)
│   └── detail/                # Implementation details (internal)
│       ├── bytecode.hpp       # Bytecode definitions
│       ├── compiler.hpp       # Compiler interface
│       ├── vm.hpp             # VM interface
│       ├── interpreter.hpp    # Interpreter interface
│       ├── lexer.hpp          # Lexer interface
│       └── parser.hpp         # Parser interface
├── src/
│   └── mmrsl.cpp              # Main implementation (PIMPL dispatch)
└── CMakeLists.txt             # Build configuration
```

## Phase 1: Bytecode Design

### 1.1 Instruction Set
Design compact, type-specialized instructions:
- LOAD_CONST_* (load from constant pool)
- LOAD_LOCAL / STORE_LOCAL (variable access)
- Binary ops: ADD_FLOAT, ADD_VEC2, MUL_FLOAT, MUL_VEC2_FLOAT, etc.
- CALL_* (direct builtin calls)
- RETURN

### 1.2 Data Structures
- Instruction: 16-bit opcode + 8-bit operand
- Constant pool: vector<Value>
- Register file: array<Value, N> (pre-allocated)

## Phase 2: Virtual Machine

### 2.1 Core VM
- Register-based execution
- Jump table dispatch (switch or function pointer table)
- No dynamic allocation during execution

### 2.2 VM Features
- Stack frame management
- Register allocation (simple greedy algorithm)
- Error handling without exceptions

## Phase 3: Compiler

### 3.1 AST to Bytecode
- Traverse AST and emit type-specialized instructions
- Constant folding (optional)
- Register allocation

### 3.2 Type Inference
- Use AST node types to select correct instruction variant
- Compile-time type checking

## Phase 4: Integration & Testing

### 4.1 Public API
- Similar interface to simple version
- compile() → bytecode
- execute() → run VM

### 4.2 Tests
- Same test cases as simple version
- Performance comparison benchmark
- Verify identical outputs

## Implementation Order

1. Create directory structure
2. Implement bytecode.hpp (instruction definitions)
3. Implement vm.hpp/cpp (core VM)
4. Implement compiler.hpp/cpp (AST → bytecode)
5. Implement hp_math_parser.hpp/cpp (public API)
6. Create tests with performance comparison
7. Update CMakeLists.txt

## Key Design Decisions

- **Register-based VM**: Better performance than stack-based for this use case
- **Type-specialized instructions**: Eliminate runtime type checks
- **Fixed-size register file**: Pre-allocate, no dynamic allocation
- **Direct builtin calls**: No function name lookup at runtime
- **Reuse frontend**: Same Lexer and Parser as simple version
