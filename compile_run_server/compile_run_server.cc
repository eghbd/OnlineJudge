//把编译运行的接口进行封装，提供给外部调用
//对应主机接收到运行任务后，就调用这个文档生成的运行文件进行处理

// 启动的端口为8081,其他端口展示还没扩展,后续可在control添加对应的端口分配功能,即负载均衡功能
// 启动的端口为8081,其他端口展示还没扩展,后续可在control添加对应的端口分配功能,即负载均衡功能
// 启动的端口为8081,其他端口展示还没扩展,后续可在control添加对应的端口分配功能,即负载均衡功能

#include "compile_run.hpp"
#include "../common/httplib.h"

using namespace oj_compile_run;
using namespace httplib;

int main(int argc, char *argv[])
{   
    //传入的为：程序名 + 端口号
    if(argc != 2){
        std::cerr << "Usage: \n\t" << argv[0] << " port" << std::endl;
        return 1;
    }

    //Server为服务器类
    //创建http服务器对象
    Server svr;
    //req相对于传入的待处理文件信息，resp相对于返回给前端的结果
    svr.Post("/compile_run", [](const Request &req, Response &resp)
    {
        std::string in_json = req.body;
        std::string out_json;
        if(!in_json.empty())
        {
            CompileRun::Start(in_json, &out_json);
            //设置响应内容，返回结果JSON，指定格式为UTF-8的JSON
            resp.set_content(out_json, "application/json;charset=utf-8");
        }
        //if(!in_json.empty())
        //{
        //     try{
        //         CompileRun::Start(in_json, &out_json);
        //     }catch(...){
        //         out_json = "{\"status\":-1,\"reason\":\"编译运行失败\"}";
        //     }
        // }else{
        //     out_json = "{\"status\":-1,\"reason\":\"请求为空\"}";
        // }
        //  严格返回JSON，禁止任何额外字符/换行
        //resp.set_content(out_json, "application/json");
    });
    //  启动服务
    //  0.0.0.0：监听所有网卡,atoi(argv[1])：把命令行参数转为端口号
    //  服务器开始阻塞运行，等待请求
    svr.listen("0.0.0.0", atoi(argv[1]));
    return 0;
    
}
