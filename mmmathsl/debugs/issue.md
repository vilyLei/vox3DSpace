# mmrsl For-Loop Deep Review Issues

本文件记录对 for 循环相关代码进行第二轮深度审查后发现的问题。
上一轮报告的 9 个问题已全部修复，以下为新发现问题。

---

## Issue 1 — [Critical] `patchJump` 不验证 `patchIdx` 越界

**文件：** `high_perf/src/compiler.cpp`，`patchJump()` 函数

**现象：**
`patchJump` 在向 `currentFunc_->code[patchIdx]` 写入回填偏移时，没有检查 `patchIdx` 是否小于 `code.size()`。

**触发路径：**
`compileCompound` 在 `setError()` 之后仍继续编译后续语句（见 Issue 2），可能产生残留的无效 `patchIdx` 留在 `loopStack_.back().breakPatches` / `continuePatches` 中，随后 `patchJump` 用该下标做 `code[patchIdx]` 写操作，触发越界 UB。虽然最终 `compile()` 末尾的 `hasError()` 会抛出，但越界写已经发生。

**建议修复：**
```cpp
void Compiler::patchJump(size_t patchIdx, size_t targetIdx) {
    if (!currentFunc_ || patchIdx >= currentFunc_->code.size()) {
        setError("patchJump: patchIdx out of bounds");
        return;
    }
    // ... 原有逻辑
}
```

---

## Issue 2 — [Critical] `compileCompound` 错误后不提前退出，继续无效编译

**文件：** `high_perf/src/compiler.cpp`，`compileCompound()` 函数（约第 187 行）

**现象：**
```cpp
void Compiler::compileCompound(const CompoundStmt& stmt) {
    for (const auto& s : stmt.statements) {
        compileStatement(*s);
        // 缺少：if (hasError()) return;
    }
}
```
`setError()` 不中断控制流。一旦某条语句触发错误（类型不匹配、变量未定义等），编译器继续处理后续语句，可能：
- 产生大量虚假的次级错误消息（噪音）
- 向 `code[]` emit 出无效指令，为 Issue 1 提供触发条件

**建议修复：**
```cpp
void Compiler::compileCompound(const CompoundStmt& stmt) {
    for (const auto& s : stmt.statements) {
        compileStatement(*s);
        if (hasError()) return;  // 错误后立即停止
    }
}
```
同理，`compileFor` / `compileIf` 中各 `compileStatement` 调用后也需加同样的检查。

---

## Issue 3 — [Medium] VM JUMP 负向跳转下溢给出误导性错误

**文件：** `high_perf/src/vm.cpp`，`JUMP` / `JUMP_IF_FALSE` case（约第 585、597 行）

**现象：**
```cpp
pc_ = static_cast<size_t>(static_cast<int>(pc_) + offset);
```
若 `pc_ + offset < 0`，`size_t` 会下溢成极大值。下一轮循环 `pc_ >= code.size()` 使执行退出，或者 `instructionCount_` 超限触发保护，但抛出的错误是误导性的 `"Unknown opcode"` 而不是 `"Invalid jump target"`，调试困难。

**建议修复：**
```cpp
int newPc = static_cast<int>(pc_) + offset;
if (newPc < 0 || static_cast<size_t>(newPc) > func.code.size()) {
    setError("Jump target out of bounds: " + std::to_string(newPc));
    return Value();
}
pc_ = static_cast<size_t>(newPc);
```

---

## Issue 4 — [Medium] `loopDepth_` / `nestingDepth_` 在异常时不安全

**文件：** `simple/src/parser.cpp`，`parseForStmt()` 及 `parseCompoundStmt()`

**现象：**
```cpp
loopDepth_++;
StmtPtr body;
body = parseCompoundStmt();  // 若抛出 ParserError，loopDepth_-- 永远不执行
loopDepth_--;
```
当 `parseCompoundStmt()` 内部因 `MAX_NESTING_DEPTH` 超限抛出 `ParserError` 时，`loopDepth_` 永远不被递减，之后的 `break`/`continue` 上下文检查将出现误判。`nestingDepth_` 存在完全相同的问题。

**建议修复（RAII guard）：**
```cpp
// 在 parser.cpp 开头添加辅助类，或在函数内定义 lambda guard
struct DepthGuard {
    size_t& depth;
    ~DepthGuard() { --depth; }
};

loopDepth_++;
DepthGuard guard{loopDepth_};
StmtPtr body = parseCompoundStmt();  // 异常时 guard 析构自动递减
```

---

## Issue 5 — [Medium] `getExpressionType` 对未知变量静默返回 `Float`

**文件：** `high_perf/src/compiler.cpp`，`getExpressionType()` 函数（约第 1326 行）

**现象：**
```cpp
} else if (auto var = dynamic_cast<const VariableExpr*>(&expr)) {
    auto it = localVarTypes_.find(var->name);
    if (it != localVarTypes_.end()) {
        return it->second;
    }
    return TypeKind::Float; // ← 查不到时不报错，静默返回 Float
}
```
当变量类型查找失败时（例如某个初始化顺序 bug 导致 `localVarTypes_` 未及时填充），函数不设错误而直接返回 `Float`。这会导致：
- `compileBinary` 选择了错误的操作码（例如 `ADD_FLOAT` 而非 `ADD_INT`）
- 后续 `getLocal()` 才触发 `setError()`，但类型错误的指令已经 emit

**建议修复：**
对于无法解析类型的变量，应调用 `setError()` 并返回一个安全的占位类型，而不是静默 fallback。或者至少在 DEBUG 模式下加断言。

---

## Issue 6 — [Medium] `MAX_LOOP_ITERATIONS = 32768` 对嵌套循环过于激进

**文件：** `include/mmrsl/detail/interpreter.hpp`，第 24 行

**现象：**
```cpp
constexpr uint32_t MAX_LOOP_ITERATIONS = 32768;
```
该计数器跨所有嵌套循环累加。对于常见的 GLSL 风格双重循环：
- `for(200) { for(200) {} }` = 40,000 次 → **超出限制，运行失败**
- 这与高性能路径的 `MAX_INSTRUCTIONS = 32768` 量级相近，但两者的含义不同

同时，注释没有说明这是"单次函数执行的总迭代上限"还是"单层循环上限"，设计意图不明确。

**建议修复：**
根据项目实际使用场景（GLSL 计算节点），将上限提升至 `1,000,000` 或更高，并补充注释说明设计依据：
```cpp
// Total loop iterations across all loops in a single function execution.
// Guards against infinite loops; set high enough for typical GLSL compute patterns.
constexpr uint32_t MAX_LOOP_ITERATIONS = 1'000'000;
```

---

## Issue 7 — [Low] `emitMove` 缺少 `TypeKind::Int` case

**文件：** `high_perf/src/compiler.cpp`，`emitMove()` 函数（约第 1116 行）

**现象：**
```cpp
void Compiler::emitMove(uint8_t destReg, uint8_t srcReg, TypeKind type) {
    switch (type) {
        case TypeKind::Float:  currentFunc_->emit(OpCode::MOV_FLOAT, ...); break;
        case TypeKind::Bool:   currentFunc_->emit(OpCode::MOV_BOOL,  ...); break;
        // ... vec/mat cases ...
        default:
            currentFunc_->emit(OpCode::MOV_FLOAT, destReg, srcReg, 0);  // ← Int 也走这里
            break;
    }
}
```
`TypeKind::Int` 没有独立 case，落入 `default` 使用 `MOV_FLOAT`。目前 VM 中所有 `MOV_*` 实现均为 `registers_[dest] = registers_[src]`，行为上无差异，但语义错误。一旦未来 VM 对 MOV 做类型检查或统计，将悄悄产生错误。

**建议修复：**
添加 `case TypeKind::Int:` 分支（可暂时也用 `MOV_FLOAT` 实现，但要显式写出，避免 fallthrough 到 default）：
```cpp
case TypeKind::Int:
    currentFunc_->emit(OpCode::MOV_FLOAT, destReg, srcReg, 0);  // Value 类型安全，语义明确
    break;
```
或添加专用 `MOV_INT` opcode。

---

## Issue 8 — [Low] `continue` 的 `update` 执行路径潜在维护风险

**文件：** `simple/src/interpreter.cpp`，`executeFor()`（约第 217 行）

**现象（当前实现正确，属于维护风险）：**
```cpp
if (isContinuing_) {
    isContinuing_ = false;  // 先清除
    runUpdate();            // 再执行 update
    continue;
}
```
当前约束（update 只能是 AssignStmt）下是安全的。但若未来扩展允许 update 中出现更复杂的表达式（例如函数调用），且该调用内部又触发 `continue`，则 `isContinuing_` 已被提前清除，外层循环将无法感知内部 `continue`。

**建议：** 在 `executeFor` 注释中明确说明此假设（update 不会产生控制流副作用），作为未来扩展时的警戒线。

---

## 优先修复顺序（第二轮，已全部修复）

| 优先级 | Issue | 状态 |
|--------|-------|------|
| 1 | Issue 2 — compileCompound 不 early exit | ✅ 已修复 |
| 2 | Issue 1 — patchJump 越界写 | ✅ 已修复（加边界检查） |
| 3 | Issue 5 — getExpressionType 静默 fallback | ✅ 已修复（加 assert） |
| 4 | Issue 3 — JUMP 下溢误导性错误 | ✅ 已修复（加范围检查） |
| 5 | Issue 4 — loopDepth_ 异常不安全 | ✅ 已修复（loopDepth_ RAII） |
| 6 | Issue 7 — emitMove 缺 Int case | ✅ 已修复（加显式 Int case + 注释） |
| 7 | Issue 6 — MAX_LOOP_ITERATIONS 过小 | ✅ 已修复（提升至 1,000,000） |
| 8 | Issue 8 — continue update 维护风险 | ✅ 已修复（加协议注释） |

---

*第三轮问题见 `debugs/issue_round3.md`*

### Issue 9 — [Critical] VM `JUMP_IF_FALSE` 调用 `asBool()` 可抛出 `TypeError`，但 VM 执行循环无 try-catch

**文件：** `high_perf/src/vm.cpp`，`JUMP_IF_FALSE` case（约第 598 行）

**现象：**
```cpp
case OpCode::JUMP_IF_FALSE: {
    bool condition = registers_[inst.regDest].asBool();  // ← 可抛出 TypeError
    ...
}
```
`Value::asBool()` 在值不是 `Bool` 类型时抛出 `TypeError`（定义在 `types.cpp`）。VM 的主执行循环是一个无 try-catch 的 `while + switch`。`TypeError` 会直接穿透 `VM::execute()`，到达 `HPmmrsl::execute()`：

```cpp
Value HPmmrsl::execute(...) {
    Value result = vm_->execute(*bytecodeFunc_, arguments);  // TypeError 从此逃逸
    if (!vm_->getLastError().empty()) { ... }               // 这行根本不会执行
    ...
}
```
`HPmmrsl::execute()` 同样没有捕获 `TypeError`，最终由调用方的 `compileAndExecute()` 也无 catch，导致异常直接传播给用户代码，**绕过了 VM 的错误处理机制（lastError_）**，造成接口行为不一致（有时 `execute()` 返回值 + 检查 `getLastError()`，有时直接抛出 `TypeError`）。

同样受影响的调用点：
- `logicalAnd`、`logicalOr`、`logicalNot`（均调用 `asBool()`）

**建议修复：**
在 `VM::execute()` 的 `while` 循环内捕获 `TypeError` 并将其转换为 `setError()`：
```cpp
try {
    // ... switch(inst.opcode) ...
} catch (const TypeError& e) {
    setError(std::string("Type error: ") + e.what());
    break;
}
```
或者更彻底地，在所有 `asBool()` / `asInt()` / `asFloat()` 调用前加 `isBool()` 等检查，用 `setError()` 代替抛出。

---

### Issue 10 — [Medium] `JUMP_IF_FALSE` 的 for 循环条件寄存器编码使用 `regDest` 字段，命名语义混乱

**文件：** `high_perf/include/mmrsl/detail/bytecode.hpp`（Instruction 格式注释）+ `compiler.cpp` 第 1264 行 + `vm.cpp` 第 600 行

**现象：**
`JUMP_IF_FALSE` 指令将条件寄存器编码在 `regDest` 字段（即第2个字节）：
```cpp
// compiler.cpp
currentFunc_->emit(OpCode::JUMP_IF_FALSE, condReg, 0, 0);  // condReg 放入 regDest

// vm.cpp
bool condition = registers_[inst.regDest].asBool();         // 从 regDest 读取
```
`regDest` 字段名暗示这是**目标**寄存器（写入方），但 `JUMP_IF_FALSE` 实际上是**读**这个字段作为条件源。这与其他所有指令的语义（`regDest` = 写出寄存器）相矛盾，增加了后续维护混乱风险。

**建议：**
在 `bytecode.hpp` 的 `JUMP_IF_FALSE` 注释中明确说明此字段的特殊语义：
```cpp
JUMP_IF_FALSE,  // regDest = condition register (READ, not write); offset in regSrc1/regSrc2
```
或者长期考虑引入 `regSrc0` 字段作为独立条件寄存器语义。

---

### Issue 11 — [Medium] Stress Test 中 `testDeepRecursionSimulation` 错误地嵌套 32 层 if（无 else），每一层独立判断，并非真正的深嵌套

**文件：** `high_perf/tests/test_hp_stress.cpp`，`testDeepRecursionSimulation()`（约第 611 行）

**现象：**
```cpp
for (int i = 0; i < 32; i++) {
    source += "    if (x > " + std::to_string(i * 0.1f) + ") {\n";
}
source += "        return 32.0;\n";
for (int i = 0; i < 32; i++) {
    source += "    }\n";
}
source += "    return 0.0;\n}";
```
生成的代码是 32 层**真正嵌套**的 if（没有 else），如：
```
if(x>0) { if(x>0.1) { if(x>0.2) { ... return 32.0; ... } } }
```
编译器的 `MAX_NESTING_DEPTH = 32`，而这里恰好产生**32层嵌套**。此测试的注释说目的是"测试边界"，但实际上它正好踩在 `MAX_NESTING_DEPTH` 的极限上——若限制是严格 `>` 检查，32 层会**刚好通过**；若改为 `>=`，就会失败。当前代码的检查是 `currentNestingDepth_ > MAX_NESTING_DEPTH`（即允许等于32），所以此测试恰好通过。

这属于**边界值测试的脆弱性**：测试只验证了"恰好等于限制"的情况，但没有"超出1层时应该失败"的对应反向测试。

**建议：** 补充一个 33 层嵌套的反向测试，验证超限时编译失败。

---

### Issue 12 — [Low] `test_hp_stress.cpp` 中 `testDeepRecursionSimulation` 的 `total` 计数（14）与实际测试函数数量（14）一致，但 `runStressTests` 函数没有标准前缀输出

**文件：** `high_perf/tests/test_hp_stress.cpp`，第 823 行

**现象：**
```cpp
int runStressTests() {
    std::cout << "\n=== High-Performance mmrsl Stress Test Suite ===\n\n";
```
根据项目规范，每个测试可执行文件的 `main()` 必须输出标准前缀格式（如 `=== TEST: test_hp_stress ===`），以便自动化区分不同测试套件的失败上下文。当前输出格式与规范不一致。

**建议：**
```cpp
std::cout << "=== TEST: test_hp_stress ===\n\n";
```

---

### Issue 13 — [Low] `test_hp_for_loop.cpp` 和 `test_main.cpp` 的测试结果汇总格式不对称

**文件：** `high_perf/tests/test_hp_for_loop.cpp` 第 408 行，`simple/tests/test_main.cpp` 第 539 行

**现象：**
- `test_hp_for_loop.cpp`：`std::cout << "=== For Loop Test Suite ==="`（非标准前缀格式）
- `test_main.cpp`：`std::cout << "=== SimpleParser Test Suite ==="`（非标准前缀格式）

两者都未遵循项目统一的 `=== TEST: <name> ===` 规范格式，在多套件同时失败时难以快速定位。

---

## 第三轮优先修复顺序

| 优先级 | Issue | 严重度 |
|--------|-------|--------|
| 1 | Issue 9 — VM TypeError 逃逸 asBool/logicalAnd 等 | **Critical** |
| 2 | Issue 10 — JUMP_IF_FALSE regDest 字段命名混乱 | Medium（注释修复） |
| 3 | Issue 11 — stress test 缺少超限反向测试 | Medium |
| 4 | Issue 12 — stress test 非标准前缀 | Low |
| 5 | Issue 13 — for_loop / simple test 非标准前缀 | Low |
