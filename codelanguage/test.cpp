#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

// 传值 - 创建副本
void modifyByValue(vector<int> vec) {
    if (!vec.empty()) {
        vec[0] = 999; // 修改只影响副本
    }
}

// 传引用 - 直接操作原对象
void modifyByReference(vector<int>& vec) {
    if (!vec.empty()) {
        vec[0] = 999; // 修改原vector
    }
}

// 传指针 - 通过地址操作原对象
void modifyByPointer(vector<int>* vec) {
    if (vec && !vec->empty()) {
        (*vec)[0] = 999; // 修改原vector
    }
}

// 常量引用 - 只读访问，避免拷贝
void printVector(const vector<int>& vec) {
    for (const auto& item : vec) {
        cout << item << " ";
    }
    cout << endl;
}

int main() {
    vector<int> originalVec = {1, 2, 3, 4, 5};
    
    cout << "原始vector: ";
    printVector(originalVec);
    
    // 测试传值
    vector<int> valueVec = originalVec;
    modifyByValue(valueVec);
    cout << "传值后的vector: ";
    printVector(valueVec);
    
    // 测试传引用
    vector<int> refVec = originalVec;
    modifyByReference(refVec);
    cout << "传引用后的vector: ";
    printVector(refVec);
    
    // 测试传指针
    vector<int> pointerVec = originalVec;
    modifyByPointer(&pointerVec);
    cout << "传指针后的vector: ";
    printVector(pointerVec);
    
    // 性能测试
    const int SIZE = 1000000;
    vector<int> largeVec(SIZE, 1);
    
    auto start = high_resolution_clock::now();
    modifyByValue(largeVec); // 传值会导致拷贝整个vector
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "传值操作耗时: " << duration.count() << " 微秒" << endl;
    
    start = high_resolution_clock::now();
    modifyByReference(largeVec); // 传引用无拷贝
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "传引用操作耗时: " << duration.count() << " 微秒" << endl;
    
    return 0;
}