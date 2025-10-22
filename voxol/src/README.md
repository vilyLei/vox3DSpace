#
Scene-Oriented, Backend-Agnostic Rendering Pipeline.Python as Render IR.Design driven by computing power and AI Thinking.A General Graphic Architecture Model for Computing Power and Semantics.
#
Web 端 10 million renderable graphics primitives 规模, PC Native 端 100 million renderable graphics primitives 规模, Mobile 端 1 million renderable graphics primitives 规模
#
3D,2D,视频等的混合编辑/展示/交互
#
对于编辑应用方向，目标是无约束的编辑设计
#
编辑，播放，这两个看似不同的系统，实际上核心系统应该基于同一机制。
编辑可以看做是编辑器或者设计工具，也就是内容生产系统。
而播放可以看做是内容应用系统。

#
总体上，核心系统应该分为三大方向:
1. 数据生成。包括几何数据，色彩数据，其他描述数据等等。
2. 编辑系统。例如是个编辑器或者设计工具。
3. 播放\展示\应用系统。例如可以组合为一个游戏player。

#
基于协议、数据、命令的分离式的、算力驱动的系统设计, Ai逻辑协同。
FFI 隔离与同步。
主系统分为两端: 驱动端(生产端), 执行端(消费端)
驱动端产生命令队列，而执行端则解释并执行这些命令。
驱动端不会包含具体表现所需要的资源，而执行端则要拥有这些资源的管理能力。这也是分布式机制的设计原则。
两端通过协议和命令协作与关联。因此可以分别在不同的独立进程里各司其职。甚至可以在不同的物理核心上建立对应关联关系。着意味着逻辑(生产)、内容(传输)、表达(展示)三阶段可以完全解耦。这三阶段的数据表述形态可以独立存储传输应用。
引擎编排系统（Engine Orchestrator）
Render Task Graph / Render IR
#
这种空间管理机制下，必然会出现2倍的渲染实现效果
#
[Input Layer] ─▶ [Cascaded Controllers] ─▶ [Scene Logic & Task Flow]
       │                                  │
       ▼                                  ▼
 (UI Controller)                    (UI Scene / Entity Scene)
       │                                  │
       └──────────────▶ [Rendering System] ─▶ [Backend: Vulkan / OpenGL / Remote]
#
UI Scene ─┬─> buildRenderIR() ─┐
           │                    │
Entity Scene ─> buildRenderIR() ─┼──> [Render Graph Builder]
                                 │
                             Merged IR
#
分布式交互–渲染拓扑系统
数据、输入、计算、渲染、反馈都被组织在一张**动态拓扑图（Graph Topology）里
#
核心机制可直接跨平台迁移/分发/协作