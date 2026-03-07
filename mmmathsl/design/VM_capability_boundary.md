## VM完整能力限制(DoS保护限制)
- 源码	MAX_SOURCE_LENGTH	8KB
- 词法	MAX_IDENTIFIER_LENGTH	64
- 语法	MAX_NESTING_DEPTH	64
- 语法	MAX_EXPRESSION_DEPTH	64
- AST	MAX_AST_NODES	10,000
- 编译	MAX_CONSTANT_POOL_SIZE	1,024
- 编译	MAX_LOCAL_VARIABLES	128
- 编译	MAX_FUNCTION_PARAMETERS	16
- 编译	MAX_BYTECODE_INSTRUCTIONS	64K
- 执行	MAX_INSTRUCTIONS	10,000,000
## VM能力的一些限制
- 最大 AST 节点数: 10000
- 最大源码长度: 8k
- if 嵌套深度最大层数: 32
- 表达式深度最大层数: 64
- 最大寄存器使用数量: 128
- 编译期最大字节码指令数量: 65536
- 运行期最大指令执行步数: 10,000,000
- 最大内建函数调用次数: 不需要再特别防护。
- 最大矩阵尺寸:（固定已解决）。
- 浮点异常防护（NaN/Inf）：已经有了防护。
## 备注:
- 最大字节码指令数量, 这是编译期限制。意思是：这个脚本最多可以生成多少条字节码指令。最大执行步数,这是运行期限制。意思是：VM 最多允许执行多少条指令。这两者是有重要区别的。