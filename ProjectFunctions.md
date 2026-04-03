整个oj文件的介绍

1. common: 公共模块
2. compile_run_server: 编译与运行模块
3. oj_server: 获取题目列表，查看题目编写题目界面，负载均衡，其他功能

<!-- compile_server文件夹下的文件功能 -->
compile.hpp:编译代码的接口
run.hpp:运行代码的接口
compile_run.hpp:编译与运行接口的融合，用于编译与运行代码（但是只运行，判断运行状态，不保证结果）
compile_run_server.cc：后端运行的核心部分，负责接收前端代码，并通过接口进行编译运行代码，返回对应的结果给用户
temp文件夹：用于生成的临时文档的存放

<!-- oj_servr文件夹下的文件功能 -->
Home文件夹：用于存放前端的首页代码
questions文件夹：用于存放题目
Interface文件夹：用于存放其他页面的前端代码

<!-- test -->
用于各种测试的文件夹

<!-- 端口启动 -->
    <!-- 编译服务 -->
./compile_run_server 8081
    <!-- 前端与后端交互的服务 -->
./oj_server

<!-- 模式说明 -->
引用了mvc模式,即模型(model),视图(view),控制器(control)进行框架的构建
基于 MVC 设计模式完成 OJ（在线判题）系统核心框架搭建：
模型层（model.hpp）：负责数据封装、业务逻辑处理（如题目数据管理、判题核心逻辑）；
视图层（view.hpp）：负责数据渲染与用户交互界面展示（如题目列表、判题结果输出）；
控制器层（control.hpp）：作为核心调度层，接收请求并协调模型与视图完成数据流转。

<!-- 查看html文件的方法 -->
下载live server插件，然后右键文件选择Open with Live Server

