# knowledge     

## 命名空间
命名空间是一种在大型代码库中组织和区分不同类、接口、函数等标识符的机制。	
>防止名称冲突
>具有更好的组织结构
>提高可读性和可维护性

**std**:代表标准库的命名空间		
cout	
cin		
endl:插入一个换行符\n      刷新输出缓冲区		



# STL
标准模板库，是C++标准库中的核心组成部分，提供了一套通用的，模板化的数据结构和算法       
旨在提高代码复用性和开发效率        

## vector
### 定义
>定义一个一维变长数组        
`vector<typename> name;`    

>定义一个二维vector数组
`vetctor<typename> Arrayname[size]`     
其中每个元素都是一个vector      

### 初始化      
>使用花括号赋值
`vector<int> v{1,2,3,4,5}; //直接使用花括号赋值`
`for(auto i : v) cout << i << " "; //输出1 2 3 4 5`

>使用圆括号赋值
>```
>vector<int> v(5); //初始化5个值为0的元素
>for(auto i : v) cout << i << " "; //输出0 0 0 0 0
>```
>```
>vector<int> v(5, 4); //初始化5个值为4的元素
>for(auto i : v) cout << i << " "; //输出4 4 4 4 4
>```

### 访问
>下标访问   
`v[0],v[1],v[2]`

>迭代器访问
`vector<typename>::iterator it;`可以吧迭代器理解为类似指针      
```
>#include<bits/stdc++.h>
>using namespace std;
int main(){
	vector<int> v;
	v.push_back(1);	//依次输入1、 2、 3 
	v.push_back(2);
	v.push_back(3);
	for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
		cout << *it << " ";
	}
	
    return 0;
} 
```

### 常见函数    
- v.push_back(x)，就是在vector容器v后面添加一个元素x，时间复杂度为O(1)。
```
#include<bits/stdc++.h>
using namespace std;
int main(){
	vector<int> v;
	for(int i = 1; i <= 3; i++){
		v.push_back(i);
	}
	for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
		cout << *it << " ";
	}
	return 0;
} 
```
- pop_back()可以删除vector的尾元素，时间复杂度为O(1)：
```
#include<bits/stdc++.h>
using namespace std;
int main(){
	vector<int> v;
	for(int i = 1; i <= 3; i++){
		v.push_back(i);	//将1、2、3 依次插入v的队尾 
	}
	v.pop_back();	//删除尾元素 
	for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
		cout << *it << " ";
	}
	
    return 0;
} 
```
输出：`1 2`

- size()用来获得vector中元素的个数，时间复杂度为O(1)。size()返回的是unsigned类型
```
#include<bits/stdc++.h>
using namespace std;
int main(){
	vector<int> v;
	for(int i = 1; i <= 3; i++){
		v.push_back(i);	//将1、2、3 依次插入v的队尾 
	}
	v.pop_back();	//删除尾元素 
	//for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
//		cout << *it << " ";
//	}
	cout << v.size();    //获取容器长度
 
    return 0;
} 
```
输出结果`2` 
- clear()用来清空vector中的所有元素，时间复杂度为O(N)，N为vector中的元素个数：
```
#include<bits/stdc++.h>
using namespace std;
int main(){
	vector<int> v;
	for(int i = 1; i <= 3; i++){
		v.push_back(i);	//将1、2、3 依次插入v的队尾 
	}
	v.pop_back();	//删除尾元素 
	v.clear();	
	//for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
//		cout << *it << " ";
//	}
	cout << v.size();
	
    return 0;
} 
```
输出结果`0` 

- insert(it, x)用来向vector的任意迭代器it处插入一个元素x，时间复杂度O(N)：
```
#include<bits/stdc++.h>
using namespace std;
int main(){
	vector<int> v;
	for(int i = 1; i <= 5; i++){
		v.push_back(i);	//将1、2、3、4、5 依次插入v的队尾 
	}
	v.insert(v.begin() + 2, -1);	//将-1插入v[2]的位置 
	for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
		cout << *it << " ";
	}
	
	return 0; 
} 
```
输出结果`1 2 -1 3 4 5`      

- insert(pos,first,last) 在 pos 位置之前，插入其他容器（不仅限于vector）中位于 [first,last) 区域的所有元素(简单说就是把两个容器拼接在一起)。
```
#include<bits/stdc++.h>
using namespace std;
 
int main(){
	vector<int> v, v1, v2;
    for(int i = 1; i <= 3; i++) v.push_back(i);
	for(int i  = 11; i <= 13; i++) v1.push_back(i);
	for(int i  = 101; i <= 103; i++) v2.push_back(i);
	v.insert(v.end(), v1.begin(), v1.end());  //输出1 2 3 11 12 13
	v.insert(v.end(), v2.begin(), v2.end()); //输出1 2 3 11 12 13 101 102 103
 
    return 0;
}
```

- erase()有两种用法：删除单个元素、删除一个区间内所有元素。时间复杂度为O(N)。
>删除单个元素
```
#include<bits/stdc++.h>
using namespace std;
int main(){
	vector<int> v;
	for(int i = 1; i <= 5; i++){
		v.push_back(i);	//将1、2、3、4、5 依次插入v的队尾 
	}
	v.erase(v.begin() + 2);	//删除3 
	for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
		cout << *it << " ";
	}
	
	return 0; 
} 
```
输出结果`1 2 4 5`       
>删除一个区间的所有元素     
```
#include<bits/stdc++.h>
using namespace std;
int main(){
	vector<int> v;
	for(int i = 1; i <= 5; i++){
		v.push_back(i);	//将1、2、3、4、5 依次插入v的队尾 
	}
	v.erase(v.begin() + 1, v.begin() + 3);	//删除2和3 
	for(vector<int>::iterator it = v.begin(); it != v.end(); it++){
		cout << *it << " ";
	}
	
	return 0; 
} 
```
输出结果`1 4 5`     
>如果想要用erase清空vector容器，正确的写法是 v.erase(v.begin(), v.end())，v.end() 表示的是尾元素的下一个地址。


以下是整理为 Markdown 表格格式的容器迭代器相关成员函数汇总：

---

| **成员函数** | **功能**                                                                                                     |
|--------------|------------------------------------------------------------------------------------------------------------|
| `begin()`    | 返回指向容器第一个元素的正向迭代器；若容器是 `const` 类型，返回常量正向迭代器。                               |
| `end()`      | 返回指向容器最后一个元素之后位置的正向迭代器；若容器是 `const` 类型，返回常量正向迭代器。通常与 `begin()` 搭配使用。 |
| `rbegin()`   | 返回指向最后一个元素的反向迭代器；若容器是 `const` 类型，返回常量反向迭代器。                                  |
| `rend()`     | 返回指向第一个元素之前位置的反向迭代器；若容器是 `const` 类型，返回常量反向迭代器。通常与 `rbegin()` 搭配使用。   |
| `cbegin()`   | 返回常量正向迭代器（与 `begin()` 类似），不可修改元素。                                                      |
| `cend()`     | 返回常量正向迭代器（与 `end()` 类似），不可修改元素。                                                        |
| `crbegin()`  | 返回常量反向迭代器（与 `rbegin()` 类似），不可修改元素。                                                     |
| `crend()`    | 返回常量反向迭代器（与 `rend()` 类似），不可修改元素。                                                       |

---

### **补充说明**  
1. **迭代器类型**：  
   - **正向迭代器**：从容器头部向尾部遍历（`begin()` → `end()`）。  
   - **反向迭代器**：从容器尾部向头部遍历（`rbegin()` → `rend()`）。  
   - **常量迭代器**：禁止通过迭代器修改元素（`cbegin()`, `crbegin()` 等）。  

2. **`const` 容器的行为**：  
   - 若容器为 `const` 类型（如 `const vector<int>`），所有迭代器函数（包括 `begin()`、`rbegin()`）默认返回常量迭代器。  
   - 非 `const` 容器可通过 `cbegin()` 等函数显式获取常量迭代器。  

3. **典型用法**：  
   ```cpp
   vector<int> vec = {1, 2, 3};
   
   // 正向遍历（非 const 容器）
   for (auto it = vec.begin(); it != vec.end(); ++it) {
       *it += 1; // 可修改元素
   }
   
   // 反向遍历（const 容器）
   const vector<int> cvec = {4, 5, 6};
   for (auto rit = cvec.rbegin(); rit != cvec.rend(); ++rit) {
       // *rit += 1; // 错误！常量迭代器不可修改元素
   }
   ```

4. **兼容性**：  
   - 所有标准库容器（如 `vector`、`list`、`map`）均支持这些成员函数。  
   - 反向迭代器可通过 `base()` 方法转换为正向迭代器。  

--- 

此表格适用于 C++ 标准库中支持迭代器的容器（如 `vector`、`deque`、`list` 等）。


## array
模板结构体	
### 特点
array的容器的大小是固定的，无法动态的扩展或收缩，这就意味着只允许访问或者替换存储的元素		

**头文件**：`#include<array>`		

### 函数操作
`array<double, 10> values;`		
注意，初始化不会全部赋零	

以下是整理为 Markdown 表格格式的 `array` 容器成员函数汇总：

---

| **成员函数**       | **功能**                                                                                                                                 |
|--------------------|----------------------------------------------------------------------------------------------------------------------------------------|
| `begin()`          | 返回指向容器中第一个元素的随机访问迭代器。                                                                                               |
| `end()`            | 返回指向容器最后一个元素之后一个位置的随机访问迭代器，通常和 `begin()` 结合使用。                                                        |
| `rbegin()`         | 返回指向最后一个元素的随机访问迭代器（反向迭代器）。                                                                                     |
| `rend()`           | 返回指向第一个元素之前一个位置的随机访问迭代器（反向迭代器）。                                                                           |
| `cbegin()`         | 和 `begin()` 功能相同，但具有 `const` 属性，不可用于修改元素。                                                                           |
| `cend()`           | 和 `end()` 功能相同，但具有 `const` 属性，不可用于修改元素。                                                                             |
| `crbegin()`        | 和 `rbegin()` 功能相同，但具有 `const` 属性，不可用于修改元素。                                                                          |
| `crend()`          | 和 `rend()` 功能相同，但具有 `const` 属性，不可用于修改元素。                                                                            |
| `size()`           | 返回容器中当前元素的数量，其值始终等于初始化 `array` 的模板参数 `N`。注意：与 `vector` 不同，`size()` 获取的是固定容量，而非实际元素数量。 |
| `max_size()`       | 返回容器可容纳元素的最大数量，其值始终等于模板参数 `N`。                                                                                 |
| `empty()`          | 判断容器是否为空，等价于 `size() == 0`，但实现可能更高效。                                                                               |
| `at(n)`            | 返回位置 `n` 处元素的引用，自动检查范围有效性，越界时抛出 `out_of_range` 异常。                                                         |
| `front()`          | 返回容器中第一个元素的直接引用，空容器调用此函数行为未定义。                                                                             |
| `back()`           | 返回容器中最后一个元素的直接引用，空容器调用此函数行为未定义。                                                                           |
| `data()`           | 返回指向容器首元素的指针，可用于直接操作底层数组。                                                                                       |
| `fill(val)`        | 将值 `val` 赋给容器中的每个元素。                                                                                                        |
| `swap(array2)`     | 交换两个 `array` 的所有元素，要求两者类型和长度完全相同。                                                                                |

---

### **关键说明**：
1. **`size()` 的特殊性**：  
   `array` 的 `size()` 返回的是模板参数 `N`（固定容量），而非动态元素数量。例如：  
   ```cpp
   array<int, 5> arr;  // arr.size() 始终为 5，即使未完全初始化
   ```

2. **迭代器类型**：  
   - `begin()`/`end()` 提供正向迭代。  
   - `rbegin()`/`rend()` 提供反向迭代。  
   - `c` 前缀版本（如 `cbegin()`）为只读迭代器。

3. **`swap` 的限制**：  
   交换的 `array` 必须满足以下条件：  
   - 相同数据类型（如 `array<int, 5>` 只能与 `array<int, 5>` 交换）。  
   - 相同长度（模板参数 `N` 一致）。

4. **`data()` 的应用**：  
   可通过指针操作实现底层数组的快速访问或复制：  
   ```cpp
   array<int, 3> arr = {1, 2, 3};
   int* ptr = arr.data();  // ptr 指向 arr[0]
   ```
# code


# 跳转链接
[点这里跳转list](../list.md)