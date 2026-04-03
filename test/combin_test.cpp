//用于测试拼接功能的文件

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

// 读取文件内容
string readFile(const string &path) {
    ifstream ifs(path);
    return string((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
}

// 写入文件
void writeFile(const string &path, const string &content) {
    ofstream ofs(path);
    ofs << content;
}

int main() {
    // ===================== 固定路径 =====================
    string header = "./1/header.cpp";
    string tail   = "./1/tail.cpp";
    string merged = "./1/merged.cpp";        // 拼接后的文件
    string output = "./1/merged_test";       // 编译后的可执行文件

    // 1. 读取两段代码
    string h_code = readFile(header);
    string t_code = readFile(tail);

    // 2. 拼接
    string all = h_code + "\n// -------- 拼接分割线 --------\n" + t_code;

    // 3. 输出【拼接后完整代码】（你要的展示）
    cout << "\n==================== 拼接后的完整代码 ====================\n\n";
    cout << all << endl;
    cout << "\n==========================================================\n\n";

    // 4. 写入合并后的 cpp
    writeFile(merged, all);

    // 5. 用你原版的 execlp 编译！
    cout << "开始编译：g++ -o " << output << " " << merged << " -DCOMPILER_ONLINE -std=c++11\n\n";

    pid_t pid = fork();
    if (pid == 0) {
        // ↓↓↓ 完全是你原版那行 execlp，一字不改 ↓↓↓
        execlp("g++", "g++",
               "-o", output.c_str(),
               merged.c_str(),
               "-D", "COMPILER_ONLINE",
               "-std=c++11",
               nullptr);

        perror("编译失败");
        return 1;
    }

    wait(NULL);
    cout << "\n编译完成！生成可执行文件：" << output << endl;

    return 0;
}