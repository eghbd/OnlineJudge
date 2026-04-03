
void Test1() {
    int x = 123;
    int res = Solution().reverse(x);
    if (res == 321) {
        cout << "通过用例1, 测试123通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例1, 测试的值是: 123" << endl;
    }
}

void Test2() {
    int x = -123;
    int res = Solution().reverse(x);
    if (res == -321) {
        cout << "通过用例2, 测试-123通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例2, 测试的值是: -123" << endl;
    }
}

void Test3() {
    int x = 120;
    int res = Solution().reverse(x);
    if (res == 21) {
        cout << "通过用例3, 测试120通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例3, 测试的值是: 120" << endl;
    }
}

void Test4() {
    int x = 0;
    int res = Solution().reverse(x);
    if (res == 0) {
        cout << "通过用例4, 测试0通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例4, 测试的值是: 0" << endl;
    }
}

// 测试溢出场景
void Test5() {
    int x = 1534236469;
    int res = Solution().reverse(x);
    if (res == 0) {
        cout << "通过用例5, 测试1534236469（溢出）通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例5, 测试的值是: 1534236469" << endl;
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