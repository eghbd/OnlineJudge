1. comm: 公共模块
2. compile_server: 编译与运行模块
3. oj_server: 获取题目列表，查看题目编写题目界面，负载均衡，其他功能

<!-- compile_server文件夹下的文件功能 -->
compile.hpp:编译代码的接口
runner.hpp:运行代码的接口
compile_run.hpp:编译与运行接口的融合，用于编译与运行代码（但是只运行，判断运行状态，不保证结果）
compile_server.cc：后端运行的核心部分，负责接收前端代码，并通过接口进行编译运行代码，返回对应的结果给用户


<!-- oj_servr文件夹下的文件功能 -->
conf文件夹：用于存放网络地址和端口
Home文件夹：用于存放前端的首页代码
questions文件夹：用于存放题目
template_html文件夹：用于存放其他页面的前端代码

引用了mvc模式,即模型(model),视图(view),控制器(control)进行框架的构建
oj_control.hpp：


