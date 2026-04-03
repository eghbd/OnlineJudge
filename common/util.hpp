//路径工具类
//路径工具类
//路径工具类
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <boost/algorithm/string.hpp>
//工具类，提供一些公共的接口
namespace oj_util
{
    //时间工具类
    class TimeUtil
    {
    public:
        //获得时间戳
        static std::string GetTimeStamp()
        {
            struct timeval _time;
            //运用linux下的系统调用获取时间戳
            gettimeofday(&_time, nullptr);
            return std::to_string(_time.tv_sec);
        }
        //获取毫秒级时间戳
        static std::string GetMsTimeStamp()
        {
            struct timeval _time;
            //运用linux下的系统调用获取时间戳
            gettimeofday(&_time,nullptr);
            return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
        }
    };
    //用于路径拼接的字符串
    const std::string temp_path = "./temp/";
    // 路径工具类,提供一些路径拼接的接口
    class PathUtil
    {
        
        // 我们需要的路径有
        // 1.源代码文件路径: ./temp/xxx.cpp
        // 2.可执行程序路径: ./temp/xxx.exe
        // 3.编译错误文件路径: ./temp/xxx.compile_error
        // 4.输入文件路径: ./temp/xxx.stdin
        // 5.输出文件路径: ./temp/xxx.stdout
        // 6.运行时错误文件路径: ./temp/xxx.stderr
    public:
        //负责拼接的函数
        // file_name: 文件名，suffix: 后缀
        static std::string Splicing(const std::string &file_name,const std::string &Suffix)
        {
            std::string path_name = temp_path;
            path_name += file_name;
            path_name += Suffix;
            return path_name;
        }
        //.cpp用于源代码文件的路径 
        static std::string Src(const std::string &file_name)
        {
            return Splicing(file_name, ".cpp");
        }
        //.exe用于启动文件的路径
        static std::string Exe(const std::string &file_name)
        {
            return Splicing(file_name, ".exe");
        }
        //.compile_error用于编译阶段错误的文件的路径
        //用于能否编译
        static std::string CompilerError(const std::string &file_name)
        {
            return Splicing(file_name, ".compiler_error");
        }
        //.stdin用于输入文件的路径
        static std::string Stdin(const std::string &file_name)
        {
            return Splicing(file_name, ".stdin");
        }
        //.stdout用于输出文件的路径
        static std::string Stdout(const std::string &file_name)
        {            
            return Splicing(file_name, ".stdout");
        }
        //.stderr用于运行时错误文件的路径
        //用于是否能运行
        static std::string Stderr(const std::string &file_name)
        {            
            return Splicing(file_name, ".stderr");
        }
    };
    //文件工具类，提供一些文件相关的接口
    class FileUtil
    {
    public:
        
        // 路径+输出的字符串+是否保存换行
        static bool ReadFile(const std::string &path, std::string *out, bool keep = false)
        {
            (*out).clear();
            //创建文件输入流
            std::ifstream in(path);
            if(!in.is_open())
            {
                return false;
            }
            std::string line;
            //每次读一给line
            while(std::getline(in, line))
            {
                (*out) += line;
                (*out) += (keep ? "\n" : "");
            }
            in.close();
            return true;
        }
        static bool FileIsExist(const std::string &path_name)
        {
            //通过stat系统调用来判断文件是否存在
            struct stat st;
            if(stat(path_name.c_str(), &st) == 0)
            {
                //获取属性成功，文件已经存在
                return true;
            }
            return false;
        }
        //返回唯一文件名
        static std::string UniqFileName()
        {
            static std::atomic_uint id(0);
            id++;
            std::string _id = std::to_string(id);
            std::string ms = TimeUtil::GetMsTimeStamp();
            return _id + "_" + ms;
        }
        //把代码写入文件：文件路径+内容
        static bool WriteIntoFile(const std::string &path, const std::string &code)
        {
            std::ofstream out(path);
            if(!out.is_open())
            {
                return false;
            }
            out.write(code.c_str(), code.size());
            out.close();
            return true;
        }

    };
    class StringUtil
    {
    public:
        static void SplitString(const std::string &str, std::vector<std::string> *target, const std::string sep)
        {
        //     boost::split(
        //     目标容器,    // 一般是 vector<string>
        //     原字符串,
        //     分隔符规则,  // 必须用 boost::is_any_of(分隔符)
        //     压缩模式     // token_compress_on / off
        // );
            boost::split((*target), str, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
    static void StringReplace(std::string* str, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = (*str).find(from, pos)) != std::string::npos) {
            (*str).replace(pos, from.length(), to);
            pos += to.length();
        }
    }
}