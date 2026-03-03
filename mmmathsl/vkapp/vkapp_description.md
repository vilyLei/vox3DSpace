## 1. 系统概述

- **目标**：基于 Vulkan SDK 实现一个若干视觉显示相关的 Demo。
- **范围**
    - 在 vkapp 里面建立 src 目录，所有源码都在这个目录里面。
    - 按照标准 3D 引擎架构组织：application（应用层）、core（核心层）、rhi（渲染硬件接口）、renderer（渲染层）、scene（场景层）五个模块来实现渲染的公共系统。
    - 先实现绘制一个红色矩形的 Demo。

## 2. 功能需求

- 在 Windows 系统中用 Vulkan context 创建程序，能打开窗口。窗口系统用 GLFW3 实现。
- 创建项目需要的 CMakeLists.txt 文件，期望能用 CMake 构建和编译项目。CMake 版本是 3.25。会用 CMake 来创建 Visual Studio 项目，使用 Visual Studio 2022。
- 用 C++20 标准实现相关代码。

## 3. 目录结构

```
vkapp/
├── src/
│   ├── application/             # 应用层：平台抽象 + 应用生命周期
│   │   ├── platform/
│   │   │   ├── window.hpp           # 窗口接口抽象
│   │   │   └── window_glfw.hpp/cpp  # GLFW 实现
│   │   ├── input.hpp/cpp            # 输入系统
│   │   ├── timer.hpp/cpp            # 帧计时器
│   │   ├── event.hpp                # 事件系统
│   │   ├── application.hpp/cpp      # 应用基类
│   │   └── entry.hpp                # 平台入口封装
│   │
│   ├── core/                    # 核心层：基础工具
│   │   ├── log.hpp/cpp              # 日志系统
│   │   ├── assert.hpp               # 断言宏
│   │   ├── types.hpp                # 基础类型定义
│   │   └── utils.hpp                # 通用工具
│   │
│   ├── rhi/                     # RHI 层：渲染硬件接口（Vulkan）
│   │   └── vulkan/
│   │       ├── vulkan_rhi.hpp/cpp
│   │       ├── vulkan_device.hpp/cpp
│   │       ├── vulkan_swapchain.hpp/cpp
│   │       ├── vulkan_command.hpp/cpp
│   │       ├── vulkan_buffer.hpp/cpp
│   │       ├── vulkan_pipeline.hpp/cpp
│   │       └── vulkan_shader.hpp/cpp
│   │
│   ├── renderer/                # 渲染层：高层渲染逻辑
│   │   ├── renderer.hpp/cpp         # 主渲染器
│   │   ├── render_context.hpp/cpp
│   │   ├── render_pass.hpp/cpp
│   │   ├── camera.hpp/cpp
│   │   └── mesh.hpp/cpp
│   │
│   └── scene/                   # 场景层
│       ├── scene.hpp/cpp
│       └── entity.hpp
│
├── app/                         # 具体应用实现
│   └── red_rect_app.hpp/cpp     # 红色矩形 Demo 应用
│
├── shaders/                     # GLSL 着色器文件
│   ├── rect.vert
│   └── rect.frag
│
├── CMakeLists.txt
└── main.cpp                     # 入口文件
```

## 4. 模块职责

| 模块 | 职责 |
|------|------|
| **application** | 平台窗口抽象（GLFW）、输入处理、事件系统、应用生命周期管理 |
| **core** | 日志、断言、基础类型、通用工具 |
| **rhi** | Vulkan 底层封装：设备、交换链、命令缓冲区、管线、着色器 |
| **renderer** | 高层渲染逻辑：渲染器、渲染 Pass、相机、网格 |
| **scene** | 场景管理、实体系统 |
| **app** | 具体应用实现（红色矩形 Demo）|

## 5. 相关库及头文件

- **Vulkan** 路径的环境变量：`VULKAN_SDK`
- **Vulkan 头文件**：`%VULKAN_SDK%/Include`
- **Vulkan 库位置**：`%VULKAN_SDK%/Lib/vulkan-1.lib`
- **GLM 源码**：`D:/dev/webdev/vox3DSpace/space3d/externals/glm`
- **GLFW3 头文件位置**：`D:/dev/webdev/vox3DSpace/space3d/openglLibs/libx64/include`
- **GLFW3 库位置**：`D:/dev/webdev/vox3DSpace/space3d/openglLibs/libx64/lib/glfw3.lib`
- **着色器编译器**：`%VULKAN_SDK%/Bin/glslc.exe`

## 6. 首个 Demo 目标

- **名称**：Red Rectangle Demo
- **功能**：打开 800x600 窗口，使用 Vulkan 渲染一个红色矩形
- **技术点**：
    - Vulkan Instance 创建
    - 物理/逻辑设备初始化
    - 交换链创建
    - 渲染管线配置
    - 顶点缓冲区
    - 着色器加载（vert/frag）
    - 渲染循环
## 7. 检查与构建
    - 实现代码之后，先检查代码正确性，然后构建验证。