// 后端判题的核心服务

#include "compile_run.hpp"
#include "../comm/httplib.h"

using namespace ns_compile_and_run;
using namespace httplib;

void Usage(std::string proc)
{
    std::cerr << "Usage: " << "\n\t" << proc << " port" << std::endl;
}

//编译服务随时可能被多个人请求，必须保证传递上来的code，形成源文件名称的时候，要具有唯一性，要不然多个用户之间会互相影响
//./compile_server port
int main(int argc, char *argv[])
{
    if(argc != 2){
        Usage(argv[0]);
        return 1;
    }
    // 2. 创建Server对象（HTTP服务器核心对象）
    Server svr;

    svr.Post("/compile_and_run", [](const Request &req, Response &resp){
        // 用户请求的服务正文是我们想要的json string
        std::string in_json = req.body;
        //out_json是我们想要返回给用户的json字符串
        std::string out_json;
        if(!in_json.empty()){
            //通过调用compile_and_run模块的Start接口来完成编译和运行的功能
            CompileAndRun::Start(in_json, &out_json);
            //设置响应内容，返回结果JSON，指定格式为UTF-8的JSON
            resp.set_content(out_json, "application/json;charset=utf-8");
        }
    });
    svr.listen("0.0.0.0", atoi(argv[1])); //启动http服务
    return 0;
}