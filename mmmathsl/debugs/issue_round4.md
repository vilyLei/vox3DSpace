# mmrsl 第四轮代码深度检查 Issues

第三轮发现的 5 个问题已全部修复。本文件记录第四轮新发现的问题。

---

## Issue 14 — [Critical] `BytecodeFunction::emit()` 抛出 `CompilerError`，但 `Compiler::compile()` 调用链无 try-catch

**文件：** `high_perf/src/bytecode.cpp`，第 56–80 行；`high_perf/src/compiler.cpp`，第 13–87 行

**现象：**
每个 `emit()` 重载在超过 `MAX_BYTECODE_INSTRUCTIONS` 时**抛出** `CompilerError`：

```cpp
void BytecodeFunction::emit(OpCode op, uint8_t rd, uint8_t rs1, uint8_t rs2) {
    if (code.size() >= MAX_BYTECODE_INSTRUCTIONS) {
        throw CompilerError("Bytecode instruction limit (...) exceeded");
    }
    code.emplace_back(op, rd, rs1, rs2);
}
```

`Compiler::compile()` 内部使用 `CompileGuard`（RAII），但没有捕获这个 `CompilerError`：

```cpp
BytecodeFunction Compiler::compile(const Program& program) {
    Compiler::CompileGuard guard(this);  // cleans up fields on exception
    // ...
    compileCompound(*program.function->body);  // calls emit() -> can throw CompilerError
    // ...
    if (hasError()) {
        throw std::runtime_error(getError());  // 正常路径：setError 后在这里抛
    }
    guard.release();
    return func;
}
```

**影响：**
- `CompilerError` 穿透 `compile()`，绕过 `hasError()` 检查和 `setError()` 机制
- `HPmmrsl::compile()` 调用 `compiler_->compile(program)`，同样没有捕获 `CompilerError`，异常直接传达用户代码
- `CompileGuard` 析构函数会运行（成员清理），但 `errorMsg_` 不会被赋值，`getLastError()` 返回空串
- 超大脚本（生成超过 `MAX_BYTECODE_INSTRUCTIONS` 条指令时）会导致接口契约破坏

**建议修复：**
在 `emit()` 中改用 `setError()` 而非抛出，或在 `compile()` 中捕获 `CompilerError`：
```cpp
BytecodeFunction Compiler::compile(const Program& program) {
    Compiler::CompileGuard guard(this);
    // ...
    try {
        compileCompound(*program.function->body);
        // ...trailing return / HALT emits...
    } catch (const CompilerError& e) {
        setError(e.what());
    }
    if (hasError()) {
        throw std::runtime_error(getError());
    }
    guard.release();
    return func;
}
```

---

## Issue 15 — [Medium] `MAX_INSTRUCTIONS = 32768` 对于含循环的脚本明显过低

**文件：** `include/mmrsl/detail/vm.hpp`，第 11 行

**现象：**
```cpp
constexpr uint64_t MAX_INSTRUCTIONS = 32768;  // Max instructions per execution (32K)
```

相比之下，`simple` 解释器的 `MAX_LOOP_ITERATIONS` 已在上一轮从 32768 提升到 1,000,000。

对于 VM：一个空循环体的 `for` 需要大约 4–5 条指令（条件检查 + 跳转 + 更新 + 回跳）。运行 1000 次简单迭代就消耗 5000 条指令，运行 6000 次以上就超限了。

**具体问题：**
- 两个执行路径（VM vs Interpreter）的 DoS 限制差异极大（32768 vs 1,000,000），行为不一致
- 用户的合法循环在 HP 路径中会提前截断，产生误导性的运行错误
- 无任何 stress test 覆盖这一差异

**建议修复：**
把 `MAX_INSTRUCTIONS` 提高到与迭代计数匹配的值（考虑每次迭代约 10 条指令）：
```cpp
constexpr uint64_t MAX_INSTRUCTIONS = 10'000'000;  // ~1M iterations × 10 instr/iter
```
同时建议在注释中说明与 `MAX_LOOP_ITERATIONS` 的配合关系。

---

## Issue 16 — [Medium] `getExpressionType()` 对 `CallExpr` 中的 `min`/`max`/`clamp` 无条件返回 `Float`，忽略向量重载

**文件：** `high_perf/src/compiler.cpp`，第 1408–1420 行

**现象：**
```cpp
if (call->function == "floor" || ... ||
    call->function == "min" || call->function == "max" || call->function == "clamp" ||
    ...) {
    return TypeKind::Float;   // ← 对于 min(vec3, vec3) 也返回 Float！
}
```

`min(vec2,vec2)`、`max(vec3,vec3)`、`clamp(vec4,...)`、`pow(vec2, float)` 等向量重载的实际返回类型是向量，但 `getExpressionType()` 返回 `Float`。

**影响：**
若上层代码使用 `getExpressionType()` 的结果来 `emitMove` 或决定强制转换，就会发出错误的 MOV 指令（`MOV_FLOAT` 替代 `MOV_VEC2`），导致寄存器内容类型与指令期望的类型不一致，在 VM 执行时引发 `TypeError`（已被 Issue 9 的修复拦截，但根源在这里）。

具体受影响函数：
- `min(vecN, vecN)` → 实际返回 vecN
- `max(vecN, vecN)` → 实际返回 vecN
- `clamp(vecN, ...)` → 实际返回 vecN
- `pow(vecN, float)` → 实际返回 vecN（已有 CALL_POW_VEC2/VEC3/VEC4 opcode）

**建议修复：**
```cpp
} else if (call->function == "min" || call->function == "max") {
    if (!call->arguments.empty()) {
        return getExpressionType(*call->arguments[0]);
    }
    return TypeKind::Float;
} else if (call->function == "clamp") {
    if (!call->arguments.empty()) {
        return getExpressionType(*call->arguments[0]);
    }
    return TypeKind::Float;
} else if (call->function == "pow") {
    if (!call->arguments.empty()) {
        return getExpressionType(*call->arguments[0]);  // vec2/vec3/vec4 or float
    }
    return TypeKind::Float;
}
```

---

## Issue 17 — [Medium] `addConstant()` 对浮点数使用 `==` 精确比较，会错误地去重 `NaN` 和不同符号的 `0.0f`

**文件：** `high_perf/src/bytecode.cpp`，第 9–46 行

**现象：**
```cpp
case TypeKind::Float:
    match = (constants[i].asFloat() == value.asFloat());
    break;
```

问题一：`NaN != NaN`（IEEE 754），所以 `NaN` 永远不会命中缓存，每次都会新增常量池条目，可能造成池膨胀（虽然 `MAX_CONSTANT_POOL_SIZE` 会最终拦截，但行为诡异）。

问题二：`+0.0f == -0.0f`（IEEE 754），导致 `+0.0f` 和 `-0.0f` 被错误合并为同一常量池条目。若某段脚本明确使用 `-0.0f` 作为负数标志（虽然 GLSL 风格中少见），会产生静默错误。

**建议修复：**
使用 `std::memcmp` 进行 bit-exact 比较：
```cpp
case TypeKind::Float: {
    float a = constants[i].asFloat();
    float b = value.asFloat();
    match = (std::memcmp(&a, &b, sizeof(float)) == 0);
    break;
}
```
这样 `NaN` 不会误缓存（不同 NaN payload 不合并），`+0.0f` 与 `-0.0f` 也不合并。

---

## Issue 18 — [Low] `disassemble()` 中部分新增 opcode 未出现在 switch 分支，输出格式为 `OPCODE_NAME\n`（无操作数信息）

**文件：** `high_perf/src/bytecode.cpp`，第 231–436 行

**现象：**
以下 opcode 在 `opcodeToString()` 中有正确的字符串名，但在 `disassemble()` 的 switch 中没有专门的格式化分支，会走 `default:` 只输出名称，不显示寄存器操作数：

- `ADD_FLOAT_VEC2`, `SUB_FLOAT_VEC2`（scalar±vec2）
- `ADD_FLOAT_VEC3`, `SUB_FLOAT_VEC3`, `SUB_VEC3_FLOAT`（scalar±vec3 / vec3-scalar）
- `ADD_FLOAT_VEC4`, `SUB_FLOAT_VEC4`（scalar±vec4）
- `CALL_ATAN2`（2-arg atan）
- `CALL_POW_VEC2`, `CALL_POW_VEC3`, `CALL_POW_VEC4`
- `CALL_MIN_VEC2`~`VEC4`, `CALL_MAX_VEC2`~`VEC4`
- `CALL_CLAMP_VEC2`~`VEC4`
- `MOV_FLOAT`~`MOV_MAT4`（8 个 MOV 指令）
- `INT_TO_FLOAT`, `FLOAT_TO_INT`（类型转换）
- `ADD_INT`, `SUB_INT`, `MUL_INT`, `DIV_INT`, `MOD_INT`

这不会导致运行时错误，但调试时 `disassemble()` 输出缺少操作数信息，难以跟踪问题。

**建议修复：**
将这些 opcode 分组加入对应的 disassemble 分支（二元、一元等）。

---

## Issue 19 — [Low] `compileCall()` 中 `argRegs` 在 `setError()` 后仍调用 `freeRegister()`，但缺少 `hasError()` 提前退出

**文件：** `high_perf/src/compiler.cpp`，第 545–770 行

**现象：**
```cpp
uint8_t Compiler::compileCall(const CallExpr& expr) {
    std::vector<uint8_t> argRegs;
    for (const auto& arg : expr.arguments) {
        argRegs.push_back(compileExpression(*arg));  // ← 若此处 hasError()，后续继续执行
    }
    uint8_t resultReg = allocateRegister();
    // ...many if-else branches, some call setError()...
    for (uint8_t reg : argRegs) {
        freeRegister(reg);
    }
    return resultReg;
}
```

问题：
1. 编译参数时如果某个 `compileExpression` 设了错误，后续参数编译仍继续执行（与 `compileCompound` 有早退策略不同）
2. 错误路径上也执行了 `freeRegister(resultReg)` 的对称调用缺失（`resultReg` 已分配但不会被 free，若 caller 继续编译会出现寄存器泄漏）

注：在现有架构下，错误后编译结果不会被执行，寄存器泄漏仅导致编译时资源浪费，不会崩溃。但与整体的 early-exit 策略不一致。

**建议修复：**
在参数编译循环后添加提前退出：
```cpp
for (const auto& arg : expr.arguments) {
    argRegs.push_back(compileExpression(*arg));
    if (hasError()) {
        for (uint8_t reg : argRegs) freeRegister(reg);
        freeRegister(resultReg);
        return resultReg;
    }
}
```

---

## 优先修复顺序

| 优先级 | Issue | 严重度 |
|--------|-------|--------|
| 1 | Issue 14 — `emit()` 抛出 `CompilerError` 绕过错误处理机制 | **Critical** |
| 2 | Issue 15 — `MAX_INSTRUCTIONS = 32768` 与 `MAX_LOOP_ITERATIONS = 1,000,000` 严重不匹配 | Medium |
| 3 | Issue 16 — `getExpressionType()` 对向量重载的 `min/max/clamp/pow` 返回 `Float` | Medium |
| 4 | Issue 17 — `addConstant()` 对 `float` 用 `==` 比较，错误处理 `NaN` 和 `-0.0f` | Medium |
| 5 | Issue 18 — `disassemble()` 缺少部分 opcode 的操作数格式化 | Low |
| 6 | Issue 19 — `compileCall()` 参数编译无 early-exit，错误路径 `resultReg` 未释放 | Low |
