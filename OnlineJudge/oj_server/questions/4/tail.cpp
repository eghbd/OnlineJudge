#include <iostream>
#include <string>
#include <map>
using namespace std;
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1() {
    string s = "III";
    int res = Solution().romanToInt(s);
    if (res == 3) {
        cout << "通过用例1, 测试III通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例1, 测试的值是: III" << endl;
    }
}

void Test2() {
    string s = "IV";
    int res = Solution().romanToInt(s);
    if (res == 4) {
        cout << "通过用例2, 测试IV通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例2, 测试的值是: IV" << endl;
    }
}

void Test3() {
    string s = "IX";
    int res = Solution().romanToInt(s);
    if (res == 9) {
        cout << "通过用例3, 测试IX通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例3, 测试的值是: IX" << endl;
    }
}

void Test4() {
    string s = "LVIII";
    int res = Solution().romanToInt(s);
    if (res == 58) {
        cout << "通过用例4, 测试LVIII通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例4, 测试的值是: LVIII" << endl;
    }
}

void Test5() {
    string s = "MCMXCIV";
    int res = Solution().romanToInt(s);
    if (res == 1994) {
        cout << "通过用例5, 测试MCMXCIV通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例5, 测试的值是: MCMXCIV" << endl;
    }
}

int main() {
    Test1();
    Test2();
    Test3();
    Test4();
    Test5();

    return 0;
}