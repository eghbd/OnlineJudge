//编译运行接口进行整合封装的文件
//编译运行接口进行整合封装的文件
//编译运行接口进行整合封装的文件

//g++ Compile_Run_test.cpp -o test -ljsoncpp -pthread

//OJ中，编译和运行功能分开是核心的架构决策
//本质上是为了逻辑清晰、错误隔离、资源管控和安全保障。
//简单来说，编译和运行是两个不同的阶段，涉及不同的操作和风险。将它们分开可以让系统更健壮、更安全、更易于维护。

#include <iostream>
#include <string>

#include <signal.h>
#include <unistd.h>
#include <jsoncpp/json/json.h>

#include "../common/util.hpp"
#include "../common/log.hpp"
#include "compile.hpp"
#include "run.hpp"
namespace oj_compile_run
{
    using namespace oj_run;
    using namespace oj_compiler;
    using namespace oj_util;
    using namespace oj_log;
    class CompileRun
    {
    public:
        CompileRun() {}
        ~CompileRun() {}  
    public:
        //RAII临时文件守卫类
        class TempFileGuard
        {
        public:
            TempFileGuard(const std::string &file_name)
                :_file_name(file_name){}
            ~TempFileGuard()
            {
                CompileRun::RemoveTempFile(_file_name);
            }
        private:
            std::string _file_name;
        };
        //判断是否存在该文件
        static bool IsExist(const std::string &path)
        {
            struct stat s;
            if(stat(path.c_str(), &s) == 0)
            {
                return true;
            }
            return false;
        }
        // 辅助删除临时文件的函数
        static void RemoveFile(const std::string &path)
        {
            if(IsExist(path))unlink(path.c_str());
        }
        // 删除临时文件
         static void RemoveTempFile(const std::string &file_name)
        {
            // .cpp
            RemoveFile(PathUtil::Src(file_name));
            // .exe
            RemoveFile(PathUtil::Exe(file_name));
            // .compile_error
            RemoveFile(PathUtil::CompilerError(file_name));
            // .stdin
            RemoveFile(PathUtil::Stdin(file_name));
            // .stdout
            RemoveFile(PathUtil::Stdout(file_name));
            // .stderr
            RemoveFile(PathUtil::Stderr(file_name));
        }
        
        // 用于描述状态
        static std::string Desc(const std::string file_name, int code)
        {
            std::string desc;
            switch(code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case -1:
                desc = "代码为空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3: //编译错误，需要读取对应错误文件获取错误信息
                if(IsExist(PathUtil::CompilerError(file_name)))
                {
                    FileUtil::ReadFile(PathUtil::CompilerError(file_name), &desc, true);
                }
                else
                {
                    desc = "文档不存在";
                }
                break;
            case SIGABRT: //6
                desc = "程序异常";
                break;
            case SIGXCPU: //24
                desc = "CPU使用超时";
                break;
            case SIGFPE: //8
                desc = "浮点数移除或除零错误";
                break;
            case SIGSEGV: //11
                desc = "段错误";
                break;
            default:
                desc = "未知错误，状态码:" + std::to_string(code);
                break;
            }
            return desc;
        }


        // 后端接口，功能：接收代码，返回结果
        // in_json里有code(代码),input(输入),cpu_limit(cpu限制),mem_limit(内存限制)
        static void Start(const std::string &in_json, std::string *out_json)
        {
            // ✅ 核心修复：所有变量提前声明，解决goto编译报错
            Json::Value in_value;
            Json::Reader reader;
            int status_code = 0;//http状态码
            Json::Value out_value;
            int run_result = 0;//用于判断运行结果
            std::string file_name = ""; // 初始化空字符串，解决段错误

            // 所有被goto跳过的变量，全部提前声明
            std::string code;
            std::string input;
            int cpu_limit;
            int mem_limit;
            TempFileGuard* guard = nullptr; // 初始化为空
            std::string _stdout;
            std::string _stderr;

            //把相关数据序列号
            Json::FastWriter writer;

            if(!reader.parse(in_json,in_value))
            {
                status_code = -2;
                goto END;
            }

            //对信息进行提取
            //传部分需要调整的限制即可，其他的如钟墙，进程限制等等我们先行设置好即可
            code = in_value["code"].asString();
            input = in_value["input"].asString();
            cpu_limit = in_value["cpu_limit"].asInt();
            mem_limit = in_value["mem_limit"].asInt();
            
            //我们可以排除可能出现的问题了
            if(code.empty())
            {
                status_code = -1;
                goto END;
            }
            file_name = FileUtil::UniqFileName();
            //引入RAII守卫管理临时文件的生命周期
            guard = new TempFileGuard(file_name);

            //把code内容写入文件
            if(!FileUtil::WriteIntoFile(PathUtil::Src(file_name), code))
            {
                status_code = -2;
                goto END; 
            }

            if(!Compiler::Compile(file_name))
            {
                status_code = -3;
                goto END;
            }
            //运行可能存在很多结果
            run_result = Run::RunCode(file_name, cpu_limit, mem_limit);
            if(run_result == 0) // 运行成功
            {
                status_code = 0;
            }
            else if(run_result > 0)
            {
                status_code = run_result;
            }
            else
            {
                status_code = -2;
            }

        END:
            out_value["status"] = status_code;
            //通过状态码，统一返回描述
            out_value["reason"] = Desc(file_name, status_code);
            if(status_code == 0)
            {
                if(FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout, true))
                {
                    out_value["stdout"] = _stdout;
                }
                else
                {
                    out_value["stdout"] = "";
                    LOG(WARNING) << "stdout文件打开失败" << "\n";
                }

                if (FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr, true))
                {
                    out_value["stderr"] = _stderr;
                }
                else
                {
                    out_value["stderr"] = "";
                    LOG(WARNING) << "stderr文件打开失败" << "\n";
                }
            }
            else
            {
                out_value["stdout"] = "";
                out_value["stderr"] = "";
            }
            
            *out_json = writer.write(out_value);

            if(guard) delete guard;
        }
    };
}