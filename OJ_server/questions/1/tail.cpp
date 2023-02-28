#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    bool ret = Solution().isPalindrome(121);
    if (ret)
    {
        std::cout << "OK , 通过用例1, 测试值: 121 " << std::endl;
    }
    else
    {
        std::cout << "NO , 未通过用例1 , 测试值: 121" << std::endl;
    }
}

void Test2()
{
    bool ret = Solution().isPalindrome(123);
    if (!ret)
    {
        std::cout << "OK , 通过用例2, 测试值: 123 " << std::endl;
    }
    else
    {
        std::cout << "NO , 未通过用例2 , 测试值: 123" << std::endl;
    }
}

int main()
{
    Test1();
    Test2();
    return 0;
}