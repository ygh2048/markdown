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




