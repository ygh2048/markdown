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

**sysroot** 可以先记成一句话：

> **sysroot 是交叉编译时给编译器看的“目标板系统环境”，里面放的是目标板的头文件和库。**

它不是给你电脑运行用的，也不是完整意义上的板端系统，而是为了让 PC 上的交叉编译器知道：

```text
目标板上有哪些头文件？
目标板上有哪些库？
目标板的 libc、pthread、libmodbus、gstreamer 等在哪里？
```

---

# 1. 为什么需要 sysroot？

你在 PC 上开发，PC 是：

```text
x86_64
```

RV1126 板子是：

```text
ARM / AArch64
```

如果你写了一个程序：

```cpp
#include <modbus/modbus.h>

int main() {
    return 0;
}
```

编译时需要两个东西：

```text
1. 头文件：modbus.h
2. 库文件：libmodbus.so
```

但是注意：

> 你不能用 PC 上 x86 版本的 `libmodbus.so`，而要用 RV1126 板子 ARM/AArch64 版本的 `libmodbus.so`。

否则编译或运行就会出问题。

所以 sysroot 的作用就是：

> **让交叉编译器去目标板环境里找头文件和库，而不是误用 PC 自己的 `/usr/include`、`/usr/lib`。**

---

# 2. sysroot 里面一般有什么？

一个 sysroot 目录大概长这样：

```text
rv1126_sysroot/
├── usr/
│   ├── include/
│   │   ├── stdio.h
│   │   ├── pthread.h
│   │   └── modbus/
│   │       └── modbus.h
│   └── lib/
│       ├── libc.so
│       ├── libpthread.so
│       ├── libstdc++.so
│       └── libmodbus.so
├── lib/
│   ├── ld-linux-aarch64.so.1
│   └── libc.so
└── etc/
```

重点是：

```text
usr/include   放头文件
usr/lib       放库文件
lib           放系统基础库和动态链接器
```

---

# 3. sysroot 给谁用？

sysroot 是给 **交叉编译器** 用的。

比如：

```bash
aarch64-buildroot-linux-gnu-g++ \
  --sysroot=/path/to/rv1126_sysroot \
  main.cpp \
  -lmodbus \
  -o app
```

这句话的意思是：

```text
用 AArch64 交叉编译器编译 main.cpp
并且去 /path/to/rv1126_sysroot 里面找头文件和库
```

而不是去 PC 自己的：

```text
/usr/include
/usr/lib
```

---

# 4. sysroot 和 toolchain 的关系

你可以这样理解：

```text
toolchain：负责怎么编译
sysroot：提供编译时需要的目标板环境
```

比如：

```text
toolchain = aarch64-buildroot-linux-gnu-g++
sysroot   = RV1126 的头文件和库目录
```

它们配合起来，才能在 PC 上编译出 RV1126 能运行的程序。

---

# 5. sysroot 和 rootfs 的区别

这个很重要。

| 名称          | 用途     | 给谁用     |
| ----------- | ------ | ------- |
| **sysroot** | 交叉编译时用 | 给编译器用   |
| **rootfs**  | 板子运行时用 | 给目标板系统用 |

一句话：

> **sysroot 是编译时环境，rootfs 是运行时环境。**

举例：

你在 PC 上交叉编译时，编译器从 sysroot 里找：

```text
modbus.h
libmodbus.so
```

程序部署到 RV1126 后，板端运行时会从 rootfs 里找：

```text
libmodbus.so
libstdc++.so
libpthread.so
```

如果板端 rootfs 里没有对应动态库，程序就可能运行失败。

---

# 6. CMake 里怎么使用 sysroot？

在 CMake 交叉编译时，通常会在 toolchain 文件里写：

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER aarch64-buildroot-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-buildroot-linux-gnu-g++)

set(CMAKE_SYSROOT /path/to/rv1126_sysroot)
```

这句最关键：

```cmake
set(CMAKE_SYSROOT /path/to/rv1126_sysroot)
```

它告诉 CMake：

> 查找头文件和库时，以这个目录作为目标系统根目录。

---

# 7. sysroot 出问题会报什么错？

## 情况 1：找不到头文件

比如：

```text
fatal error: modbus/modbus.h: No such file or directory
```

可能原因：

```text
sysroot 里面没有 modbus.h
CMake 没有正确设置 CMAKE_SYSROOT
include 路径没配置对
```

排查：

```bash
find $RV1126_SYSROOT -name modbus.h
```

---

## 情况 2：找不到库

比如：

```text
cannot find -lmodbus
```

可能原因：

```text
sysroot 里面没有 libmodbus.so
库路径没有被 CMake 找到
链接到了错误的路径
```

排查：

```bash
find $RV1126_SYSROOT -name "libmodbus*"
```

---

## 情况 3：误用了 PC 的库

这是交叉编译里很坑的问题。

如果你不设置 sysroot，CMake 可能找到 PC 的库：

```text
/usr/lib/x86_64-linux-gnu/libmodbus.so
```

但你真正需要的是 ARM/AArch64 库：

```text
/path/to/rv1126_sysroot/usr/lib/libmodbus.so
```

所以交叉编译时要特别注意不要混用主机库。

---

# 8. 怎么确认 sysroot 里的库是 ARM/AArch64 的？

可以用 `file` 查看：

```bash
file $RV1126_SYSROOT/usr/lib/libmodbus.so
```

如果是 RV1126 平台，应该类似：

```text
ELF 64-bit LSB shared object, ARM aarch64
```

如果显示：

```text
ELF 64-bit LSB shared object, x86-64
```

那就是错的，说明这个库是 PC 平台的，不能给 RV1126 用。

---

# 9. 和你项目怎么关联？

你的 RV1126 项目可以这样讲：

> 我在 PC 上交叉编译 RV1126 程序时，需要配置 AArch64 工具链和 RV1126 sysroot。sysroot 里提供目标板的头文件和库，比如 libmodbus、GStreamer、pthread、libstdc++ 等。CMake 通过 toolchain 文件指定 `CMAKE_SYSROOT`，保证编译和链接时使用 ARM/AArch64 目标平台依赖，而不是误用主机 x86 依赖。

---

# 10. 面试回答版本

如果面试官问：**sysroot 是什么？**

你可以这样答：

> sysroot 是交叉编译时使用的目标系统根目录，里面包含目标平台的头文件、库文件和部分运行时环境。比如在 x86 PC 上编译 RV1126 的 AArch64 程序时，编译器需要从 RV1126 的 sysroot 中查找 `usr/include` 和 `usr/lib`，确保链接的是 ARM 平台的库，而不是 PC 本机的 x86 库。简单说，sysroot 是编译时给交叉编译器使用的目标板环境。

再补一句区别：

> 它和 rootfs 的区别是：sysroot 主要给编译器使用，属于编译时环境；rootfs 是板子启动后真正挂载的根文件系统，属于运行时环境。

---

# 11. 一句话总结

> **sysroot = 交叉编译时使用的目标板头文件和库目录，用来保证 PC 上编译出来的程序能正确链接到 ARM/RV1126 平台的依赖。**


你这里应该是想问 **设备树**，不是“设备商”。我按 **嵌入式 Linux 设备树 Device Tree** 来讲。

一句话理解：

> **设备树是用来描述硬件信息的文件，告诉 Linux 内核：这块板子上有哪些硬件、它们挂在哪条总线上、地址是多少、用哪些 GPIO/中断/时钟/电源。**

Linux 官方文档里也把 Device Tree 描述为一种“操作系统可读的硬件描述”，这样内核就不需要把每块板子的硬件细节都写死在代码里。([Linux内核文档][1])

---

## 1. 为什么需要设备树？

以前如果每块板子的硬件都不一样，比如：

```text
板子 A：UART0 接调试串口，I2C1 接摄像头
板子 B：UART2 接调试串口，I2C3 接摄像头
板子 C：GPIO12 控制摄像头复位
```

如果这些信息都写死在内核代码里，就会导致：

```text
每换一块板子，就要改内核源码
```

这很麻烦。

所以 Linux 用设备树把“硬件描述”单独拿出来。

也就是说：

```text
驱动代码：负责怎么操作硬件
设备树：负责告诉驱动硬件在哪里、怎么连接
```

---

## 2. 设备树在启动流程中的位置

嵌入式 Linux 启动流程大概是：

```text
上电
 ↓
BootROM
 ↓
U-Boot
 ↓
加载 Kernel
 ↓
加载 DTB 设备树
 ↓
Kernel 根据设备树初始化硬件和匹配驱动
 ↓
生成 /dev 设备节点
 ↓
启动 rootfs 和应用程序
```

设备树通常由 **U-Boot 加载并传递给 Linux Kernel**。

你可以这样理解：

```text
Kernel 是操作系统核心
DTB 是硬件说明书
Driver 是硬件操作方法
```

Kernel 拿到 DTB 后，才知道当前板子上有哪些设备。

---

## 3. DTS、DTSI、DTB 分别是什么？

设备树相关经常看到这几个文件：

```text
.dts
.dtsi
.dtb
```

### 1）`.dts`

`.dts` 是 Device Tree Source，设备树源文件。

它是人能看懂、能编辑的文本文件。

例如：

```dts
/ {
    model = "RV1126 Board";
    compatible = "rockchip,rv1126";
};
```

---

### 2）`.dtsi`

`.dtsi` 类似 C 语言里的头文件，是公共设备树片段。

比如 Rockchip RV1126 这个芯片有很多公共外设：

```text
CPU
UART
I2C
SPI
GPIO
MIPI CSI
ISP
```

这些公共部分可以写在：

```text
rv1126.dtsi
```

具体某块板子的差异写在：

```text
rv1126-board.dts
```

`.dts` 里可以 include `.dtsi`。

---

### 3）`.dtb`

`.dtb` 是 Device Tree Blob，是编译后的二进制设备树文件。

也就是说：

```text
.dts / .dtsi  源文件
      ↓ dtc 编译
.dtb          二进制文件
      ↓ U-Boot 加载
Linux Kernel 使用
```

设备树编译器叫：

```text
dtc
```

---

## 4. 设备树长什么样？

一个非常简化的例子：

```dts
/ {
    model = "example board";
    compatible = "vendor,example-board";

    memory@0 {
        device_type = "memory";
        reg = <0x00000000 0x40000000>;
    };

    chosen {
        bootargs = "console=ttyS0,115200 root=/dev/mmcblk0p5 rw";
    };
};
```

这段描述：

```text
板子型号是什么
内存从哪里开始，有多大
内核启动参数是什么
```

---

## 5. 设备树节点和属性是什么？

设备树是树状结构。

```dts
&i2c1 {
    status = "okay";

    imx307: camera@1a {
        compatible = "sony,imx307";
        reg = <0x1a>;
        reset-gpios = <&gpio2 5 GPIO_ACTIVE_LOW>;
    };
};
```

这里有两个重点：

### 节点 node

比如：

```dts
camera@1a
```

这是一个设备节点。

它表示 I2C 总线上地址为 `0x1a` 的摄像头。

---

### 属性 property

比如：

```dts
compatible = "sony,imx307";
reg = <0x1a>;
reset-gpios = <&gpio2 5 GPIO_ACTIVE_LOW>;
status = "okay";
```

这些都是属性。

含义大概是：

```text
compatible：用来匹配驱动
reg：设备地址
reset-gpios：复位引脚
status：是否启用这个设备
```

---

## 6. `compatible` 是什么？

`compatible` 是设备树里非常关键的属性。

它用来告诉内核：

> 这个设备应该匹配哪个驱动。

比如设备树里写：

```dts
compatible = "sony,imx307";
```

驱动里可能有：

```c
static const struct of_device_id imx307_of_match[] = {
    { .compatible = "sony,imx307" },
    { }
};
```

当内核发现设备树里有 `"sony,imx307"`，并且驱动里也声明支持 `"sony,imx307"`，它们就能匹配上。

然后驱动的 `probe()` 函数会被调用。

你可以这样记：

```text
compatible 是设备树和驱动之间的“暗号”
```

---

## 7. `status = "okay"` 是什么？

设备树里经常看到：

```dts
status = "disabled";
```

或者：

```dts
status = "okay";
```

含义：

```text
disabled：这个设备不启用
okay：这个设备启用
```

比如 I2C1 默认可能是关闭的：

```dts
&i2c1 {
    status = "disabled";
};
```

如果你的摄像头挂在 I2C1 上，就需要在板级 DTS 里打开：

```dts
&i2c1 {
    status = "okay";
};
```

Linux 内核文档中的设备树 API 也有 `of_device_is_available()` 这类接口，用于判断设备节点是否可用，通常就是看 `status` 是否为 `okay/ok` 或者是否缺省。([Linux内核文档][2])

---

## 8. 设备树和驱动是什么关系？

非常重要，面试常问。

可以这样理解：

```text
设备树：描述硬件
驱动：操作硬件
内核：根据设备树匹配驱动
```

例如 IMX307 摄像头：

```text
设备树告诉内核：
    这里有一个 IMX307
    它挂在 I2C 几号总线上
    I2C 地址是多少
    reset GPIO 是哪个
    pwdn GPIO 是哪个
    时钟是多少
    MIPI CSI 接到哪里

驱动负责：
    通过 I2C 配置 IMX307 寄存器
    控制 sensor 上电/复位
    设置分辨率、帧率、曝光
    和 V4L2 子设备框架对接
```

所以：

> **设备树不会操作硬件，它只是描述硬件。真正操作硬件的是驱动。**

---

## 9. 设备树和 `/dev/video*` 的关系

你项目里有 IMX307 摄像头和 RV1126 视频链路，可以这样理解：

```text
设备树中启用 IMX307 摄像头
 ↓
Kernel 启动时解析设备树
 ↓
匹配 IMX307 sensor 驱动
 ↓
驱动 probe 成功
 ↓
MIPI CSI / ISP / V4L2 链路注册成功
 ↓
用户态出现 /dev/video*
 ↓
GStreamer / 应用程序采集视频
```

如果设备树配置不对，可能表现为：

```text
没有 /dev/video*
有 /dev/video* 但采集失败
dmesg 里 sensor probe 失败
I2C 地址找不到
MIPI CSI 没有数据
```

---

## 10. 设备树里常见硬件信息

设备树通常描述这些内容：

```text
CPU 和内存
UART 串口
I2C 总线
SPI 总线
GPIO
中断 interrupt
时钟 clock
电源 regulator
复位 reset
PWM
摄像头 sensor
MIPI CSI / DSI
以太网
USB
SD/eMMC
```

官方设备树绑定文档也强调，binding 应该基于硬件本身来描述，而不是基于 Linux 驱动当前怎么实现。([Linux内核文档][3])

---

## 11. 设备树常见排查命令

### 1）查看内核运行时设备树

Linux 启动后，设备树会出现在：

```bash
/proc/device-tree
```

可以看：

```bash
ls /proc/device-tree
```

比如查看板子 model：

```bash
cat /proc/device-tree/model
```

查看 compatible：

```bash
cat /proc/device-tree/compatible
```

---

### 2）查看某个设备是否存在

比如查 imx307：

```bash
find /proc/device-tree -iname "*imx307*"
```

也可以：

```bash
grep -r "imx307" /proc/device-tree 2>/dev/null
```

---

### 3）看内核日志

```bash
dmesg | grep -i imx307
dmesg | grep -i camera
dmesg | grep -i csi
dmesg | grep -i i2c
```

---

### 4）查看设备节点

```bash
ls /dev/video*
```

如果设备树和驱动都正常，视频链路可能会生成对应 V4L2 设备节点。

---

## 12. 实际场景：IMX307 摄像头没识别，怎么排查？

可以按这个顺序：

### 第一步：看设备节点

```bash
ls /dev/video*
```

如果没有视频节点，说明 V4L2 链路可能没起来。

---

### 第二步：看 dmesg

```bash
dmesg | grep -i imx307
dmesg | grep -i i2c
dmesg | grep -i csi
dmesg | grep -i video
```

关注：

```text
probe failed
i2c read failed
no sensor found
link error
clock error
gpio error
```

---

### 第三步：确认设备树是否启用

看源码里对应节点：

```dts
status = "okay";
```

确认：

```text
I2C 总线是否 okay
IMX307 节点是否 okay
I2C 地址是否正确
reset/pwdn GPIO 是否正确
clock/regulator 是否正确
MIPI endpoint 是否连接正确
```

---

### 第四步：确认驱动是否匹配

看驱动里的 compatible 是否和设备树一致。

设备树：

```dts
compatible = "sony,imx307";
```

驱动：

```c
.compatible = "sony,imx307"
```

两边字符串必须匹配。

---

### 第五步：确认电源、时钟、复位

摄像头常见问题是：

```text
供电没开
复位 GPIO 方向/电平错
MCLK 时钟没给
I2C 地址错
MIPI lane 配置错
```

这些很多都和设备树有关。

---

## 13. 设备树和你简历怎么讲？

你可以说：

> 我了解 Linux 设备树的作用，设备树主要用于描述板级硬件资源，比如 I2C、SPI、GPIO、中断、时钟、电源和摄像头 sensor 等。驱动通过 compatible 字符串和设备树节点匹配，匹配成功后执行 probe，最终在用户态生成对应设备节点。项目中我主要在 RV1126 + IMX307 视频链路联调时，结合 `/dev/video*`、`dmesg` 和运行日志排查摄像头识别、设备节点和视频采集问题。

这句话很稳，因为它体现你了解，但不夸大成“精通驱动”。

---

## 14. 面试题：设备树是什么？

可以这样答：

> 设备树是嵌入式 Linux 中用于描述硬件资源的数据结构和配置文件。它描述板子上的 CPU、内存、总线、GPIO、中断、时钟、电源、外设等信息。U-Boot 启动 Linux 时会把编译后的 DTB 传给 Kernel，Kernel 根据设备树创建 platform device，并通过 compatible 属性匹配对应驱动。这样可以把硬件描述和驱动代码分离，减少内核中针对不同板子的硬编码。

---

## 15. 面试题：DTS 和 DTB 区别？

可以答：

> DTS 是设备树源文件，是人可以阅读和修改的文本格式；DTSI 是公共设备树片段，通常被多个 DTS 引用；DTB 是 DTS 经过 dtc 编译后生成的二进制设备树文件，真正由 U-Boot 加载并传递给 Linux Kernel 使用。

---

## 16. 面试题：设备树和驱动怎么匹配？

可以答：

> 设备树节点中通常有 compatible 属性，例如 `"sony,imx307"`。驱动代码中会定义 of_device_id 匹配表，也包含相同的 compatible 字符串。内核解析设备树后，如果发现节点的 compatible 和某个驱动匹配，就会调用该驱动的 probe 函数完成设备初始化。

---

## 17. 面试题：设备树配置错会有什么表现？

可以答：

> 可能表现为设备节点没有生成、驱动 probe 失败、I2C 设备找不到、GPIO 控制异常、中断不触发、时钟或电源初始化失败等。比如摄像头设备树配置错误时，可能没有 `/dev/video*`，或者 dmesg 中出现 sensor probe failed、I2C read failed、MIPI link error 等日志。

---

## 18. 一句话总结

> **设备树就是 Linux 内核的硬件说明书，负责描述板子硬件；驱动根据设备树找到硬件并完成初始化。**

[1]: https://docs.kernel.org/translations/zh_CN/devicetree/usage-model.html?utm_source=chatgpt.com "Linux 和 Devicetree — The Linux Kernel documentation"
[2]: https://docs.kernel.org/devicetree/kernel-api.html?utm_source=chatgpt.com "DeviceTree Kernel API — The Linux Kernel documentation"
[3]: https://docs.kernel.org/devicetree/bindings/writing-bindings.html?utm_source=chatgpt.com "DOs and DON’Ts for designing and writing Devicetree bindings"
