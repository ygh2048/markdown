/*
 * @lc app=leetcode.cn id=2 lang=cpp
 *
 * [2] 两数相加
 */

// @lc code=start
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {

        ListNode* dummy = new ListNode(0); // 创建一个虚拟头节点
        ListNode* current = dummy; // 当前节点指向虚拟头节点
        int carry = 0; // 进位

        while (l1 != nullptr || l2 != nullptr || carry != 0) {
            int sum = carry; // 初始化和为进位

            if (l1 != nullptr) {
                sum += l1->val; // 加上l1的值
                l1 = l1->next; // 移动到下一个节点
            }

            if (l2 != nullptr) {
                sum += l2->val; // 加上l2的值
                l2 = l2->next; // 移动到下一个节点
            }

            carry = sum / 10; // 更新进位
            current->next = new ListNode(sum % 10); // 创建新节点并连接到结果链表
            current = current->next; // 移动到新创建的节点
        }

        return dummy->next; // 返回结果链表，去掉虚拟头节点

    }
};
// @lc code=end

