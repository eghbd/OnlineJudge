//用于运行功能的文件
//用于运行功能的文件
//用于运行功能的文件

// 只需要保证文件能不能运行即可                                               
// 不需要在意运行结果是否正确

// 通过学习得知，由于用户代码的不确定性
// 为了避免恶意代码，需要引入一个概念叫沙箱
// 沙箱概念的核心：资源限制 + 超时管控 + 进程隔离

// 沙箱的实现
// ResourceLimit()：设置资源限制的接口，限制用户程序的CPU、内存、时间和文件描述符等资源，防止恶意代码滥用系统资源；
// fork()： 沙箱核心 —— 创建独立子进程运行用户程序，子进程崩溃 / 异常不会影响父进程（服务器主进程）；
// dup2()： 沙箱：将用户程序的输入 / 输出重定向到临时文件，隔离程序的 IO 流，防止输出污染服务器控制台；
// execl()：在子进程中执行用户程序，与父进程完全隔离，父进程只负责监控状态。
#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../common/log.hpp"
#include "../common/util.hpp"

namespace oj_run
{
    //获取路径才能知道要运行哪个文件
    using namespace oj_util;
    using namespace oj_log;

    class Run
    {
    public:
        Run() {}
        ~Run() {}

    public:
        // 设置资源限制的接口
        // 我们需要对用户的代码运行进行些约束，防止恶意代码的导致的问题
        // 我针对了常见的问题进行了限制，如下：CPU资源、内存资源、时间资源、文件描述符等资源，并设置了默认值，需要时再进行传参
        static void ResourceLimit(int cpu_limit = 1, int mem_limit = 10*1024*1024, int time_limit = 5, int fd_limit = 64)
        {
            // 设置CPU时长限制
            struct rlimit cpu_rlimit;
            //RLIM_INFINITY的意思是没有限制
            cpu_rlimit.rlim_max = RLIM_INFINITY;
            cpu_rlimit.rlim_cur = cpu_limit;
            if (setrlimit(RLIMIT_CPU, &cpu_rlimit) < 0) {
                LOG(ERROR) << "设置CPU资源限制失败" << "\n";
            }
            // 设置内存大小限制
            struct rlimit mem_rlimit;
            mem_rlimit.rlim_max = RLIM_INFINITY;
            mem_rlimit.rlim_cur = mem_limit;
            if (setrlimit(RLIMIT_AS, &mem_rlimit) < 0) {
                LOG(ERROR) << "设置内存资源限制失败" << "\n";
            }

            // 钟墙时间
            // linux没有直接限制时间的系统调用，但可以通过alarm和SIGALRM信号来实现时间限制
            alarm(time_limit); // 设置闹钟，时间到会发送SIGALRM信号

            // 设置文件描述符限制
            struct rlimit fd_rlimit;
            fd_rlimit.rlim_max = RLIM_INFINITY;
            fd_rlimit.rlim_cur = fd_limit;
            if (setrlimit(RLIMIT_NOFILE, &fd_rlimit) < 0) {
                LOG(ERROR) << "设置文件描述符资源限制失败" << "\n";
            }

            //除此之外还可以设置如：禁用核心转存、限制进程数量等资源限制

            //核心转存（只做了简单的oj系统，用不上，但普遍会设置来保证安全性，所以做来展示）
            //原理是，linux系统默认在程序崩溃时会生成一个核心转储文件，包含程序崩溃时的内存状态等信息，可能泄露敏感数据
            //通过设置RLIMIT_CORE资源限制为0，可以禁止生成核心转储文件，增强安全性
            // struct rlimit core_rlimit;
            // core_rlimit.rlim_max = 0; // 禁止生成核心转储文件
            // core_rlimit.rlim_cur = 0;
            // if (setrlimit(RLIMIT_CORE, &core_rlimit) < 0) {
            //     LOG(ERROR) << "设置核心转储资源限制失败" << "\n";
            // }
        }

        // 基本原理：通过获取前端传来的文件，运用fork运行，主线程等待即可
        static int RunCode(const std::string &file_name, int cpu_limit = 1,int mem_limit = 10*1024*1024,int time_limit = 5,int fd_limit = 256)
        {
            //先拼接路径
            std::string _exe = PathUtil::Exe(file_name);//可执行文件路径
            std::string _stdin = PathUtil::Stdin(file_name);//标准输入文件路径
            std::string _stdout = PathUtil::Stdout(file_name);//标准输出文件路径
            std::string _stderr = PathUtil::Stderr(file_name);//标准错误文件路径

            //取消umask限制，确保文件权限按设置生效
            umask(0);
            //打开或创建对应的重定向文件：O_CREAT为不存在则创建，O_RDONLY/O_WRONLY为读/写模式，0644为文件权限
            int _stdin_fd = open(_stdin.c_str(), O_CREAT|O_RDONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT|O_WRONLY, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT|O_WRONLY, 0644);

            pid_t pid = fork();
            if(pid < 0)
            {
                LOG(ERROR) << "子进程创建失败 " << "\n";
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2; //代表创建子进程失败
            }
            else if(pid == 0) //子进程逻辑
            {
                //准备工作
                //重定向标准输入/输出/错误到对应的文件
                //由于获取了对应的文件描述符，我们对应的文件就能同样接收到输入输出等信息了
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);    
                //为子进程设置资源限制
                ResourceLimit(cpu_limit, mem_limit, time_limit, fd_limit);

                //正式开始执行目标程序
                //execl的原理：在当前进程空间加载并执行指定的程序
                //第一个参数：我要执行谁
                //第二个参数：在命令行上如何执行该程序（通常与第一个参数相同）
                //第三个参数：以nullptr结尾，表示参数列表结束
                execl(_exe.c_str(), _exe.c_str(), nullptr);
                exit(1); //如果execl返回了，说明执行失败了，退出码为1
            }
            else //父进程逻辑
            {
                //首先父进程无需持有子进程的重定向文件句柄，立即关闭避免泄漏
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int status = 0;
                //等待子进程结束并获取状态
                waitpid(pid, &status, 0);
                //WIFEXITED(status)宏用于检查子进程是否正常退出，如果是，则可以使用WEXITSTATUS(status)获取子进程的退出状态码
                if(WIFEXITED(status))
                {
                    // 子进程正常退出   
                    int exit_code = WEXITSTATUS(status);
                    if(exit_code == 0)
                    {
                        //代码跑完结果正确
                        LOG(INFO) << "代码运行成功，退出码为0" << "\n";
                        return 0;
                    }
                    else
                    {
                        //代码跑完结果错误
                        LOG(INFO) << "代码运行失败，退出码为: " << exit_code << "\n";
                        return exit_code; //代表代码运行错误，返回对应的错误码
                    }
                }
                // 如果程序运行出现了异常，一定是因为收到了信号
                else if(WIFSIGNALED(status))
                {
                    //WTERMSIG(status)宏用于获取导致子进程异常终止的信号编号(只有在子进程崩溃的情况下才有意义)
                    int sig = WTERMSIG(status);
                    LOG(INFO) << "代码运行失败，收到了信号: " << sig << "\n";
                    return sig; //代表代码运行错误，返回对应的错误码
                }
                else
                {
                    //其他异常情况
                    LOG(INFO) << "代码运行失败，未知异常\n";
                    return SIGKILL; //代表代码运行错误
                }
            }
        }
    };

}













