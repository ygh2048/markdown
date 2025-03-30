# knowledge     

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
# code


# 跳转链接
[点这里跳转list](../list.md)