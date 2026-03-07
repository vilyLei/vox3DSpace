# Round 5 Issues

**Scope**: Full inspection of remaining files not deeply reviewed in prior rounds.
Files read: `bytecode.cpp`, `compiler.cpp` (lines 1340–1549), `interpreter.cpp`,
`parser.cpp`, `lexer.cpp`, `compiler.hpp` (simple), `test_hp_builtin.cpp`,
`test_hp_complex.cpp`, `test_hp_main.cpp`, `design/VM_capability_boundary.md`.

---

## Confirmed Fixed (Verification of Round 4 Issues)

- **Issue 16** (`getExpressionType` vector types): ✅ FIXED — lines 1435–1443 now
  delegate `pow/min/max/clamp` to `getExpressionType(*call->arguments[0])`, correctly
  propagating `Vec2/Vec3/Vec4`.
- **Issue 17** (`addConstant()` float `==`): ✅ FIXED — bytecode.cpp lines 17–23 now
  use `std::memcmp` for bit-exact float comparison.
- **Issue 18** (`disassemble()` missing opcodes): ✅ FIXED — all opcodes including
  `ADD_FLOAT_VEC2/3/4`, `SUB_FLOAT_VEC2/3/4`, `SUB_VEC3_FLOAT`, `MOV_*`, `CALL_ATAN2`,
  `CALL_POW_VEC2/3/4`, `CALL_MIN/MAX_VEC2/3/4`, `CALL_CLAMP_VEC2/3/4` are now handled.

---

## Issue 20 — `design/VM_capability_boundary.md` stale after MAX_INSTRUCTIONS fix

**Severity**: Low (documentation only)
**File**: `design/VM_capability_boundary.md`, lines 11 and 19

Both entries still show the old value of `32K` / `32768`:
```
- 执行	MAX_INSTRUCTIONS	32K          ← line 11, stale
- 运行期最大指令执行步数: 32768          ← line 19, stale
```
The actual value in the codebase is now `10,000,000` (fixed in Round 4, Issue 15).

**Fix**: Update both lines to `10,000,000`:
```
- 执行	MAX_INSTRUCTIONS	10,000,000
...
- 运行期最大指令执行步数: 10,000,000
```

---

## Issue 21 — `CompileGuard` RAII only resets `currentFunc_`, leaving stale compiler state

**Severity**: Medium
**File**: `include/mmrsl/detail/compiler.hpp`, lines 117–128

```cpp
class CompileGuard {
    Compiler* compiler_;
public:
    explicit CompileGuard(Compiler* c) : compiler_(c) {}
    ~CompileGuard() {
        if (compiler_) {
            compiler_->currentFunc_ = nullptr;  // Only this field is cleared
        }
    }
    void release() { compiler_ = nullptr; }
};
```

On exception during `compile()`, the guard resets `currentFunc_ = nullptr` but leaves
all other state dirty:

| Field left dirty | Risk |
|---|---|
| `errorMsg_` | Stale error from a prior failed compile |
| `localVars_` | Stale variable slot map causes wrong slot assignments |
| `localVarTypes_` | Stale type map causes wrong `getExpressionType()` results |
| `loopStack_` | Non-empty stack can corrupt break/continue patching |
| `usedRegisters_` | Leaked register bits → phantom "all registers in use" error |
| `nextRegister_` | Misaligned register counter |

**Current exposure**: `HPmmrsl` constructs a fresh `Compiler` per instance
(`compile()` in `hp_math_parser.cpp` does `compiler_ = std::make_unique<Compiler>()`),
so reuse across calls does not currently occur. However:
1. A future refactor that reuses the `Compiler` object (e.g., incremental compilation)
   would silently inherit corrupt state.
2. `setError()` / `getLastError()` can return stale messages if `compile()` is called
   twice on the same `Compiler` object and the second call exits early before
   overwriting `errorMsg_`.

**Fix**: Add a `reset()` method to `Compiler` and call it at the top of `compile()`:
```cpp
void Compiler::reset() {
    currentFunc_    = nullptr;
    errorMsg_.clear();
    localVars_.clear();
    localVarTypes_.clear();
    loopStack_.clear();
    usedRegisters_ = 0;
    nextRegister_  = 0;
}
```
Then in `Compiler::compile()` (compiler.cpp):
```cpp
Value Compiler::compile(const FunctionDecl& func, ...) {
    reset();  // ensure clean state before each compilation
    ...
}
```
The `CompileGuard` can remain for `currentFunc_` cleanup on exception exit, but all
persistent state should be initialised by `reset()` at entry rather than only patched
by the guard at exit.

---

## Issue 22 — `test_hp_builtin.cpp` missing test-prefix line

**Severity**: Low (test harness consistency)
**File**: `high_perf/tests/test_hp_builtin.cpp`, line 1042

```cpp
std::cout << "\n=== GLSL Built-in Functions Test Suite ===\n\n";
```

All other test files use the standardised format `"=== TEST: <name> ===\n\n"`
(established in Round 3, Issues 12–13). This file still uses a non-standard title.

**Fix**:
```cpp
std::cout << "=== TEST: test_hp_builtin ===\n\n";
```

---

## Issue 23 — `test_hp_complex.cpp` missing test-prefix line

**Severity**: Low (test harness consistency)
**File**: `high_perf/tests/test_hp_complex.cpp`, line 423

```cpp
std::cout << "\n=== High-Performance mmrsl Complex Expression Test Suite ===\n\n";
```

Same non-standard prefix issue as Issue 22.

**Fix**:
```cpp
std::cout << "=== TEST: test_hp_complex ===\n\n";
```

---

## Issue 24 — `test_hp_main.cpp` missing test-prefix line

**Severity**: Low (test harness consistency)
**File**: `high_perf/tests/test_hp_main.cpp`, line 5977

```cpp
std::cout << "=== High-Performance mmrsl Test Suite ===\n\n";
```

Same non-standard prefix issue as Issues 22–23.

**Fix**:
```cpp
std::cout << "=== TEST: test_hp_main ===\n\n";
```

---

## Issue 25 — `interpreter.cpp` `evaluateBinary`: logical `&&`/`||` evaluates both operands (no short-circuit)

**Severity**: Medium
**File**: `simple/src/interpreter.cpp`, lines 295–297, 618–628

`evaluateBinary` evaluates *both* `left` and `right` before the switch:
```cpp
Value left  = evaluateExpression(*expr.left);
Value right = evaluateExpression(*expr.right);
// ... then:
case TokenType::And:
    if (left.isBool() && right.isBool()) {
        return Value(left.asBool() && right.asBool());
    }
```

GLSL (and C) specifies that `&&` and `||` are short-circuit operators: the right
operand must **not** be evaluated if the result is already determined from the left.
In the current design:
1. Side-effect functions (e.g., `noise`, `smoothstep`) in the right operand are
   always executed even when the left is `false` for `&&` (or `true` for `||`).
2. Any `RuntimeError` thrown in the right operand (e.g., division by zero inside a
   guarded expression like `x != 0 && 1.0/x > 2.0`) will propagate even though GLSL
   would skip it.

The high-performance VM compiler **does** emit `JUMP_IF_FALSE` for logical `&&` to
short-circuit, creating a **semantic divergence** between the two execution paths.

**Fix**: Handle `And`/`Or` before evaluating `right`:
```cpp
case TokenType::And: {
    Value lv = evaluateExpression(*expr.left);
    if (!lv.isBool()) throw RuntimeError("Invalid operands for &&");
    if (!lv.asBool()) return Value(false);  // short-circuit
    Value rv = evaluateExpression(*expr.right);
    if (!rv.isBool()) throw RuntimeError("Invalid operands for &&");
    return Value(rv.asBool());
}
case TokenType::Or: {
    Value lv = evaluateExpression(*expr.left);
    if (!lv.isBool()) throw RuntimeError("Invalid operands for ||");
    if (lv.asBool()) return Value(true);   // short-circuit
    Value rv = evaluateExpression(*expr.right);
    if (!rv.isBool()) throw RuntimeError("Invalid operands for ||");
    return Value(rv.asBool());
}
```
This requires breaking `&&` and `||` out of the pre-evaluated `left`/`right`
pattern, which is an intentional structural change (the other operators remain
pre-evaluated).

---

## Issue 26 — `interpreter.cpp` `callBuiltin("atan", 2-arg)` argument order mismatch with GLSL

**Severity**: Medium
**File**: `simple/src/interpreter.cpp`, lines 1098–1103

```cpp
if (args.size() == 2) {
    if (args[0].isFloat() && args[1].isFloat()) {
        return Value(std::atan2(args[0].asFloat(), args[1].asFloat()));
    }
```

GLSL `atan(y, x)` maps to `atan2(y, x)` — i.e., `args[0]` is `y` and `args[1]` is
`x`. This matches the standard.

However, the high-performance VM bytecode in `vm.cpp` for `CALL_ATAN2` should be
checked for the same argument order to confirm consistency. The issue itself is that
the 2-argument `atan` form is fully implemented in the simple interpreter but there
is **no corresponding opcode path in the HP compiler** — searching `compiler.cpp`
shows `atan` is dispatched as a 1-argument call only:
```cpp
// In compileCall(), the atan branch:
result = compileBuiltin1("atan", OpCode::CALL_ATAN, args, rd);
```
If a user writes `atan(y, x)` in a high-perf script, the compiler will fail with
"wrong number of arguments" or silently use only the first arg, while the simple
interpreter handles it correctly. This is a **cross-backend capability divergence**.

**Fix**: Add a 2-argument `atan` dispatch in `compiler.cpp` `compileCall()`:
```cpp
} else if (name == "atan") {
    if (args.size() == 1) {
        result = compileBuiltin1("atan", OpCode::CALL_ATAN, args, rd);
    } else if (args.size() == 2) {
        result = compileBuiltin2("atan2", OpCode::CALL_ATAN2, args, rd);
    } else {
        setError("atan() takes 1 or 2 arguments");
    }
```
(Verify `CALL_ATAN2` is already defined in the opcode enum and implemented in
`vm.cpp` — confirmed present in `opcodeToString` at bytecode.cpp line 396.)

---

## Issue 27 — `lexer.cpp` does not lex block comments `/* ... */`

**Severity**: Low
**File**: `simple/src/lexer.cpp`, lines 215–228

`skipWhitespace()` only handles single-line `//` comments:
```cpp
} else if (c == '/') {
    if (pos_ + 1 < source_.size() && source_[pos_ + 1] == '/') {
        // Single line comment
        while (!isAtEnd() && peek() != '\n') advance();
    } else {
        break;  // Not a comment — stop skipping whitespace
    }
}
```

Block comments `/* ... */` are not handled. If a user writes `/* comment */` the
lexer will stop at the first `/`, lex it as `TokenType::Divide`, and produce a parse
error. This is inconsistent with GLSL and most C-family languages.

**Note**: This is a known limitation for a minimal implementation, but since the
design targets GLSL-style scripts it warrants a note. If block comments are out of
scope, it should be documented in `base_description.md`.

**Fix** (if block comments are desired):
```cpp
} else if (c == '/') {
    if (pos_ + 1 < source_.size() && source_[pos_ + 1] == '/') {
        // Single-line comment
        while (!isAtEnd() && peek() != '\n') advance();
    } else if (pos_ + 1 < source_.size() && source_[pos_ + 1] == '*') {
        // Block comment
        advance(); advance();  // consume '/' and '*'
        while (!isAtEnd()) {
            if (peek() == '*' && pos_ + 1 < source_.size() && source_[pos_ + 1] == '/') {
                advance(); advance();  // consume '*' and '/'
                break;
            }
            advance();
        }
    } else {
        break;
    }
}
```

---

## Issue 28 — `test_hp_builtin.cpp` has no test for `refract()`

**Severity**: Low (test coverage gap)
**File**: `high_perf/tests/test_hp_builtin.cpp`

`testReflect()` is present (line 1010) and called (line 1107). `refract()` has no
corresponding test. The `refract` opcode, VM handler, and compiler path all exist and
were reviewed to be correct, but there is zero automated coverage for this path in the
builtin test suite.

**Fix**: Add a `testRefract()` function and include it in the runner:
```cpp
bool testRefract() {
    std::cout << "Builtin: refract()... ";
    HighPerfParser parser;
    std::string source = R"(
        vec3 calc(vec3 i, vec3 n, float eta) {
            return refract(i, n, eta);
        }
    )";
    try {
        // eta=1.0 → no bending; refract(I, N, 1.0) ≈ I when N·I not extreme
        std::vector<mmrsl::Value> args = {
            mmrsl::Value(mmrsl::Vec3(0.0f, -1.0f, 0.0f)),  // I pointing down
            mmrsl::Value(mmrsl::Vec3(0.0f,  1.0f, 0.0f)),  // N pointing up
            mmrsl::Value(1.0f)                              // eta = 1.0
        };
        mmrsl::Value result = parser.compileAndExecute(source, args);
        mmrsl::Vec3 r = result.asVec3();
        // refract(I=(0,-1,0), N=(0,1,0), eta=1): k=1-(1*(1-(-1)^2))= 1-0=1; result = I - 0 = (0,-1,0)
        if (std::abs(r.x) < 0.001f && std::abs(r.y + 1.0f) < 0.001f && std::abs(r.z) < 0.001f) {
            std::cout << "PASS\n";
            return true;
        }
        std::cout << "FAIL (got " << r.x << "," << r.y << "," << r.z << ")\n";
        return false;
    } catch (const std::exception& e) {
        std::cout << "FAIL: " << e.what() << "\n";
        return false;
    }
}
```
Also update `total` from `38` to `39`.

---

## Summary

| # | Severity | File | Description |
|---|---|---|---|
| 20 | Low | `VM_capability_boundary.md` | Stale `MAX_INSTRUCTIONS: 32K` (should be 10M) |
| 21 | Medium | `compiler.hpp` | `CompileGuard` only resets `currentFunc_`; other state left dirty |
| 22 | Low | `test_hp_builtin.cpp` | Non-standard test prefix |
| 23 | Low | `test_hp_complex.cpp` | Non-standard test prefix |
| 24 | Low | `test_hp_main.cpp` | Non-standard test prefix |
| 25 | **Medium** | `interpreter.cpp` | `&&`/`\|\|` not short-circuited; diverges from HP VM |
| 26 | Medium | `compiler.cpp` / `interpreter.cpp` | 2-arg `atan(y,x)` missing in HP compiler |
| 27 | Low | `lexer.cpp` | Block comments `/* */` not lexed |
| 28 | Low | `test_hp_builtin.cpp` | No test for `refract()` |
