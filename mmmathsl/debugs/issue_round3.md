# mmrsl 第三轮代码深度检查 Issues

第二轮发现的 8 个问题已全部修复。本文件记录第三轮新发现的问题。

---

## Issue 9 — [Critical] VM `asBool()` 抛出 `TypeError` 绕过错误处理机制

**文件：** `high_perf/src/vm.cpp`，`JUMP_IF_FALSE` case（约第 598 行）；
`logicalAnd`、`logicalOr`、`logicalNot`（约第 1823–1837 行）

**现象：**
```cpp
case OpCode::JUMP_IF_FALSE: {
    bool condition = registers_[inst.regDest].asBool();  // ← 可抛出 TypeError
    ...
}

void VM::logicalAnd(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    bool a = registers_[rs1].asBool();  // ← 同上
    bool b = registers_[rs2].asBool();
    ...
}
```
`Value::asBool()` 在值不是 `Bool` 类型时抛出 `TypeError`（`types.cpp` 第 56 行）。
VM 主执行循环是无 try-catch 的 `while + switch`，`TypeError` 会穿透 `VM::execute()`，到达 `HPmmrsl::execute()`：

```cpp
Value HPmmrsl::execute(...) {
    Value result = vm_->execute(*bytecodeFunc_, arguments);  // TypeError 从此逃逸
    if (!vm_->getLastError().empty()) { ... }               // 这行根本不会执行到
}
```

**影响：**
- `TypeError` **绕过** `lastError_` 错误处理机制直接传递给用户代码
- 接口行为不一致：正常错误走 `getLastError()`，类型错误则直接抛异常
- 受影响调用点：`JUMP_IF_FALSE`、`LOGICAL_AND`、`LOGICAL_OR`、`LOGICAL_NOT`

**建议修复：**
在 `VM::execute()` 的 switch 块外层包裹 try-catch，将 `TypeError` 转为 `setError()`：
```cpp
while (pc_ < func.code.size()) {
    const Instruction& inst = func.code[pc_++];
    // ...DoS check...
    try {
        switch (inst.opcode) {
            // ...
        }
    } catch (const TypeError& e) {
        setError(std::string("Type error: ") + e.what());
    }
    if (!lastError_.empty()) {
        throw VMError(lastError_);
    }
}
```
或者更彻底地，在所有 `asBool()` 调用前加 `isBool()` 检查，用 `setError()` 代替抛出。

---

## Issue 10 — [Medium] `JUMP_IF_FALSE` 用 `regDest` 字段存储**源**寄存器，命名语义矛盾

**文件：** `include/mmrsl/detail/bytecode.hpp`（注释）；`high_perf/src/compiler.cpp` 约第 1264 行；`high_perf/src/vm.cpp` 约第 600 行

**现象：**
按照所有其他指令的约定，`regDest`（指令的第2字节）是**写出目标**寄存器。但 `JUMP_IF_FALSE` 将条件寄存器（读操作）放在此字段：

```cpp
// compiler.cpp — condReg 作为"源"被写入 regDest 位置
currentFunc_->emit(OpCode::JUMP_IF_FALSE, condReg, 0, 0);

// vm.cpp — 从 regDest 读取，实为源操作
bool condition = registers_[inst.regDest].asBool();
```

这与其他所有指令的语义矛盾，后续维护时容易误解（例如误以为 `JUMP_IF_FALSE` 会写 `regDest`）。

**建议修复（注释修复即可）：**
在 `bytecode.hpp` 的 `JUMP_IF_FALSE` 条目补充说明：
```cpp
JUMP_IF_FALSE,  // regDest = condition source register (READ, not write);
                // jump offset (signed int16) packed in regSrc2<<8 | regSrc1
```
长期可考虑重新设计为 `emit(JUMP_IF_FALSE, 0, condReg, 0)`，将条件寄存器放入 `regSrc1`，与语义一致。

---

## Issue 11 — [Medium] Stress Test 边界测试不完整：只验证"恰好等于限制"，缺反向失败测试

**文件：** `high_perf/tests/test_hp_stress.cpp`，`testDeepRecursionSimulation()`（约第 611 行）

**现象：**
该测试构造精确 32 层嵌套 if（`MAX_NESTING_DEPTH = 32`），恰好等于上限：
```cpp
for (int i = 0; i < 32; i++) {
    source += "    if (x > " + std::to_string(i * 0.1f) + ") {\n";
}
```
当前检查为 `currentNestingDepth_ > MAX_NESTING_DEPTH`（允许等于 32），所以测试通过。

**问题：**
- 缺少"33 层应该拒绝编译"的**反向测试**
- 若将来把检查改为 `>=`，此测试会静默通过（编译失败但测试期望失败），边界正确性无法得到保证

**建议修复：**
在 `testDeepRecursionSimulation` 之后添加一个反向测试：
```cpp
bool testNestingDepthExceeded() {
    // 33 层嵌套应该被编译器拒绝
    std::string source = "float calc(float x) {\n";
    for (int i = 0; i < 33; i++) {
        source += "    if (x > 0.0) {\n";
    }
    source += "        return 1.0;\n";
    for (int i = 0; i < 33; i++) source += "    }\n";
    source += "    return 0.0;\n}";
    HighPerfParser parser;
    bool compiled = parser.compile(source);
    // 期望编译失败
    return !compiled;
}
```

---

## Issue 12 — [Low] `test_hp_stress.cpp` 输出前缀不符合项目规范

**文件：** `high_perf/tests/test_hp_stress.cpp`，第 823 行

**现象：**
```cpp
int runStressTests() {
    std::cout << "\n=== High-Performance mmrsl Stress Test Suite ===\n\n";
```

项目规范要求每个测试可执行文件的 `main()` 输出标准前缀（`=== TEST: <name> ===`），以便在多套件同时失败时自动化区分来源。当前格式与规范不一致。

**建议修复：**
```cpp
std::cout << "=== TEST: test_hp_stress ===\n\n";
```

---

## Issue 13 — [Low] `test_hp_for_loop.cpp` 和 `test_main.cpp` 输出前缀非标准

**文件：** `high_perf/tests/test_hp_for_loop.cpp` 第 408 行；`simple/tests/test_main.cpp` 第 539 行

**现象：**
```cpp
// test_hp_for_loop.cpp
std::cout << "=== For Loop Test Suite ===" << std::endl;

// test_main.cpp
std::cout << "=== SimpleParser Test Suite ===\n\n";
```
两者均未遵循 `=== TEST: <name> ===` 规范格式，在多测试套件场景中难以快速定位失败来源。

**建议修复：**
```cpp
// test_hp_for_loop.cpp
std::cout << "=== TEST: test_hp_for_loop ===\n\n";

// test_main.cpp
std::cout << "=== TEST: test_simple_parser ===\n\n";
```

---

## 优先修复顺序

| 优先级 | Issue | 严重度 |
|--------|-------|--------|
| 1 | Issue 9 — VM `TypeError` 逃逸，绕过 `lastError_` 机制 | **Critical** |
| 2 | Issue 10 — `JUMP_IF_FALSE` `regDest` 字段命名语义矛盾 | Medium（注释修复） |
| 3 | Issue 11 — stress test 缺少超限反向测试 | Medium |
| 4 | Issue 12 — stress test 非标准输出前缀 | Low |
| 5 | Issue 13 — for_loop / simple test 非标准输出前缀 | Low |
