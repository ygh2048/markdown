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
>v.push_back(x)，就是在vector容器v后面添加一个元素x，时间复杂度为O(1)。
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

# code




# 跳转链接
[点这里跳转list](../list.md)