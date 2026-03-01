# GLSL 数学脚本引擎 — 功能演进路线图

> 聚焦纯数学/逻辑运算引擎，不做通用脚本语言。  
> 保证高安全性、功能一致性和性能可控性。

---

## 当前状态

- **已实现**：标量 / 向量 / 矩阵的算术运算、比较运算、逻辑运算
- **已实现**：if-else 条件分支
- **已实现**：内置函数：`floor`, `sin`, `cos`, `tan`, `abs`, `sqrt`, `pow`, `dot`, `cross`, `length`, `normalize`, `reflect`, `refract`, `min`, `max`, `clamp`, `mix`
- **已实现**：向量成员访问（`.x`, `.y`, `.z`, `.w`）、矩阵索引（`m[col][row]`）
- **已实现**：向量 / 矩阵构造器（`vec2(x,y)`, `mat4(16 floats)` 等）
- **已实现**：字节码编译 + 寄存器式 VM 执行
- **测试覆盖**：95 个测试用例全部通过

---

## Phase 1 — 核心补全（最高 ROI）

> 目标：覆盖绝大多数实际着色器算法的需求。

### 1.1 `for` 循环

大量数学算法需要迭代（牛顿法、级数展开、光线步进等）。

```glsl
float calc(float x) {
    float guess = x * 0.5;
    for (int i = 0; i < 10; i = i + 1) {
        guess = (guess + x / guess) * 0.5;
    }
    return guess;
}
```

- **实现要点**：编译为 `JUMP` 回跳，循环条件用 `JUMP_IF_FALSE` 跳出
- **安全保障**：配合 DoS 防护的指令计数上限，防止死循环
- **优先级**：⭐⭐⭐⭐⭐

### 1.2 三元运算符 `? :`

数学表达式中极常用，比 if-else 语句简洁得多。

```glsl
float calc(float x) {
    return x > 0.0 ? x : -x;
}
```

- **实现要点**：与 if-else 编译方式相同（`JUMP_IF_FALSE` + 回填），但作为表达式有返回值
- **优先级**：⭐⭐⭐⭐⭐

### 1.3 高频内置函数补全

以下是 GLSL 中高频使用但尚未实现的函数：

| 函数 | 说明 | 典型应用 |
|------|------|---------|
| `step(edge, x)` | `x < edge ? 0.0 : 1.0` | 硬阈值判断 |
| `smoothstep(e0, e1, x)` | Hermite 插值 | 几乎所有着色器都用 |
| `fract(x)` | 取小数部分 `x - floor(x)` | 纹理坐标、噪声 |
| `sign(x)` | 返回 `-1.0 / 0.0 / 1.0` | 方向判断 |
| `exp(x)` / `log(x)` | 自然指数 / 自然对数 | 衰减、色彩空间 |

- **实现要点**：每个函数新增一个 OpCode + VM 实现 + 编译器 compileCall 分支
- **优先级**：⭐⭐⭐⭐⭐

### 1.4 单参数构造器

GLSL 允许单值填充整个向量或构造单位矩阵：

```glsl
vec3 white = vec3(1.0);          // → vec3(1.0, 1.0, 1.0)
mat4 identity = mat4(1.0);       // → 单位矩阵（对角线为1）
vec4 zero = vec4(0.0);           // → 全零向量
```

- **实现要点**：在 `compileConstructor` 中检测单参数情况，复制填充
- **优先级**：⭐⭐⭐⭐

---

## Phase 2 — 性能优化

> 目标：提升 VM 执行效率，尤其是批量计算场景。

### 2.1 Value 访问器返回 `const&`

当前 `asMat4()` 等访问器返回值拷贝（mat4 = 64 字节），改为引用可避免大类型拷贝。

```cpp
// 当前（每次拷贝 64 字节）
glm::mat4 asMat4() const;

// 优化后（零拷贝）
const glm::mat4& asMat4() const {
    if (!isMat4()) throw TypeError(...);
    return std::get<glm::mat4>(data_);
}
```

- **收益**：矩阵运算密集场景性能提升明显
- **难度**：低
- **优先级**：⭐⭐⭐⭐⭐

### 2.2 常量折叠

编译期直接计算已知结果，减少运行时指令数。

```glsl
// 编译前
return x * 2.0 * 3.14159;

// 常量折叠后：2.0 * 3.14159 = 6.28318 在编译期算好
return x * 6.28318;
```

- **实现思路**：在 AST 层或字节码层，识别双操作数均为常量的指令，直接替换为结果常量
- **优先级**：⭐⭐⭐⭐

### 2.3 MOV 指令替代 STORE + LOAD

当前 `compileReturn` 中返回值不在 r0 时，需要 `STORE_LOCAL` + `LOAD_LOCAL` 两条指令做中转。新增 `MOV` 指令可实现寄存器间直接拷贝，省一次内存往返。

```
// 当前
STORE_LOCAL r3, [temp]
LOAD_LOCAL  r0, [temp]

// 优化后
MOV r0, r3
```

- **优先级**：⭐⭐⭐⭐

### 2.4 批量执行模式

同一函数对大量输入逐一执行是最常见的使用模式（如逐像素处理）。提供批量接口，避免每次调用的 `reset()` 全量开销。

```cpp
// 当前：每次调用都 reset 全部 128 个寄存器 + 128 个局部变量
for (auto& pixel : image) {
    result = vm.execute(func, {Value(pixel)});
}

// 优化：批量模式，只重置必要状态
std::vector<Value> results = vm.executeBatch(func, inputs);
```

- **优先级**：⭐⭐⭐⭐

### 2.5 Computed Goto / Threaded Dispatch

将 VM 的 `switch-case` 替换为 GCC/Clang 的 computed goto，消除分支预测开销。

```cpp
// 当前
switch (inst.opcode) {
    case OpCode::ADD_FLOAT: ...
    case OpCode::MUL_FLOAT: ...
}

// 优化后（GCC/Clang 扩展，需条件编译）
static void* dispatch[] = { &&op_add_float, &&op_mul_float, ... };
goto *dispatch[inst.opcode];
op_add_float:
    addFloat(rd, rs1, rs2);
    goto *dispatch[next_opcode];
```

- **收益**：通常带来 20-50% 的 VM 执行性能提升
- **注意**：MSVC 不支持此特性，需要 `#ifdef __GNUC__` 条件编译
- **优先级**：⭐⭐⭐

---

## Phase 3 — 表达力增强

> 目标：补全 GLSL 核心语法特性，提升脚本编写体验。

### 3.1 Swizzle 操作

GLSL 的标志性特性，写向量操作非常自然：

```glsl
vec3 calc(vec4 color) {
    vec3 rgb = color.xyz;        // 提取前三分量
    vec3 bgr = color.zyx;        // 分量重排
    vec2 rg = color.xy;          // 降维
    return rgb;
}
```

- **实现思路**：编译器识别多字符成员访问（`.xyz`），拆解为多次 `MEMBER_X/Y/Z` + `CONSTRUCT_VECn`，或新增专用 `SWIZZLE` 指令
- **优先级**：⭐⭐⭐⭐

### 3.2 混合构造器

GLSL 允许从低维向量构造高维向量：

```glsl
vec3 rgb = vec3(0.5, 0.3, 0.1);
vec4 rgba = vec4(rgb, 1.0);      // vec3 + float → vec4
vec4 v = vec4(rg, 0.0, 1.0);    // vec2 + float + float → vec4
```

- **实现要点**：在 `compileConstructor` 中根据参数类型组合（非全 float）做分量展开
- **优先级**：⭐⭐⭐⭐

### 3.3 更多内置函数

| 函数 | 说明 | 典型应用 |
|------|------|---------|
| `ceil(x)` / `round(x)` | 向上取整 / 四舍五入 | 离散化 |
| `asin(x)` / `acos(x)` / `atan(y, x)` | 反三角函数 | 角度计算 |
| `exp2(x)` / `log2(x)` | 2 为底的指数 / 对数 | HDR、mipmap |
| `radians(x)` / `degrees(x)` | 角度弧度转换 | 旋转计算 |
| `distance(a, b)` | 两点距离 | 碰撞检测、SDF |
| `faceforward(N, I, Nref)` | 法线朝向修正 | 光照 |

- **优先级**：⭐⭐⭐

### 3.4 矩阵运算函数

| 函数 | 说明 | 典型应用 |
|------|------|---------|
| `inverse(mat)` | 矩阵求逆 | 坐标变换 |
| `transpose(mat)` | 矩阵转置 | 法线变换 |
| `determinant(mat)` | 行列式 | 可逆性判断 |

- **优先级**：⭐⭐⭐

### 3.5 向量版 min / max / clamp

当前 `min`、`max`、`clamp` 仅支持 float，GLSL 也支持分量级向量操作：

```glsl
vec3 clamped = clamp(color, vec3(0.0), vec3(1.0));
vec3 result = max(a, b);  // 分量级取最大
```

- **优先级**：⭐⭐⭐

---

## Phase 4 — 进阶能力

> 目标：面向更复杂的应用场景，提升系统的工程化水平。

### 4.1 多函数定义与调用

允许定义辅助函数并互相调用，减少代码重复：

```glsl
float saturate(float x) {
    return clamp(x, 0.0, 1.0);
}

vec3 calc(vec3 color) {
    return vec3(saturate(color.x), saturate(color.y), saturate(color.z));
}
```

- **实现需要**：调用栈、`CALL` / `RET` 指令、函数表
- **复杂度**：较高
- **优先级**：⭐⭐⭐

### 4.2 编译错误行号定位

当前编译错误只有文字描述，缺乏位置信息：

```
// 当前
Compiler error: Invalid operand types for '+' operator

// 改进后
Compiler error at line 3, col 16: Invalid operand types for '+' operator
  3 |    return v + 1.0;
     |              ^^^
```

- **实现要点**：AST 节点记录 `SourceLocation`，编译器报错时附带位置
- **优先级**：⭐⭐⭐

### 4.3 字节码序列化 / 反序列化

将编译结果持久化到文件，下次直接加载执行，跳过编译步骤：

```cpp
// 保存
bytecodeFunc->serialize("shader.bin");

// 加载（跳过 lexer + parser + compiler）
auto func = BytecodeFunction::deserialize("shader.bin");
vm.execute(func, args);
```

- **适用场景**：脚本不经常变化但频繁执行
- **优先级**：⭐⭐⭐

### 4.4 外部常量注入

允许宿主程序注入命名常量，脚本中直接引用：

```cpp
// C++ 侧
parser.setConstant("PI", Value(3.14159265f));
parser.setConstant("SCREEN_WIDTH", Value(1920.0f));
```

```glsl
float calc(float angle) {
    return sin(angle * PI / 180.0);
}
```

- **实现要点**：编译器在 `compileVariable` 中查找常量表，命中则 `LOAD_CONST` 而非 `LOAD_LOCAL`
- **优先级**：⭐⭐⭐

### 4.5 类型检查独立化

将类型检查从编译逻辑中抽离为独立的 Type Check Pass：

- 编译前统一校验类型匹配
- 错误信息更精确（"第3行：vec3 不能和 float 相加"）
- 编译逻辑更干净（只关心代码生成，不混类型检查）

- **优先级**：⭐⭐

---

## DoS 防护策略（安全加固）

> 系统天然具备安全隔离（无 I/O、无内存操作、无系统调用），  
> 以下防护主要针对资源消耗类攻击。

| 优先级 | 防护项 | 说明 |
|-------|--------|------|
| P0 | VM 指令执行上限 | 防止死循环 / 超长计算耗尽 CPU |
| P1 | 字节码指令数上限 | 防止超长函数生成大量指令占用内存 |
| P1 | 常量池大小上限 | 防止大量唯一常量膨胀内存 |
| P2 | 源码长度限制 | 防止超大源码消耗解析内存 |
| P2 | 解析器递归深度限制 | 防止深度嵌套表达式导致 C++ 调用栈溢出 |

---

## 设计原则

1. **专注数学** — 只做 GLSL 风格的数学/逻辑运算，不做通用脚本语言
2. **安全优先** — 语言能力最小化，天然隔离；资源消耗有上限
3. **性能可控** — 无 GC、无动态分派、执行开销上界明确
4. **渐进演进** — 每个 Phase 独立可交付，不依赖后续 Phase

