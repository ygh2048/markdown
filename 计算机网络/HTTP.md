# 介绍

**超文本传输协议**  
分布式，协作式，超媒体信息的应用层协议      
默认端口80  


客户端/服务器模式       

HTTPS 超文本传输安全协议  增加了SSL/TLS协议，通常使用端口443    




## 工作原理
**请求响应模型**    
- 建立连接
- 发送请求
- 处理请求
- 发送响应
- 关闭连接

## 协议组成

### HTTP 方法
- GET 从服务器获取指定资源
- POST 请求服务器接受处理请求体中的数据
- PUT 请求服务器存储一个资源
- DELETE 请求服务器删除指定的资源
- HEAD 与GET类似，只获取响应头信息


#### 响应
- 状态行
- 消息报头
- 空行
- 响应正文

#### 请求
- 请求行
- 请求头
- 空行
- 请求体

#### 状态码
1XX   信息性状态码  表示接受的请求正在处理
2XX   成功状态码    表示请求正常处理完毕
3XX   重定向状态码
4XX   客户端错误状态码
5XX   服务器错误状态码

### keep alive
会保持请求的那个TCP连接    ，就是复用TCP连接进行处理多个HTTP事物        

 终止
 - 超时关闭
 - 限额关闭
 - 主动关闭
 - 错误关闭
 - 应用关闭


区别于TCP的keepalive，TCP的是检查连接是否还在，无响应后主动关闭     

# 有关代码
客户端请求：        

```
GET /index.html HTTP/1.1
Host: www.example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:91.0) Gecko/20100101 Firefox/91.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Encoding: gzip, deflate
Connection: keep-alive

```



```
HTTP/1.1 200 OK
Date: Wed, 18 Apr 2024 12:00:00 GMT
Server: Apache/2.4.1 (Unix)
Last-Modified: Wed, 18 Apr 2024 11:00:00 GMT
Content-Length: 12345
Content-Type: text/html; charset=UTF-8

<!DOCTYPE html>
<html>
<head>
    <title>Example Page</title>
</head>
<body>
    <h1>Hello, World!</h1>
    <!-- The rest of the HTML content -->
</body>
</html>
```

# 具体流程

1. 客户端发起请求
2. 服务器处理请求
3. 服务器返回响应
4. 客户端渲染页面
