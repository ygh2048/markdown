# 基本知识  
## 万物皆文件
是unix和类unix系统的核心设计哲学之一。
它并非字面意义上的“所有东西都是文件”，而是通过统一的文件接口抽象，将系统的各种资源(如硬件设备，进程，网络连接等)以文件的形式呈现和管理，这种设计简化了系统的复杂性，提供了高度一致的编程和操作体验。    

**用简单统一的规则解决复杂问题**    

### 核心思想 
- 抽象统一接口  
    ```
    所有资源(磁盘，键盘，内存，还是网络套接字)都通过文件描述符(file descriptor)进行访问，
    使用相同的系统调用如read(),write(),open(),close()
    ```
- 命名与路径
    ```
    资源在文件系统中被赋予路径(如/dev/sda 表示硬盘，
    /proc/cpuinfo表示CPU信息)，用户可以通过文件路径直接访问。
    ```

### 具体表现    

#### 硬件设备即文件 
- 磁盘和分区：/dev/sda,/dev/sdal        
- 键盘和鼠标：/dev/input/event*     
- 终端设备：/dev/tty*
- 实例
  ```
    # 向终端发送数据（写入文件）
    echo "Hello" > /dev/tty1
    
    # 读取磁盘的 MBR（前512字节）
    dd if=/dev/sda of=mbr.bin bs=512 count=1
  ```

#### 进程和系统信息即文件
- 进程信息：/porc/PID/目录
- 系统参数：/sys/---
- 实例：
    ```
    # 查看 CPU 信息
    cat /proc/cpuinfo
    
    # 动态修改内核参数（如开启 IP 转发）
    echo 1 > /proc/sys/net/ipv4/ip_forward
    ```

#### 网络和通信即文件   
- Unix域套接字：通过文件路径如 /tmp/my_socket 通信
- 管道(pipe):通过文件描述符传递数据 
- 示例：
  ```
    # 创建命名管道
    mkfifo my_pipe
    # 进程A写入
    echo "data" > my_pipe
    # 进程B读取
    cat my_pipe
  ```
#### 特殊文件类型   
- 块设备文件(如硬盘)：支持随机访问，以块为单位操作。    
- 字符设备文件(如串口)：按字节流顺序访问    

## shell脚本编程    

在win中就是批处理文件    .bat   .cmd   
linux是shell脚本文件    .sh 


都是用于自动化任务的脚本文件    

### 关键点
**约定头**
`#！/bin/bash`      
#!是一个约定的标记，它告诉系统这个脚本需要什么解释器来执行，即使用哪一种shell;后面的/bin/bash就是指明了解释器的具体位置        

**权限**
`chmod +x xxx.sh`   
为文件添加可执行权限    

**执行**
- ./xxx.sh :先按照 文件中#!指定的解析器解析，如果#！指定指定的解析器不存在 才会使用系统默认的解析器

- bash xxx.sh:指明先用bash解析器解析，如果bash不存在 才会使用默认解析器

- . xxx.sh 直接使用默认解析器解析（不会执行第一行的#！指定的解析器，但是第一行还是要写的

**添加环境变量**    
常见语句:   
`echo $PATH` 查看当前环境变量   
`source ~/.bashrc` 重新加载bashrc的内容(比如环境变量)   
`echo 'export MY_VAR="my_value"' >> ~/.bashrc`永久添加环境变量  
`export MY_VAR="my_value"`  临时添加环境变量    

### 语句
#### 输入语句   
`read -p 'input name:' name`    
运行结果：
`input name:`   

-p就是显示其中内容，不加就是直接读取

*读取多个值*
`read data1 data2`

#### 输出语句   
`echo $name`    

### 变量
*定义：*
变量名等于变量值    
`num = 10`  
*引用变量：*    
`$num`
*清楚变量名*    
`unset num` 

#### 控制语句   

**if控制语句**：    
```
格式一：
if [条件1]; then
    执行第一段程序
else
    执行第二段程序
fi
格式二：
if [条件1]; then
    执行第一段程序
elif [条件2]；then
执行第二段程序
else
    执行第三段程序
fi

比如：
if[$num = 1];then
    

```


**for循环语句**：   
```
#格式：for name [ [ in [ word ... ] ] ; ] do list ; done
  for 变量名 in 取值列表; do
    语句 1
  done

```

**while循环语句**： 
while
```
# While循环语句与for循环功能类似，主要用于对某个数据域进行循环读取、对文件进行遍历，通常用于需要循环某个文件或者列表，满足循环条件会一直循环，不满足则退出循环，其语法格式以while…do开头，done结尾与 
#while 关联的还有一个 until 语句，它与 while 不同之处在于，是当条件表达式为 false 时才循环，实际使用中比较少，这里不再讲解。

while  (表达式)
do
  语句1
done

```
break和continue 
```
# break 和 continue 语句
  break 是终止循环。
  continue 是跳出当前循环。
#示例 1：在死循环中，满足条件终止循环
while true; do
  let N++
  if [ $N -eq 5 ]; then
    break
fi
  echo $N
done
输出： 1 2 3 4

#示例 2：举例子说明 continue 用法
N=0
while [ $N -lt 5 ]; do
  let N++
if [ $N -eq 3 ]; then
  continue
fi
  echo $N
done

输出： 1 2 4

# 打印 1-100 数字
i=0
while ((i<=100))
do
        echo  $i
        i=`expr $i + 1`
done


```

# 基本操作  

## apt包管理器

##  一些常用命令行  

## 线程操作 

### 线程通信    

# socket套接字
[CSDN来源](https://blog.csdn.net/Edward_Asia/article/details/127412092)
## 网络     
网络分层![alt text](image.png)

7层模型![alt text](image-1.png)


## socket

### 源IP和目的IP    
*报文的来源*    
*报文的目的地*  

### 端口号
port
是传输层协议的内容      
- 是一个2字节16位的整数
- 用来表示一个进程，告诉操作系统当前的这个数据要交给哪个进程来处理  
- IP地址 + 端口号能够标识网络上的某一台主机的某个进程   
- 一个端口号只能被一个进程占用  
  
### 端口号和进程ID
pid表示唯一一个进程，此外我们的端口号也是唯一一个进程   
一个进程可以绑定多个端口号，但是一个端口号不能被多个进程绑定    
IP + port端口号 = 互联网中的唯一的一个进程  

*要先通信，本质为：*    
- 先找到目标主机
- 再找到该主机上的服务（进程）

**所以互联网中的通信实际上是进程间通信**    


## socket 编程接口