// 测试编译功能的文件
// 测试编译功能的文件
// 测试编译功能的文件
// 测试编译功能的文件
// 测试编译功能的文件
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../compile_run_server/compile.hpp"
#include "../common/util.hpp"
#include "../common/log.hpp"

// 通过系统调用ofstream来创建临时的测试代码文件,并写入测试代码
bool SourceFile(const std::string &file_name, const std::string &code)
{
    //通过ofstream来创建/打开文件并准备写入数据
    std::ofstream ofs(oj_util::PathUtil::Src(file_name));
    if (!ofs.is_open())
    {
        std::cerr << "创建测试源文件失败！" << std::endl;
        return false;
    }
    // 将测试代码写入文件
    ofs << code;
    ofs.close();
    return true;
}

// 测试工具函数：清理测试生成的文件，不用就删除即可
void CleanFiles(const std::string &file_name)
{
    // 删除源文件
    remove(oj_util::PathUtil::Src(file_name).c_str());
    // 删除可执行文件
    remove(oj_util::PathUtil::Exe(file_name).c_str());
    // 删除编译错误日志
    remove(oj_util::PathUtil::CompilerError(file_name).c_str());
}

// 测试用例主函数
int main()
{
    // 测试用例1：编译正确的代码
    std::cout << "测试1（正确代码）" << std::endl;
    std::string test1_name = "test_correct";
    std::string correct_code = R"(
#include <iostream>
int main() {
    std::cout << "Hello, OJ!" << std::endl;
    return 0;
}
)";
    if (SourceFile(test1_name, correct_code))
    {
        //调用对应的编译接口进行编译
        bool ret = oj_compiler::Compiler::Compile(test1_name);
        if (ret)
        {
            std::cout << "测试用例1通过：正确代码编译成功！\n" << std::endl;
        }
        else
        {
            std::cout << "测试用例1失败：正确代码编译失败！\n" << std::endl;
        }
    }
    CleanFiles(test1_name);
    std::cout << "========================================" << std::endl;
    std::cout << "测试2（错误代码）" << std::endl;
    // 测试用例2：编译错误的代码（语法错误）
    std::string test2_name = "test_error";
    std::string error_code = R"(
#include <iostream>
int main() {
    // 故意写语法错误：缺少分号
    std::cout << "Hello, OJ!"
    return 0;
}
)";
    if (SourceFile(test2_name, error_code))
    {
        bool ret = oj_compiler::Compiler::Compile(test2_name);
        if (!ret)
        {
            std::cout << "测试用例2：错误代码编译失败！\n" << std::endl;
        }
        else
        {
            std::cout << "测试用例2：错误代码编译成功！\n" << std::endl;
        }
    }
    CleanFiles(test2_name);

    return 0;
}