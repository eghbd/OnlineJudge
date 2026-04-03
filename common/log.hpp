//用于日志的拼接接口

#pragma once
#include <iostream>
#include <string>
#include "util.hpp"

namespace oj_log
{
    using namespace oj_util;
    //定义日志的等级
    //默认为0，依次递增
    enum
    {
        INFO, //就是整数    
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };
    //通过调用输出流ostream将数据输出到指定位置
    //形成的形式为：[等级][文件名][行号][时间戳]
    inline std::ostream &Log(const std::string &level,const std::string name,int line)
    {
        std::string message = "";
        //添加日志等级
        message += "[";
        message += level;
        message += "]";

        //添加报错文件名称
        message += "[";
        message += name;
        message += "]";

        //添加报错行
        message += "[";
        message += std::to_string(line);
        message += "]";

        //添加日志的时间戳
        message += "[";
        message += TimeUtil::GetTimeStamp();
        message += "]";
        // ostream的特性，向cout写入数据但不刷新缓冲区，最后返回cout对象
        // cout的本质：内部是包含缓冲区的
        std::cout << message; //不要endl进行刷新
        return std::cout;
    }
    #define LOG(level) Log(#level,__FILE__,__LINE__) << std::endl
}