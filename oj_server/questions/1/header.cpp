#include <iostream>
#include <string>
#include <stack>
#include <unordered_map>

using namespace std;

// 写好的代码，用于测试运行返回功能
// 写好的代码，用于测试运行返回功能
// 写好的代码，用于测试运行返回功能
class Solution {
public:
    bool isValid(string s) {
        // 定义右括号到左括号的映射表，方便快速匹配
        unordered_map<char, char> bracketMap;
        bracketMap[')'] = '(';
        bracketMap['}'] = '{';
        bracketMap[']'] = '[';
        stack<char> stk; // 栈用于存储左括号

        // 遍历字符串中的每个字符
        for (char c : s) {
            // 如果是右括号（存在于map的key中）
            if (bracketMap.find(c) != bracketMap.end()) {
                // 栈为空 或 栈顶不是对应的左括号 → 无效
                if (stk.empty() || stk.top() != bracketMap[c]) {
                    return false;
                }
                // 匹配成功，弹出栈顶的左括号
                stk.pop();
            }
            else {
                // 是左括号，压入栈中
                stk.push(c);
            }
        }

        // 遍历结束后，栈必须为空（所有左括号都匹配完成）
        return stk.empty();
    }
};