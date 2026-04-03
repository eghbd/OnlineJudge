//测试运行功能的文件
//测试运行功能的文件
//测试运行功能的文件
//测试运行功能的文件
//测试运行功能的文件

//g++ -o run_test Run_test.cpp -std=c++11
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

#include "../compile_run_server/run.hpp"
#include "../common/util.hpp"
#include "../common/log.hpp"
using namespace std;
using namespace oj_run;
using namespace oj_util;
using namespace oj_log;

// 1. 正确代码：运行成功 return 0
void CorrectCode(const string &file_name)
{
    string cpp = "./temp/" + file_name + ".cpp";
    ofstream f(cpp);
    f << R"(
#include <iostream>
using namespace std;
int main() 
{
    cout << "正常运行：Hello OJ Sandbox" << endl;
    return 0;
}
)";
    f.close();

    string cmd = "g++ " + cpp + " -o " + PathUtil::Exe(file_name);
    system(cmd.c_str());
}

// 2. 错误代码：运行结束 return 1
void ErrorCode(const string &file_name)
{
    string cpp = "./temp/" + file_name + ".cpp";
    ofstream f(cpp);
    f << R"(
#include <iostream>
using namespace std;
int main() 
{
    cout << "运行结束，但返回错误码" << endl;
    return 1;
}
)";
    f.close();

    string cmd = "g++ " + cpp + " -o " + PathUtil::Exe(file_name);
    system(cmd.c_str());
}

// 3. 死循环代码
void TimeoutCode(const string &file_name)
{
    string cpp = "./temp/" + file_name + ".cpp";
    ofstream f(cpp);
    f << R"(
int main() 
{
    while(1); // 死循环 → 超时
    return 0;
}
)";
    f.close();

    string cmd = "g++ " + cpp + " -o " + PathUtil::Exe(file_name);
    system(cmd.c_str());
}

// 4. 内存超限代码
void MemLimitCode(const string &file_name)
{
    string cpp = "./temp/" + file_name + ".cpp";
    ofstream f(cpp);
    f << R"(
int main() {
    int a[1024 * 1024 * 80]; // 超过 64MB 限制
    return 0;
}
)";
    f.close();

    string cmd = "g++ " + cpp + " -o " + PathUtil::Exe(file_name);
    system(cmd.c_str());
}

// 5. 崩溃代码（空指针）
void CrashCode(const string &file_name)
{
    string cpp = "./temp/" + file_name + ".cpp";
    ofstream f(cpp);
    f << R"(
int main() {
    int *p = nullptr;
    *p = 10; // 崩溃
    return 0;
}
)";
    f.close();

    string cmd = "g++ " + cpp + " -o " + PathUtil::Exe(file_name);
    system(cmd.c_str());
}

// 清理文件
void Clean(const string &file_name)
{
    remove(("./temp/" + file_name + ".cpp").c_str());
    remove(PathUtil::Exe(file_name).c_str());
    remove(PathUtil::Stdin(file_name).c_str());
    remove(PathUtil::Stdout(file_name).c_str());
    remove(PathUtil::Stderr(file_name).c_str());
}

// 统一运行 + 打印结果
void RunTest(const string &name, const string &test_file, void (*GenFunc)(const string&), int cpu, int mem, int time, int fd)
{
    cout << "\n========================================" << endl;
    cout << "测试：" << name << endl;
    Clean(test_file);
    GenFunc(test_file);
    int ret = Run::RunCode(test_file, cpu, mem, time, fd);
    cout << "RunCode 返回值：" << ret << endl;

    if (ret == 0)
        cout << "结果：运行成功" << endl;
    else if (ret > 0 && ret < 128)
        cout << "结果：运行结束，退出码 = " << ret << endl;
    else
        cout << "结果：被信号杀死，信号 = " << ret << endl;
}

int main()
{
    const string test_file = "test_code";

    // 测试 1：正常运行
    RunTest("正常代码", test_file, CorrectCode, 1, 64, 5, 64);

    // 测试 2：运行失败 return 1
    RunTest("运行失败", test_file, ErrorCode, 1, 64, 5, 64);

    // 测试 3：死循环,超时
    RunTest("超时死循环", test_file, TimeoutCode, 1, 64, 1, 64);

    // 测试 4：内存超限
    RunTest("内存超限", test_file, MemLimitCode, 1, 64, 5, 64);

    // 测试 5：崩溃代码
    RunTest("空指针崩溃", test_file, CrashCode, 1, 64, 5, 64);

    Clean(test_file);
    return 0;
}