//用于日志的拼接接口

#pragma once

#include <iostream>
#include <string>
#include "util.hpp"

namespace ns_log
{
    using namespace ns_util;

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
    //我们需要形成的形式为：[等级][文件名][行号][时间戳]
    inline std::ostream &Log(const std::string &level, const std::string &file_name, int line)
    {
        //添加日志等级
        std::string message = "[";
        message += level;
        message += "]";

        //添加报错文件名称
        message += "[";
        message += file_name;
        message += "]";

        //添加报错行
        message += "[";
        message += std::to_string(line);
        message += "]";

        //添加日志的时间戳
        message += "[";
        message += TimeUtil::GetTimeStamp();
        message += "]";

        // cout的本质：内部是包含缓冲区的
        std::cout << message; //不要endl进行刷新

        return std::cout;
    }
    // LOG(INFo) << "message" << "\n";
    // 开放式日志
    #define LOG(level) Log(#level, __FILE__, __LINE__)
}