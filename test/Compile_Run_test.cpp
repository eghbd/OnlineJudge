//编译与运行整合后的接口测试文件
//编译与运行整合后的接口测试文件
//编译与运行整合后的接口测试文件

//测试内容：正常运行、代码为空、编译报错、除零、死循环

//g++ Compile_Run_test.cpp -o compile_run_test -ljsoncpp -pthread

#include<iostream>
#include<string>
#include<jsoncpp/json/json.h>
#include "../compile_run_server/compile_run.hpp"

#include "../common/util.hpp"
#include "../common/log.hpp"
using namespace std;
using namespace oj_compile_run;
using namespace oj_util;
using namespace oj_log;

void PrintJson(const std::string &out_json)
{   
    
    static int i = 0;
    i++;
    Json::Value res;
    Json::Reader reader;
    if(reader.parse(out_json, res))
    {
        cout << "测试" << i << "结果:" << endl;
        cout << "status:" << res["status"].asInt() << endl;
        cout << "reason:" << res["reason"].asString() << endl;
        if(res.isMember("stdout"))
            cout << "stdout:" << res["stdout"].asString() << endl;
        if(res.isMember("stderr"))
            cout << "stderr:" << res["stderr"].asString() << endl;
    }   
    cout << "/////////////////////////////////" << endl;
}

int main()
{
    //用于把json转成字符串
    Json::FastWriter writer;

    cout << "测试1.正确代码" << "\n";
    Json::Value in_json1;
    in_json1["code"] = R"(
#include<iostream>
using namespace std;
int main()
{
    cout << "hello world" << endl;
    return 0;
}
)";
    in_json1["input"] = "";
    in_json1["cpu_limit"] = 1;
    in_json1["mem_limit"] = 10*1024;
    string out_json1;
    CompileRun::Start(writer.write(in_json1), &out_json1);
    PrintJson(out_json1);
    ///////////////////////////////////////////////////////////
    cout << "测试2.代码为空" << "\n";
    Json::Value in_json2;
    in_json2["code"] = R"()";
    in_json2["input"] = "";
    in_json2["cpu_limit"] = 1;
    in_json2["mem_limit"] = 10*1024;
    string out_json2;
    CompileRun::Start(writer.write(in_json2), &out_json2);
    PrintJson(out_json2);
    ////////////////////////////////////////////////////////
    cout << "测试3.编译错误" << "\n";
    Json::Value in_json3;
    in_json3["code"] = R"(
#include <iostream>
using namespace std;
int main() {
    cout << "test" << endl  // 少分号
    return 0; 
}
)";
    in_json3["input"] = "";
    in_json3["cpu_limit"] = 1;
    in_json3["mem_limit"] = 10*1024;
    string out_json3;
    CompileRun::Start(writer.write(in_json3), &out_json3);
    PrintJson(out_json3);
    ////////////////////////////////////////////////////////
    cout << "测试4.除零错误" << endl;
    Json::Value in_json4;
    in_json4["code"] = R"(
#include<iostream>
using namespace std;
int main()
{
    int a = 1;
    int b = 0;
    return a/b;
}    
)";
    in_json4["input"] = "";
    in_json4["cpu_limit"] = 1;
    in_json4["mem_limit"] = 10 * 1024 * 1024;
    string out_json4;
    CompileRun::Start(writer.write(in_json4), &out_json4);
    PrintJson(out_json4);
    ////////////////////////////////////////////////////////
    cout << "测试5.死循环" << "\n";
    Json::Value in_json5;
    in_json5["code"] = R"(
#include <iostream>
using namespace std;
int main() {
    while(1);
    return 0; 
}
)";
    in_json5["input"] = "";
    in_json5["cpu_limit"] = 1;
    in_json5["mem_limit"] = 1024 * 1024 * 256;
    string out_json5;
    CompileRun::Start(writer.write(in_json5), &out_json5);
    PrintJson(out_json5);

    return 0;
}




















