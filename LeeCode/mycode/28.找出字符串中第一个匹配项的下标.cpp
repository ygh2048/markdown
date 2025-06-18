/*
 * @lc app=leetcode.cn id=28 lang=cpp
 *
 * [28] 找出字符串中第一个匹配项的下标
 */

 // a a a b a b a a b
 // 1 2 3 0 1 0 1 2 0
#include <vector>
#include <string>
using namespace std;
// @lc code=start
class Solution {
public:
    int strStr(string haystack, string needle) {
        int n = haystack.size(), m = needle.size();
        if (m == 0) {
            return 0;
        }
        vector<int> pi(m);//记录前缀函数值，只记录m个
        for (int i = 1, j = 0; i < m; i++) {
            while (j > 0 && needle[i] != needle[j]) {//上一位有匹配且这一位不匹配，回退
                j = pi[j - 1];//关键优化,回退到第j-1
                j++;//在已经匹配的前缀和后缀的基础上，如果新的一位不匹配，回退到上一个匹配点
            }
            pi[i] = j;
        }
        for (int i = 0, j = 0; i < n; i++) {
            while (j > 0 && haystack[i] != needle[j]) {
                j = pi[j - 1];
            }
            if (haystack[i] == needle[j]) {
                j++;
            }
            if (j == m) {
                return i - m + 1;
            }
        }
        return -1;
    }
};
// @lc code=end

