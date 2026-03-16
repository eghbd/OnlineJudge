
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

#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace ns_runner
{
    using namespace ns_util;
    using namespace ns_log;

    class Runner
    {
    public:
        Runner() {}
        ~Runner() {}

    public:
        //设置相对于资源限制的接口
        //_cpu_limit:可以使用的最大cpu资源上限
        //_mem_limit:可以使用的最大的内存大小
        static void SetProcLimit(int _cpu_limit, int _mem_limit)
        {
            //设置CPU时长
            //设置为RLIMIT_CPU资源限制，超过该时间进程会收到SIGXCPU信号终止
            struct rlimit cpu_rlimit;
            //RLIM_INFINITY的意思是没有限制
            cpu_rlimit.rlim_max = RLIM_INFINITY;
            cpu_rlimit.rlim_cur = _cpu_limit;
            setrlimit(RLIMIT_CPU, &cpu_rlimit);

            //设置内存大小
            struct rlimit mem_rlimit;
            mem_rlimit.rlim_max = RLIM_INFINITY;
            mem_rlimit.rlim_cur = _mem_limit * 1024; //转化成为KB
            setrlimit(RLIMIT_AS, &mem_rlimit);
        }
        //对于我们的需要运行的代码无非就几种结果：1.代码跑完结果正确 2.代码跑完结果不正确 3.代码没跑完异常了
        //这个接口要做的是运行代码，并且返回代码的运行状态，即代码能不能跑，我们不关心代码是否准确，这是测试用例要做的
        static int Run(const std::string &file_name, int cpu_limit, int mem_limit)
        {
            //拼接可执行的程序(_execute)、标准输入(_stdin)/输出(_stdout)/错误文件(_stderr)的完整路径
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin   = PathUtil::Stdin(file_name);
            std::string _stdout  = PathUtil::Stdout(file_name);
            std::string _stderr  = PathUtil::Stderr(file_name);

            //取消umask限制，确保文件权限按设置生效
            umask(0);
            //打开或创建对应的重定向文件：O_CREAT为不存在则创建，O_RDONLY/O_WRONLY为读/写模式，0644为文件权限
            int _stdin_fd = open(_stdin.c_str(), O_CREAT|O_RDONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT|O_WRONLY, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT|O_WRONLY, 0644);

            if(_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0){
                LOG(ERROR) << "运行时打开标准文件失败" << "\n";
                return -1; //代表打开文件失败
            }            
            //核心！！！创建子进程，子进程负责执行代码，父进程等待子进程结束后获取状态
            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "运行时创建子进程失败" << "\n";
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2; //代表创建子进程失败
            }
            //子进程：目标是运行代码
            else if (pid == 0)
            {
                //dup2是Unix/Linux系统下的一个系统调用，核心作用是将一个文件描述符复制到另一个指定的文件描述符
                //由于获取了对应的文件描述符，我们对应的文件就能同样接收到输入输出等信息了
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);
                // 为子进程设置CPU/内存资源限制
                SetProcLimit(cpu_limit, mem_limit);
                //执行目标程序execl
                //第一个参数：我要执行谁 第二个参数：在命令行上如何执行该程序（通常与第一个参数相同）第三个参数：以nullptr结尾，表示参数列表结束
                execl(_execute.c_str(), _execute.c_str(), nullptr);
                exit(1);
            }
            //父进程：目标是接收子进程的状态
            // else
            // {
            //     //父进程无需持有子进程的重定向文件句柄，立即关闭避免泄漏
            //     close(_stdin_fd);
            //     close(_stdout_fd);
            //     close(_stderr_fd);
            //     int status = 0;
            //     //等待基础完成编译工作
            //     waitpid(pid, &status, 0);
            //     // 如果程序运行出现了异常，一定是因为收到了信号
            //     // 解析子进程终止信号：status低7位（0x7F）存储终止信号
            //     LOG(INFO) << "运行完毕, info: " << (status & 0x7F) << "\n"; 
            //     return status & 0x7F;
            // }
            else // 父进程逻辑
            {
                // 关闭重定向文件句柄（父进程无需持有）
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);

                // 5. 带超时的子进程等待逻辑（超时时间：5秒，可配置）
                int status = 0;
                pid_t ret_pid;
                const int TIMEOUT_SEC = 5; // 超时时间配置
                time_t start = time(nullptr);

                // 非阻塞轮询等待子进程结束
                while ((ret_pid = waitpid(pid, &status, WNOHANG)) == 0) {
                    // 检查是否超时
                    if (time(nullptr) - start > TIMEOUT_SEC) {
                        LOG(INFO) << "程序运行超时(" << TIMEOUT_SEC << "秒)，强制终止进程" << "\n";
                        kill(pid, SIGKILL); // 强制杀死子进程
                        break;
                    }
                    usleep(10000); // 10ms轮询一次，降低CPU占用
                }

                // 6. 解析子进程退出状态
                if (WIFEXITED(status)) { // 正常退出（非信号终止）
                    int exit_code = WEXITSTATUS(status);
                    if (exit_code == 255) { // execl执行失败
                        LOG(ERROR) << "目标程序执行失败，退出码：" << exit_code << "\n";
                        return -3;
                    } else { // 正常运行完毕
                        LOG(INFO) << "程序正常运行完毕，退出码：" << exit_code << "\n";
                        return 0;
                    }
                } else if (WIFSIGNALED(status)) { // 信号终止
                    int sig_num = WTERMSIG(status);
                    LOG(INFO) << "程序异常终止，收到信号：" << sig_num << "\n";
                    return sig_num;
                } else { // 其他情况（如超时杀死）
                    LOG(INFO) << "程序被强制终止，超时/手动杀死" << "\n";
                    return SIGKILL; // 返回9（SIGKILL信号）
                }
            }
        }
    };
}