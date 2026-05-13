# 编译环境介绍
## CMake/Makefile
Makefile 本质上就是一个 编译脚本，告诉 make 工具：
要生成什么目标文件；
依赖哪些源文件；
用什么命令编译；
用什么命令清理。

### 编译和链接是什么？
这是面试常问点。
从 .c 文件到可执行程序，一般经历：
**预处理 -> 编译 -> 汇编 -> 链接**
简单理解：
预处理
处理 #include、#define。
gcc -E main.c -o main.i
编译
把 C 代码变成汇编代码。
gcc -S main.i -o main.s
汇编
把汇编代码变成目标文件 .o。
gcc -c main.s -o main.o
链接
把多个 .o 文件和库文件合成最终可执行程序。
gcc main.o modbus.o -o app
你在项目里看到的 .o 文件，就是中间目标文件。

### CMake
Makefile 是手写编译规则，CMake 是自动生成 Makefile。
CMake 的配置文件叫：
CMakeLists.txt
Makefile 是 make 工具直接读取的编译规则文件，需要手动描述目标、依赖和编译命令。CMake 是更高层的跨平台构建工具，通过编写 CMakeLists.txt 来描述项目结构、源文件、头文件路径、链接库和编译选项，然后由 CMake 生成 Makefile 或 Ninja 文件，再交给 make 或 ninja 执行实际编译。实际项目中 CMake 更适合管理多目录、多平台和交叉编译工程。

#### ninja

它和 make 类似，都是用来执行编译流程的工具。
Ninja 是构建工具，build.ninja 是它读取的构建规则文件；它和 Makefile/Make 的关系类似，但通常更快。

## 关系关联

GCC/G++ 是真正负责 C/C++ 编译和链接的编译器。Make 和 Ninja 是构建执行工具，负责根据 Makefile 或 build.ninja 中的规则判断哪些文件需要重新编译，并调用 GCC/G++ 完成编译。CMake 是更高层的构建系统生成工具，它读取 CMakeLists.txt，根据平台、编译器和构建参数生成 Makefile 或 build.ninja。实际项目中通常是 CMake 负责生成构建规则，Make/Ninja 负责执行规则，GCC/G++ 负责真正编译代码。

# 交叉编译介绍



## 交叉编译时 CMake 怎么配置？

通常通过 CMake toolchain 文件配置目标系统、交叉编译器、sysroot、查找库路径等信息。比如在 PC 上编译 RV1126 程序时，需要指定 AArch64 交叉编译器和目标板 sysroot，确保链接的是 ARM 平台的头文件和库，而不是主机 x86 的库。

## toolchain 
3. 什么是 toolchain？

toolchain = 工具链。

你可以把它理解成一套编译工具包。

它通常包括：

gcc/g++      编译器
as           汇编器
ld           链接器
ar           静态库打包工具
strip        去符号工具，减小程序体积
readelf      查看 ELF 文件信息
objdump      反汇编和查看目标文件
libc         C 标准库，比如 glibc、musl、uclibc

在交叉编译场景下，toolchain 的名字通常带目标架构前缀。

比如：

aarch64-buildroot-linux-gnu-gcc
aarch64-buildroot-linux-gnu-g++
aarch64-buildroot-linux-gnu-ld
aarch64-buildroot-linux-gnu-readelf

这里的前缀：

aarch64-buildroot-linux-gnu-

说明这是给 AArch64 Linux 平台用的交叉编译工具链。

## 什么是 sysroot？

sysroot 是目标板系统环境的一部分。

你可以理解为：

sysroot 是一份“给编译器看的目标板根文件系统”。

它里面通常有目标板的：

头文件
库文件
pkg-config 文件
运行时依赖

比如：

sysroot/
├── usr/
│   ├── include/
│   │   ├── stdio.h
│   │   ├── pthread.h
│   │   └── modbus/
│   │       └── modbus.h
│   └── lib/
│       ├── libc.so
│       ├── libpthread.so
│       └── libmodbus.so

你写代码：

#include <modbus/modbus.h>

编译器需要找到 modbus.h。

你链接：

-lmodbus

链接器需要找到 libmodbus.so。

但是注意：这些头文件和库必须是 目标板 ARM/AArch64 环境的，不能是你 PC 上 x86 的