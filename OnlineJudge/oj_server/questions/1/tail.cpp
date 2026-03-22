#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1() {
    string s = "()";
    bool res = Solution().isValid(s);
    if (res == true) {
        cout << "通过用例1, 测试()通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例1, 测试的值是: ()" << endl;
    }
}

void Test2() {
    string s = "()[]{}";
    bool res = Solution().isValid(s);
    if (res == true) {
        cout << "通过用例2, 测试()[]{}通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例2, 测试的值是: ()[]{}" << endl;
    }
}

void Test3() {
    string s = "(]";
    bool res = Solution().isValid(s);
    if (res == false) {
        cout << "通过用例3, 测试(]通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例3, 测试的值是: (]" << endl;
    }
}

void Test4() {
    string s = "([)]";
    bool res = Solution().isValid(s);
    if (res == false) {
        cout << "通过用例4, 测试([)]通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例4, 测试的值是: ([)]" << endl;
    }
}

void Test5() {
    string s = "{[]}";
    bool res = Solution().isValid(s);
    if (res == true) {
        cout << "通过用例5, 测试{[]}通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例5, 测试的值是: {[]}" << endl;
    }
}

// 边界测试：空字符串（根据题意不会出现，但补充测试）
void Test6() {
    string s = "";
    bool res = Solution().isValid(s);
    if (res == true) {
        cout << "通过用例6, 测试空字符串通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例6, 测试的值是: 空字符串" << endl;
    }
}

int main() {
    Test1();
    Test2();
    Test3();
    Test4();
    Test5();
    Test6();

    return 0;
}