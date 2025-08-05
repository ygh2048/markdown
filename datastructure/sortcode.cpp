#include <iostream>
#include <vector> // 使用vector容器存储待排序数据
using namespace std;
// 冒泡排序函数  本函数有提前结束
void bubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    // 外层循环：控制排序轮数（每轮确定一个最大值）
    for (int i = 0; i < n - 1; ++i) {
        // 优化标志：若本轮未发生交换，说明已有序
        bool swapped = false;
        // 内层循环：比较相邻元素（每轮缩小比较范围） 每次循环会把最大值放在最外面
        for (int j = 0; j < n - i - 1; ++j) {
            // 如果前一个元素大于后一个元素，则交换
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                swapped = true; // 标记发生交换
            }
        }
        // 如果本轮未交换，提前结束排序
        if (!swapped) break;
    }
}

void mybubblesort(vector<int>& arr)
{
    for(int i= 0; i <arr.size();i++)
    {
        for(int j = 0;j<arr.size() - i -1;j++)
        {
            if(arr[j] > arr[j+1])
            swap(arr[j],arr[j+1]);
        }
    }

}

void selectionSort(vector<int>& arr) {
    int n = arr.size();
    
    // 外层循环：控制已排序部分的边界（从0到n-2）
    for (int i = 0; i < n - 1; ++i) {
        // 初始化最小元素索引为当前起始位置
        int minIndex = i;
        
        // 内层循环：在未排序部分[i+1, n-1]中寻找最小元素
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;  // 更新最小元素索引
            }
        }
        
        // 将找到的最小元素交换到已排序部分的末尾
        swap(arr[i], arr[minIndex]);
    }
}

void myselectsort(vector<int>& arr){
    for(int i = 0 ; i < arr.size();i++)
    {
        int minindex = i;
        for(int j = i ; j < arr.size();j++)
        {
            if(arr[minindex] > arr[j])
            {
                minindex = j;
            }
        }
        swap(arr[minindex],arr[i]);
    }
}

// 单函数快速排序
void quickSort(vector<int>& arr, int low, int high) {
    if (low >= high) return;  // 递归终止条件
    
    // 1. 随机选择基准值并交换到末尾
    int pivotIndex = low + rand() % (high - low + 1);
    swap(arr[pivotIndex], arr[high]);
    int pivot = arr[high];
    
    // 2. 分区操作（双指针法）
    int i = low;  // 指向小于基准的最后一个元素
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            swap(arr[i], arr[j]);
            i++;
        }
    }
    swap(arr[i], arr[high]);  // 将基准放到正确位置
    
    // 3. 递归排序子数组
    quickSort(arr, low, i - 1);  // 左半部分
    quickSort(arr, i + 1, high); // 右半部分
}

// 用户友好接口（单参数版本）
void quickSort(vector<int>& arr) {
    if (arr.size() <= 1) return;  // 处理空数组或单元素
    
    // 初始化随机种子
    srand(time(nullptr));
    
    // 调用完整版快速排序
    quickSort(arr, 0, arr.size() - 1);
}

void myquicksort(vector<int>& arr,int low,int high)//
{
    if(low >= high)
    {
        return;
    } 
    int i = low;

    int pivotIndex = low + rand() % (high - low + 1);
    swap(arr[pivotIndex], arr[high]);
    int pivot = arr[high];


    for(int j = low;j < high; j ++)
    {
        if(arr[j] <= pivot)
        {
            swap(arr[i],arr[j]);
            i++;
        }
    }
    swap(arr[i],arr[high]);
    myquicksort(arr,low,i - 1);
    myquicksort(arr,i + 1,high);
}
void myquicksort(vector<int>& arr)
{
   if (arr.size() <= 1) return;  // 处理空数组或单元素
    
    // 初始化随机种子
    srand(time(nullptr));
    // 调用完整版快速排序
    myquicksort(arr, 0, arr.size() - 1);
}

// 调整堆（大顶堆）的函数
// 参数：
//   arr: 待排序数组
//   n: 当前堆的大小
//   i: 当前需要调整的节点索引
void heapify(vector<int>& arr, int n, int i) {
    int largest = i;       // 初始化最大元素为当前节点
    int left = 2 * i + 1;  // 左子节点索引
    int right = 2 * i + 2; // 右子节点索引

    // 如果左子节点存在且大于当前最大节点
    if (left < n && arr[left] > arr[largest])
        largest = left;

    // 如果右子节点存在且大于当前最大节点
    if (right < n && arr[right] > arr[largest])
        largest = right;

    // 如果最大值不是当前节点，则交换并递归调整
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest); // 递归调整受影响的子树
    }
}
// 堆排序主函数
void heapSort(vector<int>& arr) {
    int n = arr.size();

    // 构建大顶堆（从最后一个非叶子节点开始）
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // 逐个提取堆顶元素（最大值）并调整堆
    for (int i = n - 1; i > 0; i--) {
        // 将当前堆顶（最大值）移到数组末尾
        swap(arr[0], arr[i]);
        // 调整剩余元素使其满足堆性质
        heapify(arr, i, 0);
    }
}
int main() {
    // 测试数据
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90,2,3,12};
    
    std::cout << "排序前: ";
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    

    //bubbleSort(arr);
    //mybubblesort(arr);
    //selectionSort(arr);
    //myselectsort(arr);
    //quickSort(arr);
    //myquicksort(arr);
    heapSort(arr);

    std::cout << "排序后: ";
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    
    return 0;
}