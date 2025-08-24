#  linux 命令

## 基本命令

### 关机命令
|指令|说明|
|-|-|
|shutdown|安全关机|
|reboot|重启|
|halt|调用 shutdown -h|
|poweroff|halt软链接|


## 特殊命令


### 正则表达式

grep   

例如：  
```shell
#匹配所有d开头的文件 包含test的匹配行
grep  "test"  d*
#包含test和zephyr  不区分大小写  显示行号   扩展正则表达式
grep -inE "test|zephyr" d*
#包含test和zephyr 不区分大小写  显示行号   扩展正则表达式
grep -in "test" d* | grep 'zephyr'
```