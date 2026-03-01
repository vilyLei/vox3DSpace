## 1. 系统概述
- **目标**：实现一个支持glsl数值类型及计算语法的c++运行时数学计算过程的解析器
- **范围**：整个工程最终可以打包为一个静态库被别人调用
- **功能需求**：
  - 使用glsl的语法。
  - 主要支持的数值类型包括: int, float, bool, vec2, vec3, vec4, mat2, mat3, mat4。这也说明要支持矢量运算和矩阵运算。
  - 能在运行时支持这样的脚本： 
  vec2 calcPosition(float i){
    vec2 pv = {30,30};
    vec2 disV = {20,20};
    float cn = 5;
    float r = floor(i/5);
    float c = i - (floor(i/5) * cn);
    vec2 tempV = {c, r};
    vec2 pos = pv + tempV * disV;
    return pos;
}
  - 以PIMP实现头文件及源文件管理机制，保障对源码的封装.
也就是能在c++中通过calcPosition这个函数名调用这个函数获得结果。
  - 类似于创建一个能解释执行glsl数学计算逻辑的子集的系统。
  - 语法风格保持和glsl一致。
  - 创建项目的cmakelists文件，期望能用cmake构建和编译项目。cmake版本是3.28。会用cmake来创建visual studio项目，我用的是visual studio 2022.
  - 用c++20标准实现相关代码。
- **相关库及头文件**：
  - glm源码: D:\dev\webdev\vox3DSpace\space3d\externals\glm