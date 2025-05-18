/*
 * @lc app=leetcode.cn id=15 lang=cpp
 *
 * [15] 三数之和
 */
#include <execution>
#include<vector>
using namespace std;
// @lc code=start
class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        auto p1 = nums.begin() ,p2 = nums.begin() + 1,p3 = nums.end()-1;
        int sum = 0;
        vector<vector<int>> total_answer;
        sort(nums.begin(),nums.end());
        while(p1 != nums.end() - 1)
        {
            
            vector<int> answer;
            int sum = *p1 + *p2 + *p3;
            if(sum == 0 && p2 != p1)
            {
                p2 += 1;
                p3 -= 1;
                answer = {*p1,*p2,*p3};
                total_answer.push_back(answer);
            }
            else if(sum > 0)
            {
                p3 -= 1;
            }
            else
            {
                p2 += 1;
            }

            if(p2 == p3)
            {
                p1++;
                p2 = p1+1;
                p3 = nums.end();
                continue;
            }
        }
        return total_answer;
        
    }
};
// @lc code=end

