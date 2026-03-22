//编译(compile)和运行(run)两个功能的合并

//该文档包含的接口有：
//CodeToDesc: 用于判断状态码的接口，判断是否运行成功，如果不成功又是因为什么
//RemoveTempFile: 移除临时文件
//Start: 接收用户的代码,返回给用户对应结果
#pragma once

#include "compiler.hpp"
#include "runner.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"

#include <signal.h>
#include <unistd.h>
#include <jsoncpp/json/json.h>

namespace ns_compile_and_run
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_compiler;
    using namespace ns_runner;

    class CompileAndRun
    {
    public:
        //用于判断状态码的接口，判断是否运行成功，不成功又是因为什么
        static std::string CodeToDesc(int code, const std::string &file_name)
        {
            std::string desc;
            switch (code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case -1:
                desc = "提交的代码是空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3:
                // desc = "代码编译的时候发生了错误";
                FileUtil::ReadFile(PathUtil::CompilerError(file_name), &desc, true);
                break;
            case SIGABRT: // 6
                desc = "内存超过范围";
                break;
            case SIGXCPU: // 24
                desc = "CPU使用超时";
                break;
            case SIGFPE: // 8
                desc = "浮点数溢出";
                break;
            default:
                desc = "未知: " + std::to_string(code);
                break;
            }

            return desc;
        }

        //移除临时文件
        //通过文件名+指定后缀，然后通过unlink函数的调用来删除文件
        static void RemoveTempFile(const std::string &file_name)
        {
            //清理文件的个数是不确定的，但是有哪些我们是知道的
            std::string _src = PathUtil::Src(file_name);
            if(FileUtil::IsFileExists(_src)) unlink(_src.c_str());

            std::string _compiler_error = PathUtil::CompilerError(file_name);
            if(FileUtil::IsFileExists(_compiler_error)) unlink(_compiler_error.c_str());

            std::string _execute = PathUtil::Exe(file_name);
            if(FileUtil::IsFileExists(_execute)) unlink(_execute.c_str());

            std::string _stdin = PathUtil::Stdin(file_name);
            if(FileUtil::IsFileExists(_stdin)) unlink(_stdin.c_str());

            std::string _stdout = PathUtil::Stdout(file_name);
            if(FileUtil::IsFileExists(_stdout)) unlink(_stdout.c_str());

            std::string _stderr = PathUtil::Stderr(file_name);
            if(FileUtil::IsFileExists(_stderr)) unlink(_stderr.c_str());
        }
        
        //Start 接口参数所包含的内容展示
        //  in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
        //  out_json: {"status":"0", "reason":"","stdout":"","stderr":"",}
        /////////////////////////
        //输入用的in_json
        //  code： 用户提交的代码
        //  input: 用户给自己提交的代码对应的输入，我们不做处理
        //  cpu_limit: 时间要求
        //  mem_limit: 空间要求
        /////////////////////////
        //输出用的out_json:
        //必填
        //  status: 状态码
        //  reason: 请求的结果
        //选填：
        //  stdout: 程序运行完的结果
        //  stderr: 程序运行完的错误结果

        //该接口作用：接收用户的in_json,返回给用户对应信息的out_json
        static void Start(const std::string &in_json, std::string *out_json)
        {
            //定义一个可以存储任意JSON数据类型的对象，类似map
            Json::Value in_value;
            //解析输入JSON字符串到对象里
            Json::Reader reader;
            //把JSON格式的字符串（in_json）转换成可操作的Json::Value对象
            reader.parse(in_json, in_value); //最后在处理差错问题

            //提取对应信息
            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            int status_code = 0;
            Json::Value out_value;
            int run_result = 0;
            std::string file_name; //需要内部形成的唯一文件名

            //下面我们需要考虑用户上传的代码可能存在的问题
            //比如：代码为空，编译失败，运行时崩溃，运行时超时等

            //代码为空
            if (code.size() == 0)
            {
                status_code = -1; 
                //goto的作用是如果在某个步骤发生了错误
                //就直接跳转到END标签那里，进行结果的返回和临时文件的清理
                goto END;
            }
            //形成唯一的文件名
            //保证文件名唯一值的方法：时间+原子性递增唯一值、时间戳（毫秒级）+ 随机数（更高唯一性）
            //我们使用[毫秒级时间戳+原子性递增唯一值]来保证唯一性
            file_name = FileUtil::UniqFileName();
            //形成临时src文件
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code))
            {
                status_code = -2; //未知错误
                goto END;
            }
            //编译失败
            if (!Compiler::Compile(file_name))
            {
                status_code = -3; //代码编译的时候发生了错误
                goto END;
            }

            run_result = Runner::Run(file_name, cpu_limit, mem_limit);
            if (run_result < 0)
            {
                status_code = -2; //未知错误
            }
            else if (run_result > 0)
            {
                //程序运行崩溃了
                status_code = run_result;
            }
            else
            {
                //运行成功
                status_code = 0;
            }
        END:
            out_value["status"] = status_code;
            out_value["reason"] = CodeToDesc(status_code, file_name);
            if (status_code == 0)
            {
                // 整个过程全部成功
                std::string _stdout;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout, true);
                out_value["stdout"] = _stdout;

                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr, true);
                out_value["stderr"] = _stderr;
            }

            Json::StyledWriter writer;
            *out_json = writer.write(out_value);

            RemoveTempFile(file_name);
        }
    };
}
