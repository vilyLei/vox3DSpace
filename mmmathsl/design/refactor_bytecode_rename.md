# Refactor: HighPerfParser → BytecodeParser

## Background

The original "high-performance" engine was named based on its *performance characteristic* (`HighPerfParser`, `high_perf/`, `highPerf` namespace). This was a subjective description and did not accurately communicate *how* the engine works internally.

The engine compiles GLSL-style source code into bytecode and executes it on a register-based virtual machine. "Bytecode" is a precise, implementation-level description that also creates a clear contrast with the other engine (`SimpleParser`), which directly interprets an AST.

---

## Changes

### Directory

| Before | After |
|--------|-------|
| `high_perf/` | `bytecode/` |

### C++ Class Name

| Before | After |
|--------|-------|
| `mmrsl::HighPerfParser` | `mmrsl::BytecodeParser` |

### Internal Namespace

| Before | After |
|--------|-------|
| `namespace highPerf` | `namespace bytecode` |
| `highPerf::VM` | `bytecode::VM` |
| `highPerf::Compiler` | `bytecode::Compiler` |
| `highPerf::BytecodeFunction` | `bytecode::BytecodeFunction` |

### CMake Targets

| Before | After |
|--------|-------|
| `mmrsl_hp_test` | `mmrsl_bc_test` |
| `mmrsl_hp_stress` | `mmrsl_bc_stress` |
| `mmrsl_hp_complex` | `mmrsl_bc_complex` |
| `mmrsl_hp_builtin` | `mmrsl_bc_builtin` |
| `mmrsl_hp_pipeline` | `mmrsl_bc_pipeline` |
| `mmrsl_hp_ternary` | `mmrsl_bc_ternary` |
| `mmrsl_hp_for_loop` | `mmrsl_bc_for_loop` |

### WASM JavaScript API

| Before | After |
|--------|-------|
| `new Module.HighPerfParser()` | `new Module.BytecodeParser()` |

### Files Modified

- `include/mmrsl/mmrsl.hpp` — class declaration
- `src/mmrsl.cpp` — class implementation
- `include/mmrsl/detail/compiler.hpp` — namespace
- `include/mmrsl/detail/vm.hpp` — namespace
- `include/mmrsl/detail/bytecode.hpp` — namespace
- `bytecode/src/compiler.cpp` — namespace
- `bytecode/src/vm.cpp` — namespace
- `bytecode/src/bytecode.cpp` — namespace
- `bytecode/tests/*.cpp` — all 7 test files
- `CMakeLists.txt` — source paths and target names
- `wasm/mmrsl_wasm_bridge.cpp` — wrapper class and Embind registration
- `wasm/test_wasm.mjs` — JS test
- `vkapp/src/scene/render_object.hpp` — member field types

### Files Deleted

- `bytecode/include/mmrsl/hp_math_parser.hpp` — legacy wrapper class (`HPmmrsl`), superseded by `BytecodeParser`
- `bytecode/src/hp_math_parser.cpp` — corresponding implementation

---

## Naming Consistency After Refactor

All three layers now align:

```
Directory:  bytecode/
Namespace:  mmrsl::bytecode
Class:      mmrsl::BytecodeParser
```

Contrast with the simple engine:

```
Directory:  simple/
Namespace:  mmrsl (detail)
Class:      mmrsl::SimpleParser
```

---

## Validation

All tests pass after the rename:

| Suite | Result |
|-------|--------|
| `mmrsl_test` (SimpleParser) | 17/17 |
| `mmrsl_bc_test` | 104/104 |
| `mmrsl_bc_builtin` | 39/39 |
| `mmrsl_bc_pipeline` | 5/5 |
| `mmrsl_bc_ternary` | 12/12 |
| `mmrsl_bc_for_loop` | 16/16 |
| WASM (`test_wasm.mjs`) | All tests PASS |
