#pragma once
#include <iostream>

#include <string>
#include <vector>

#include <fstream>

#include <algorithm>
#include <mutex>
#include <cassert>

#include <algorithm>  
#include <jsoncpp/json/json.h>

#include "../common/httplib.h"
#include "../common/log.hpp"
#include "../common/util.hpp"
#include "view.hpp"
#include "model.hpp"


#include <unistd.h>
#include <sys/wait.h>


namespace oj_control
{
    using namespace oj_log;
    using namespace oj_util;
    using namespace oj_view;
    using namespace oj_model;
    using namespace httplib;

    class Control
    {
    private:
        Model _model;
        View _view;

        const std::string compile_ip = "127.0.0.1";
        int compile_port = 8081;

    public:
        Control(){}
        ~Control(){}
    public:
        bool QuestionsList(std::string* html)
        {
            assert(html != nullptr);
            //获取题目,通过接口调用以及预加载好的model
            std::vector<Question> all;
            if(_model.GetAllQuestions(&all))
            {
                std::sort(all.begin(), all.end(), [](const Question& q1, const Question& q2)
                {
                    return std::stoi(q1.num) < std::stoi(q2.num);
                });
                _view.AllExpandHtml(all, *html);
                return true;
            }
            else
            {
                *html = "获取题目列表失败";
                return false;
            }
        }
        bool GetOneQuestion(const std::string& num, std::string* html)
        {
            assert(html);
            Question q;
            if(_model.GetQuestion(num, q))
            {  
                _view.OneExpandHtml(q, *html);
                return true;
            }
            else
            {
                *html = "题目" + num + "不存在";
                return false;
            }
        }
        
        //这里出现很多问题了，只能一部分一部分的打印，所以添加了大量的日志进行追踪

        //问题的解决过程总结: 

        // 1. 前端提交代码后，却显示超时，我认为
        //      解决：首先我打印了ip与端口，发现只打印了ip却没打印接口,测试后发现我的日志无法打印int类型，导致整个代码停顿，后用to_string进行转换
        // 2.（耗时最长，卡了我一天的时间,最后发现是前端转义问题 ）头文件消失问题，#include<iostream>变成#include
        //      解决:一开始我认为是model读取文件存在问题，我去测试了model文件的读取接口，没发现问题
        //      又觉得拼接出现问题，于是我在test文件夹里进行了拼接测试并生成文件进行查看，但发现拼接没问题
        //      随后我使用了第二种方法，追踪代码并打印代码以及拼接后的代码，通过追踪，std::string code = in_value["code"].asString();这行代码执行时头文件已经丢失
        //      查资料，觉得是Json::Reader存在转义问题，所以通过Json::StreamWriterBuilder来尝试解决问题，但并未解决，我单独开个文档测试后，发现也不是这部分有问题
        //      但通过日志的打印，我确定就出现在《解析前端传入JSON》这部分，且通过model得到的代码没有问题
        //      随后花费时间不断试错我的后端代码，都没发现头文件丢失的原因
        //      
        //      突然发现，我好像没验证前端传回的代码了！！！对啊，后端找不到问题，那是不是前端存在问题！！！（我前端的相应知识储备较少，一直没意识到前端也可能存在问题）
        //      随后我对in_json（前端传来的信息）进行打印，终于发现了问题！！！在前端传来的代码本身以及丢失了头文件
        //      通过相关知识的学习，知道了前端在对部分符号会存在转义，导致我头文件符号<>被转义
        //      通过禁止前端的转义后，传给后端的代码不再存在头文件丢失问题，由此问题解决
        //3.段错误
        //      前端运行代码却返回段错误，通过日志追踪，Judge函数全步骤的运行已经不存在问题，超时的话我觉得是后端判题存在问题
        //      但我单独测试过后端功能（请看test文件夹内Compile_run_server）,且对各种情况进行测试，且单独对拼接后的文件进行了测试，都没问题
        //      运行文件没问题，编译服务也没问题，随后我去检查是否路径以及文件权限是不是出了问题
        //
        //      查路径原因：没有形成对应的执行文件路径可能会导致段错误；
        //      查权限：没对应的权限，也没办法运行创建的临时文件
        //      都没问题，而我的后端判题，运用了沙箱的思路，那就检查沙箱的设置，追踪资源
        //      
        //      最后发现是cpu与内存的资源给少了
        //      原因是我内存设置的是10*1024*1024,10mb的大小
        //      但model读取时只进行了读取，而不是运算后读取，导致传的内存限制为10而不是10*1024*1024，严重的内存不足导致了段错误
        void Judge(const std::string& num, const std::string& in_json, std::string* out_json)//out_json可能为空，所以使用指针
        {
            // ===================== 初始化日志 =====================
            LOG(INFO) << "===== Judge函数开始执行 =====" << std::endl;
            LOG(INFO) << "连接编译服务器 IP: " << compile_ip << std::endl;
            LOG(INFO) << "连接编译服务器端口: " << std::to_string(compile_port) << std::endl;
            LOG(INFO) << "===== 日志打印完成，准备获取题目 =====" << std::endl;

            // =====================  获取题目对象 =====================
            LOG(INFO) << "DEBUG: 开始定义题目对象" << std::endl;
            Question q;

            LOG(INFO) << "DEBUG: 开始获取题目，题号：" << num << std::endl;
            if(!_model.GetQuestion(num, q))
            {
                Json::Value err;
                err["status"] = -1;
                err["reason"] = "题目不存在";
                *out_json = Json::FastWriter().write(err);
                LOG(INFO) << "DEBUG: 题目不存在！" << std::endl;
                return;
            }
            LOG(INFO) << "DEBUG: 题目获取成功！" << std::endl;

            std::string safe_tail = q.tail;
            // 打印题目核心信息（对头文件丢失问题进行追踪）
            LOG(INFO) << "DEBUG: 题目header内容:\n" << q.header << std::endl;
            //LOG(INFO) << "DEBUG: 题目tail内容:\n" << safe_tail << std::endl;
            LOG(INFO) << "DEBUG: 题目cpu_limit:" << q.cpu_limit << std::endl;
            LOG(INFO) << "DEBUG: 题目mem_limit:" << q.mem_limit << std::endl;

            // =====================  解析前端传入JSON =====================
            LOG(INFO) << "DEBUG: 开始解析前端JSON" << std::endl;
            Json::Reader read;
            Json::Value in_value;
            if(!read.parse(in_json, in_value))
            {
                Json::Value err;
                err["status"] = -1;
                err["reason"] = "JSON解析失败";
                *out_json = Json::FastWriter().write(err);
                LOG(INFO) << "DEBUG: JSON解析失败！" << std::endl;
                return;
            }
            LOG(INFO) << "DEBUG: 前端JSON解析成功！" << std::endl;
            // 打印前端传来的in_json
            // LOG(INFO) << "DEBUG: 前端原始JSON：" << std::string(in_json) << std::endl;

            // =====================  拼接用户代码 + 测试用例 =====================
            //  打印代码（对头文件丢失问题进行追踪，发现问题！！！code就获取后头文件丢失了，而q.header时并未丢失，所以问题就在问题就在这中间）
            LOG(INFO) << "DEBUG: 开始拼接用户代码与测试代码" << std::endl;
            std::string code = in_value["code"].asString();
            LOG(INFO) << "code代码展示 \n" << code << std::endl;
            std::string tail_code = safe_tail.empty() ? "" : safe_tail;
            std::string total_code = code + "\n" + tail_code;
            LOG(INFO) << "DEBUG: 代码拼接成功！\n" << total_code << std::endl;

            // =====================  构造请求JSON =====================
            LOG(INFO) << "DEBUG: 开始构造编译请求JSON" << std::endl;
            Json::Value splice_value;
            splice_value["input"] = in_value["input"].asString();
            splice_value["code"] = total_code;
            splice_value["cpu_limit"] = q.cpu_limit;
            splice_value["mem_limit"] = q.mem_limit;

            // 禁止转义，完整保留<>(对头文件<>消失的问题所做的一次处理，但并未解决问题)
            Json::StreamWriterBuilder builder;
            builder["commentStyle"] = "None";
            builder["indentation"] = ""; // 压缩格式
            builder["emitUTF8"] = true;
            std::string splice_string = Json::writeString(builder, splice_value);
            
            LOG(INFO) << "DEBUG: 编译请求JSON生成成功！" << std::endl;

            // =====================  发送网络请求 =====================
            // 用try{}catch(...){}来监控返回错误
            try{
                LOG(INFO) << "===== 开始创建HTTP客户端 =====" << std::endl;
                Client* cli = new Client(compile_ip, compile_port);
                
                // 客户端配置
                cli->set_keep_alive(false);
                cli->set_connection_timeout(3, 0);
                cli->set_read_timeout(10, 0);
                cli->set_write_timeout(5, 0);
                LOG(INFO) << "DEBUG: HTTP客户端配置完成" << std::endl;

                LOG(INFO) << "===== 发送请求到编译服务 =====" << std::endl;
                auto res = cli->Post("/compile_run", splice_string, "application/json;charset=utf-8");

                // 处理响应
                if(res && res->status == 200)
                {
                    LOG(INFO) << "编译服务响应成功！" << std::endl;
                    *out_json = res->body;
                }
                else
                {
                    LOG(INFO) << "编译服务无响应/异常" << std::endl;
                    Json::Value err;
                    err["status"] = -1;
                    err["reason"] = "编译服务连接失败";
                    *out_json = Json::FastWriter().write(err);
                }

                delete cli;
                LOG(INFO) << "DEBUG: HTTP客户端资源释放完成" << std::endl;
                return;
            }
            catch(...)
            {
                LOG(INFO) << " 网络请求发生异常" << std::endl;
                Json::Value err;
                err["status"] = -1;
                err["reason"] = "网络请求崩溃";
                *out_json = Json::FastWriter().write(err);
            }
        }
    };
}