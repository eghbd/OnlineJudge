#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
using namespace std;
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1() {
    vector<int> nums = {2,7,11,15};
    int target = 9;
    vector<int> res = Solution().twoSum(nums, target);
    if (res.size() == 2 && res[0] == 0 && res[1] == 1) {
        cout << "通过用例1, 测试[2,7,11,15]和9通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例1, 测试的值是: [2,7,11,15] 和 9" << endl;
    }
}

void Test2() {
    vector<int> nums = {3,2,4};
    int target = 6;
    vector<int> res = Solution().twoSum(nums, target);
    if (res.size() == 2 && res[0] == 1 && res[1] == 2) {
        cout << "通过用例2, 测试[3,2,4]和6通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例2, 测试的值是: [3,2,4] 和 6" << endl;
    }
}

void Test3() {
    vector<int> nums = {3,3};
    int target = 6;
    vector<int> res = Solution().twoSum(nums, target);
    if (res.size() == 2 && res[0] == 0 && res[1] == 1) {
        cout << "通过用例3, 测试[3,3]和6通过 ... OK!" << endl;
    } else {
        cout << "没有通过用例3, 测试的值是: [3,3] 和 6" << endl;
    }
}

int main() {
    Test1();
    Test2();
    Test3();

    return 0;
}