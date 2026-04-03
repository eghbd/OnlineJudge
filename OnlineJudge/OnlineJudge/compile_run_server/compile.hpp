// 用于编译的文件
// 用于编译的文件
// 用于编译的文件
// 用于编译的文件
#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../common/util.hpp"
#include "../common/log.hpp"

//用于编译的类
namespace oj_compiler
{
    using namespace oj_util;
    using namespace oj_log;
    class Compiler
    {
    public:
        Compiler(){}
        ~Compiler(){}
        // 原理：我们通过子进程来编译对应文件
        // 父进程等待子进程完成编译工作
        // 最后通过判断是否形成可执行程序来确定编译是否成功
        static bool Compile(const std::string &file_name)
        {
            pid_t pid = fork();
            if(pid < 0)
            {
                LOG(ERROR) << "创建子进程失败" << "\n";
                return false;
            }
            else if(pid == 0)
            {
                umask(0);
                // 创建并以只写模式打开编译错误用的日志文件
                int _stderr = open(PathUtil::CompilerError(file_name).c_str(), O_CREAT | O_WRONLY, 0644);
                //小于0说明没有成功形成stderr文件
                if(_stderr < 0)
                {
                    LOG(WARNING) << "没有成功形成编译错误文件";
                    exit(1);
                }
                //重定向标准错误到_stderr，让原来的标准错误输出到_stderr文件中
                dup2(_stderr, 2);
                // 前期工作完成，创建了编译错误的日志文件
                // 并且重定向了标准错误，接下来就可以调用编译器进行编译了

                // execlp系统调用执行g++编译器（execlp启动成功后不执行后方命令）
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(),PathUtil::Src(file_name).c_str(), "-D", "COMPILER_ONLINE", "-std=c++11", nullptr);
                // execlp返回，说明启动编译器失败
                LOG(ERROR) << "启动编译器失败";
                exit(2);
            }
            // 父进程等待子进程完成编译工作
            else 
            {
                waitpid(pid, nullptr, 0);
                // 编译是否成功，就看有没有形成对应的可执行程序
                std::string exe_file = PathUtil::Exe(file_name);
                if(FileUtil::FileIsExist(exe_file))
                {
                    chmod(exe_file.c_str(), 0755);
                    LOG(INFO) << PathUtil::Src(file_name) << " 编译成功!";
                    return true;
                }
                LOG(ERROR) << "编译失败，没有形成可执行程序";
                return false;
            }
        }
    };
}