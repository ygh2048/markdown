U-Boot 可以先用一句话理解：

> **U-Boot 是嵌入式 Linux 板子上电后，用来初始化硬件并启动 Linux 内核的 Bootloader。**

它不是 Linux 内核，也不是应用程序。它处在 **芯片上电之后、Linux 内核启动之前**。

---

# 1. 一台嵌入式板子上电后发生什么？

比如 RV1126 板子上电后，大概流程是：

```text
上电
  ↓
芯片内部 BootROM
  ↓
加载 SPL / U-Boot
  ↓
U-Boot 初始化基础硬件
  ↓
加载 Linux Kernel
  ↓
加载设备树 DTB
  ↓
挂载 rootfs
  ↓
启动 init 进程
  ↓
进入 Linux 系统
  ↓
运行你的应用程序
```

你可以把它理解为：

```text
BootROM → U-Boot → Linux Kernel → RootFS → 应用程序
```

---

# 2. U-Boot 是干什么的？

U-Boot 的核心作用有 4 个。

## 1）初始化基础硬件

Linux 内核启动前，板子不是一下子就什么都能用的。

U-Boot 会先做一些基础初始化，例如：

```text
DDR 内存初始化
时钟初始化
串口初始化
存储设备初始化，例如 eMMC、NAND、SD 卡
网络初始化，部分场景下支持 TFTP/NFS 启动
```

尤其是 **DDR 初始化** 很重要。Linux Kernel 要运行，必须先有可用内存。

---

## 2）加载 Linux 内核

U-Boot 会从存储介质里找到 Linux 内核镜像，比如：

```text
boot.img
Image
zImage
uImage
```

然后把它加载到内存中。

---

## 3）加载设备树 DTB

嵌入式 Linux 通常需要设备树。

设备树文件一般是：

```text
xxx.dtb
```

它告诉 Linux 内核：

```text
这块板子有哪些硬件
GPIO 怎么接
I2C/SPI/UART 地址是什么
摄像头挂在哪个 MIPI CSI 上
内存大小是多少
哪些外设启用
哪些驱动需要匹配
```

U-Boot 会把设备树一起传给 Linux 内核。

---

## 4）给 Linux 传启动参数

U-Boot 会给内核传一串启动参数，叫 **bootargs**。

例如：

```text
console=ttyFIQ0 root=/dev/mmcblk0p5 rw init=/sbin/init
```

这些参数告诉内核：

```text
串口控制台用哪个
根文件系统在哪里
rootfs 是只读还是可写
启动哪个 init 程序
日志级别是多少
```

---

# 3. U-Boot 和 Linux Kernel 的关系

很多初学者会混淆 U-Boot 和 Kernel。

你可以这样区分：

| 名称           | 作用                    |
| ------------ | --------------------- |
| U-Boot       | 启动 Linux 之前的引导程序      |
| Linux Kernel | 真正的操作系统内核             |
| RootFS       | Linux 系统运行时的文件系统      |
| 应用程序         | 你写的业务程序，比如采集程序、Web 服务 |

关系是：

```text
U-Boot 负责把 Linux Kernel 拉起来
Linux Kernel 负责管理 CPU、内存、驱动、进程、文件系统
RootFS 提供命令、库、配置、应用运行环境
```

一句话：

> **U-Boot 是点火器，Kernel 是发动机，RootFS 是车厢里的工具和环境。**

---

# 4. U-Boot 和 BootROM 有什么区别？

芯片上电后，最先运行的不是 U-Boot，而是芯片内部固化的 **BootROM**。

## BootROM

BootROM 是芯片厂商固化在芯片内部的一小段代码。

它通常负责：

```text
判断从哪里启动：eMMC、SD 卡、SPI Flash、USB
加载下一阶段启动程序
```

BootROM 通常不能随便改。

## U-Boot

U-Boot 是可以由开发者或厂商编译、烧录和修改的 Bootloader。

它负责更复杂的事情：

```text
初始化 DDR
加载内核
传递设备树
设置启动参数
支持升级、烧录、网络启动等
```

关系：

```text
BootROM → U-Boot → Linux Kernel
```

---

# 5. SPL 是什么？

你可能还会看到：

```text
SPL
TPL
U-Boot proper
```

先不用怕，简单理解：

> **SPL 是 U-Boot 的前置小程序，用来完成最小硬件初始化，尤其是 DDR 初始化。**

为什么需要 SPL？

因为完整 U-Boot 比较大，一开始可能没法直接加载到外部 DDR 里，因为 DDR 还没初始化。

所以先运行一个很小的 SPL：

```text
BootROM
  ↓
SPL：初始化 DDR
  ↓
完整 U-Boot：加载内核
  ↓
Linux Kernel
```

在很多 ARM 板子上，启动流程是：

```text
BootROM → SPL → U-Boot → Kernel
```

---

# 6. U-Boot 常见环境变量

U-Boot 里有一套环境变量，类似 Linux shell 里的变量。

常见的有：

```text
bootcmd
bootargs
ipaddr
serverip
ethaddr
kernel_addr
fdt_addr
```

## bootcmd

`bootcmd` 是 U-Boot 自动启动时执行的命令。

比如：

```text
bootcmd=load mmc 0:1 ${kernel_addr} Image; booti ${kernel_addr} - ${fdt_addr}
```

意思大概是：

```text
从 mmc 分区加载内核
然后启动内核
```

## bootargs

`bootargs` 是传给 Linux 内核的启动参数。

比如：

```text
bootargs=console=ttyS0,115200 root=/dev/mmcblk0p2 rw
```

意思是：

```text
串口控制台是 ttyS0
波特率 115200
rootfs 在 /dev/mmcblk0p2
以读写方式挂载
```

---

# 7. U-Boot 常见命令

如果你通过串口进入 U-Boot 命令行，可能会看到：

```text
U-Boot>
```

常用命令有：

## 查看环境变量

```bash
printenv
```

查看某个变量：

```bash
printenv bootargs
printenv bootcmd
```

## 设置环境变量

```bash
setenv bootargs 'console=ttyS0,115200 root=/dev/mmcblk0p2 rw'
```

## 保存环境变量

```bash
saveenv
```

注意：
`setenv` 只是临时修改，断电可能丢失。
`saveenv` 才会保存到存储介质。

## 启动系统

```bash
boot
```

或者：

```bash
run bootcmd
```

## 查看存储设备

```bash
mmc list
mmc dev 0
mmc part
```

## 从存储加载文件

```bash
load mmc 0:1 ${kernel_addr} Image
load mmc 0:1 ${fdt_addr} xxx.dtb
```

## 通过网络下载

```bash
tftp ${kernel_addr} Image
```

---

# 8. U-Boot 怎么启动 Linux？

常见命令有：

## ARM 64 位内核常用 booti

```bash
booti ${kernel_addr} - ${fdt_addr}
```

意思是：

```text
从 kernel_addr 启动 Linux Image
中间的 - 表示没有 initrd
fdt_addr 是设备树地址
```

## ARM 32 位可能用 bootz

```bash
bootz ${kernel_addr} - ${fdt_addr}
```

## uImage 可能用 bootm

```bash
bootm ${kernel_addr}
```

你不用现在全记住，只要知道：

```text
booti / bootz / bootm 都是 U-Boot 用来启动 Linux 内核的命令
```

---

# 9. U-Boot 和设备树有什么关系？

设备树源文件是：

```text
xxx.dts
```

编译后是：

```text
xxx.dtb
```

U-Boot 在启动 Linux 时，会把 DTB 传给 Linux Kernel。

Linux Kernel 根据 DTB 知道板子硬件情况。

比如摄像头 IMX307，如果设备树里没有正确描述，Linux 启动后可能就没有对应视频设备节点：

```text
/dev/video0
/dev/video1
```

所以你的 RV1126 + IMX307 场景里，启动链路可能涉及：

```text
U-Boot 加载 Kernel + DTB
  ↓
Kernel 根据 DTB 加载摄像头、MIPI、ISP 等驱动
  ↓
系统生成 /dev/video* 设备节点
  ↓
你的应用访问视频设备或 RTSP 流
```

---

# 10. U-Boot 和驱动有什么关系？

严格说，Linux 驱动主要在 **Kernel 阶段** 工作。

但是 U-Boot 也可能有自己的简单驱动，比如：

```text
串口驱动
网卡驱动
MMC/eMMC 驱动
显示屏驱动
USB 下载相关驱动
```

这些驱动是为了启动阶段服务的，不等同于 Linux 内核驱动。

比如：

```text
U-Boot 能读 eMMC，是为了从 eMMC 加载内核
Linux 也能读 eMMC，是因为 Linux 内核也有 eMMC 驱动
```

二者不是同一套驱动，但硬件是同一个。

---

# 11. 实际场景：板子启动不了怎么排查？

如果板子上电后起不来，你要按启动阶段分层排查。

## 场景 1：串口完全没输出

可能问题：

```text
电源问题
串口接线问题
波特率不对
BootROM 没起来
U-Boot 没烧录成功
```

排查：

```text
检查供电
检查串口 TX/RX/GND
确认波特率，例如 115200
确认启动介质
重新烧录 loader / uboot
```

---

## 场景 2：能进 U-Boot，但进不了 Linux

可能问题：

```text
kernel 镜像路径不对
dtb 路径不对
bootargs 错误
rootfs 分区错误
内核镜像损坏
```

排查：

```bash
printenv bootcmd
printenv bootargs
mmc list
mmc part
```

看它从哪里加载 Kernel 和 DTB。

---

## 场景 3：Kernel 启动了，但挂载 rootfs 失败

典型日志可能是：

```text
VFS: Unable to mount root fs
```

可能原因：

```text
bootargs 里的 root= 写错
rootfs 分区损坏
缺少文件系统驱动
分区表变化
```

重点看：

```bash
printenv bootargs
```

里面的：

```text
root=/dev/mmcblk0pX
```

是否正确。

---

## 场景 4：Linux 启动了，但没有摄像头节点

这时一般不是 U-Boot 主责，而是 Kernel/设备树/驱动问题。

排查：

```bash
ls /dev/video*
dmesg | grep -i imx307
dmesg | grep -i video
dmesg | grep -i csi
```

可能原因：

```text
设备树没有启用 sensor
驱动没编进内核
驱动模块没加载
摄像头供电/复位/时钟配置问题
MIPI CSI 配置错误
```

---

# 12. U-Boot 在 SDK 里的位置

一个嵌入式 Linux SDK 通常有：

```text
SDK/
├── u-boot/
├── kernel/
├── buildroot/
├── device/
├── tools/
└── output/
```

其中：

```text
u-boot/    负责启动引导阶段
kernel/    负责 Linux 内核和驱动
buildroot/ 负责 rootfs 和软件包
device/    负责板级配置、分区、设备树等
```

你可以把完整系统理解成：

```text
U-Boot：把内核启动起来
Kernel：驱动硬件，管理系统资源
RootFS：提供 Linux 命令、库和应用运行环境
App：你的业务程序
```

---

# 13. 面试怎么回答：U-Boot 是什么？

你可以这样答：

> U-Boot 是嵌入式系统中常用的 Bootloader，运行在 Linux 内核之前。它主要负责完成基础硬件初始化，比如 DDR、串口、存储和网络等，然后从 Flash、eMMC、SD 卡或网络加载 Linux Kernel 和设备树，并通过 bootargs 向内核传递启动参数，最后跳转到 Linux Kernel 执行。
>
> 在嵌入式 Linux SDK 中，U-Boot 通常和 Kernel、设备树、RootFS 一起组成完整启动链路。实际调试时，如果板子启动失败，可以通过串口进入 U-Boot，查看 `bootcmd`、`bootargs`、分区和启动镜像路径，判断问题是在 Bootloader 阶段、Kernel 阶段还是 RootFS 阶段。

这段可以背。

---

# 14. 面试官追问：U-Boot 和 Kernel 区别？

你可以答：

> U-Boot 是启动引导程序，主要负责在上电后初始化必要硬件并加载 Linux Kernel；Kernel 是操作系统内核，负责进程调度、内存管理、文件系统、网络协议栈和设备驱动等系统功能。U-Boot 只在启动阶段运行，Kernel 启动后系统才真正进入 Linux 运行环境。

---

# 15. 面试官追问：bootargs 是什么？

可以答：

> bootargs 是 U-Boot 传递给 Linux Kernel 的启动参数，里面通常包含串口控制台、根文件系统位置、挂载方式、日志级别等信息。比如 `console=ttyS0,115200 root=/dev/mmcblk0p2 rw`，表示内核日志从 ttyS0 输出，rootfs 挂载在 mmcblk0p2，并以读写方式挂载。

---

# 16. 面试官追问：设备树是谁加载的？

可以答：

> 设备树 DTB 通常由 U-Boot 从存储介质加载到内存，并在启动 Kernel 时传递给内核。Kernel 启动后根据设备树描述匹配平台设备和驱动，比如 I2C、SPI、UART、MIPI CSI、摄像头 sensor 等。

---

# 17. 和你简历怎么关联？

你的简历里如果写：

> 了解 U-Boot、Kernel、RootFS、设备树和驱动加载流程。

面试时可以这样解释：

> 我项目主要做 RV1126 Linux 应用层和板端联调，没有深入修改 U-Boot，但了解完整启动链路。板子上电后先由 BootROM 加载 U-Boot，U-Boot 初始化基础硬件并加载 Kernel 和设备树，Kernel 根据设备树加载驱动并生成设备节点，RootFS 提供运行环境，最后启动应用程序。我在调试 IMX307 视频链路时，主要关注 Kernel 启动后设备节点和驱动日志，比如 `/dev/video*`、`dmesg` 等。

这个说法很稳，不会显得夸大。

---

# 18. 最后记住这一张图

```text
上电
 ↓
BootROM：芯片内部固化代码，选择启动介质
 ↓
SPL：初始化 DDR 等最小硬件
 ↓
U-Boot：加载 Kernel、DTB，设置 bootargs
 ↓
Linux Kernel：驱动硬件，管理系统资源
 ↓
RootFS：提供命令、库、配置和应用环境
 ↓
你的程序：Modbus 采集、Web 服务、视频推流
```

一句话总结：

> **U-Boot 是 Linux 启动前的引导程序，负责初始化基础硬件、加载内核和设备树，并把启动参数传给内核。**


你这里应该是想问 **rootfs**，不是 `roofs`。

**rootfs = root filesystem，根文件系统。**

一句话理解：

> **rootfs 是 Linux 系统启动后看到的 `/` 根目录，里面放着系统命令、动态库、配置文件、启动脚本和应用运行环境。**

---

# 1. rootfs 在启动流程里的位置

嵌入式 Linux 板子启动大概是：

```text
上电
 ↓
BootROM
 ↓
U-Boot
 ↓
Linux Kernel
 ↓
挂载 rootfs
 ↓
启动 init 进程
 ↓
进入 Linux 系统
 ↓
运行你的应用程序
```

也就是说：

```text
Kernel 启动之后，需要 rootfs 才能真正进入 Linux 用户空间
```

没有 rootfs，Kernel 即使启动了，也没有命令、没有库、没有配置文件，系统跑不起来。

---

# 2. rootfs 里有什么？

你登录 Linux 板子后看到的这些目录，基本都属于 rootfs：

```text
/
├── bin
├── sbin
├── lib
├── usr
├── etc
├── dev
├── proc
├── sys
├── tmp
├── var
└── userdata
```

每个目录大概作用如下：

```text
/bin      基础命令，比如 ls、cp、mv、sh
/sbin     系统管理命令，比如 ifconfig、reboot、mount
/lib      系统动态库，比如 libc.so、ld-linux.so
/usr      用户程序和库，比如 /usr/bin、/usr/lib
/etc      配置文件，比如网络配置、启动脚本
/dev      设备节点，比如 /dev/video0、/dev/ttyS0
/proc     内核导出的进程和系统信息
/sys      内核导出的设备、驱动、总线信息
/tmp      临时文件
/var      日志、缓存等可变数据
/userdata 用户数据或应用部署目录
```

---

# 3. rootfs 和 Kernel 的关系

很多初学者会把 Kernel 和 rootfs 混在一起。

可以这样区分：

| 名称     | 作用                            |
| ------ | ----------------------------- |
| Kernel | Linux 内核，负责进程调度、内存管理、网络协议栈、驱动 |
| rootfs | 根文件系统，提供命令、库、配置、启动脚本和应用运行环境   |

举个例子：

你在板子上执行：

```bash
ls
```

这个 `ls` 命令本身通常来自 rootfs 的：

```text
/bin/ls
```

但 `ls` 能读取文件，是因为 Kernel 提供文件系统能力。

所以：

```text
Kernel 负责系统能力
rootfs 提供用户空间环境
```

---

# 4. rootfs 和 U-Boot 的关系

U-Boot 不直接运行你的应用程序，它主要负责启动 Kernel。

U-Boot 会通过 `bootargs` 告诉 Kernel：

```text
rootfs 在哪里
用什么方式挂载
挂载成只读还是读写
```

比如 U-Boot 的 `bootargs` 可能是：

```text
console=ttyS0,115200 root=/dev/mmcblk0p5 rw rootwait
```

这里重点是：

```text
root=/dev/mmcblk0p5
```

意思是：

> Kernel 启动后，把 `/dev/mmcblk0p5` 这个分区挂载成根文件系统 `/`。

`rw` 表示以读写方式挂载。

---

# 5. rootfs 挂载失败会怎么样？

如果 Kernel 启动了，但 rootfs 挂载失败，常见日志是：

```text
VFS: Unable to mount root fs
Kernel panic - not syncing
```

这说明：

```text
Kernel 找不到或者挂不上根文件系统
```

常见原因：

```text
bootargs 里的 root= 写错
rootfs 分区损坏
文件系统格式不支持
缺少对应存储驱动
分区表变化
rootfs 里缺少 init 程序
```

比如 `root=/dev/mmcblk0p5` 写错成了 `root=/dev/mmcblk0p6`，Kernel 就可能找不到正确的 rootfs。

---

# 6. rootfs 和 `/dev` 有什么关系？

你在项目中经常提到：

```text
/dev/video*
/dev/ttyS*
/dev/input*
```

这些叫 **设备节点**。

比如 IMX307 摄像头链路调通后，系统可能出现：

```text
/dev/video0
/dev/video1
/dev/video12
```

这些设备节点通常出现在 rootfs 的 `/dev` 目录下。

但注意：

> `/dev/video0` 不是普通文件，它是 Kernel 驱动暴露给用户空间的设备接口。

应用程序可以通过它访问底层驱动。

比如视频采集程序、GStreamer、V4L2 工具，最终都可能访问 `/dev/video*`。

---

# 7. rootfs 和动态库有什么关系？

你的应用程序运行时可能依赖很多动态库，比如：

```text
libmodbus.so
libgstreamer-1.0.so
libpthread.so
libstdc++.so
libc.so
```

这些库一般放在 rootfs 的：

```text
/lib
/usr/lib
/usr/local/lib
```

如果你的程序启动时报：

```text
error while loading shared libraries: libmodbus.so.5: cannot open shared object file
```

说明：

```text
程序需要的动态库不在 rootfs 的库路径里
```

解决方式可能是：

```bash
export LD_LIBRARY_PATH=/userdata/control_and_video/runtime/usr/lib:$LD_LIBRARY_PATH
```

或者把库放到：

```text
/usr/lib
/lib
```

所以 rootfs 不只是目录，它还决定了应用程序能不能找到运行依赖。

---

# 8. rootfs 和 sysroot 的区别

这个很重要，面试容易问。

| 名称      | 用途           | 给谁用     |
| ------- | ------------ | ------- |
| rootfs  | 板子运行时的根文件系统  | 给目标板运行用 |
| sysroot | 交叉编译时使用的目标环境 | 给编译器用   |

简单说：

```text
rootfs：板子真正启动后用的 /
sysroot：PC 交叉编译时参考的目标板头文件和库
```

两者里面可能都有：

```text
/usr/include
/usr/lib
/lib
```

但用途不同。

比如你交叉编译 RV1126 程序时，CMake 会用：

```text
RV1126_SYSROOT
```

去找目标平台的头文件和库。

程序部署到板端运行时，则依赖板端 rootfs 里的库和配置。

一句话记忆：

> **sysroot 是编译时环境，rootfs 是运行时环境。**

---

# 9. rootfs 和 Buildroot 的关系

Buildroot 可以用来生成 rootfs。

比如你在 Buildroot 里选择：

```text
BusyBox
libmodbus
GStreamer
OpenSSL
Python
```

Buildroot 构建完成后，会把这些东西放进目标 rootfs。

输出可能是：

```text
rootfs.ext4
rootfs.tar
rootfs.cpio
```

这些就是不同格式的根文件系统。

然后烧录到板子 eMMC 或 SD 卡里，Kernel 启动后挂载它。

---

# 10. rootfs 常见格式

嵌入式里 rootfs 可能以不同形式存在：

```text
ext4      常见于 eMMC、SD 卡分区
squashfs  只读压缩文件系统，常用于固件
ubifs     常用于 NAND Flash
cpio      常用于 initramfs
tar       常用于打包部署
```

你不用一开始全部掌握，先记住：

```text
rootfs 可以是一个分区，也可以是一个镜像文件
```

---

# 11. rootfs 里最关键的程序：init

Kernel 挂载 rootfs 后，会启动第一个用户空间进程，通常叫：

```text
init
```

常见路径：

```text
/sbin/init
/init
/bin/init
```

这个进程的 PID 是：

```text
1
```

它负责继续启动系统服务，比如网络、udev、应用程序、自启动脚本等。

如果 rootfs 里没有 init，Kernel 可能报：

```text
No init found
```

然后系统启动失败。

---

# 12. 嵌入式 rootfs 常见组成

嵌入式 rootfs 通常比较精简，里面可能有：

```text
BusyBox
libc
基础动态库
网络工具
启动脚本
设备节点
应用程序
配置文件
```

很多板子没有完整 Ubuntu 那么多命令，而是用 BusyBox 提供常用命令。

BusyBox 可以理解为：

> 一个小型工具集合，用一个可执行文件提供 `ls`、`cp`、`sh`、`mount` 等很多命令。

所以你在板端看到：

```bash
ls
ps
ifconfig
mount
```

很多可能都是 BusyBox 提供的。

---

# 13. 结合你的 RV1126 项目怎么理解？

你的 RV1126 板子运行时，大致依赖这些层次：

```text
U-Boot
  ↓
Kernel
  ↓
rootfs
  ↓
你的 control_and_video 程序
```

你的程序部署到：

```text
/userdata/control_and_video
```

这个 `/userdata` 也是 rootfs 挂载出来的目录或分区。

你的程序运行需要：

```text
配置文件
动态库
GStreamer 插件
启动脚本
网络环境
/dev/video* 设备节点
```

这些都和 rootfs 有关系。

比如你部署时设置：

```bash
LD_LIBRARY_PATH
GST_PLUGIN_PATH
```

就是因为 rootfs 默认库路径里找不到你打包的运行库和 GStreamer 插件。

---

# 14. 实际场景：板端程序运行不了怎么和 rootfs 关联？

## 场景 1：提示缺动态库

```text
error while loading shared libraries
```

看 rootfs 里有没有对应 `.so`：

```bash
find / -name "libmodbus*"
find / -name "libgstreamer*"
```

---

## 场景 2：命令不存在

比如：

```bash
ss: not found
```

说明 rootfs 里没有 `ss` 这个工具。

可以改用：

```bash
netstat
```

或者使用 BusyBox 里的命令。

---

## 场景 3：没有 `/dev/video*`

```bash
ls /dev/video*
```

没有结果。

可能不是 rootfs 缺文件这么简单，而是：

```text
Kernel 驱动没有加载
设备树没配好
udev/mdev 没创建设备节点
摄像头硬件没识别
```

进一步看：

```bash
dmesg | grep -i video
dmesg | grep -i imx307
```

---

## 场景 4：开机没有自动启动你的程序

可能是 rootfs 里的启动脚本没配置好。

常见位置：

```text
/etc/init.d/
```

比如你的自启动脚本可能类似：

```text
/etc/init.d/S99zz-control-and-video
```

如果它不存在、没有执行权限、脚本路径错了，开机就不会启动你的服务。

---

# 15. 面试怎么回答：rootfs 是什么？

可以这样答：

> rootfs 是 Linux 的根文件系统，也就是系统启动后挂载到 `/` 的文件系统。它提供用户空间运行环境，包括基础命令、动态库、配置文件、启动脚本、设备节点目录和应用程序等。Kernel 启动后会根据 U-Boot 传入的 bootargs 找到 rootfs 并挂载，然后启动 init 进程进入用户空间。嵌入式 Linux 中 rootfs 通常由 Buildroot 或 Yocto 构建生成。

---

# 16. 面试追问：rootfs 和 sysroot 区别？

可以这样答：

> rootfs 是目标板运行时真正挂载的根文件系统，给板子运行用；sysroot 是交叉编译时给编译器使用的目标环境，里面包含目标平台头文件和库。简单说，sysroot 是编译时用，rootfs 是运行时用。两者内容可能类似，但用途不同。

---

# 17. 面试追问：rootfs 挂载失败怎么排查？

可以这样答：

> 我会先看串口启动日志，确认 Kernel 有没有报 `VFS: Unable to mount root fs` 或 `No init found`。然后检查 U-Boot 的 `bootargs`，重点看 `root=/dev/xxx` 是否指向正确分区，文件系统类型是否正确，以及内核是否支持对应存储和文件系统驱动。如果是 init 找不到，还要检查 rootfs 中是否存在 `/sbin/init` 或 `/init`。

---

# 18. 面试追问：你的项目中 rootfs 相关问题怎么体现？

可以这样答：

> 在 RV1126 项目中，应用程序部署到板端 rootfs 的 `/userdata/control_and_video` 目录。程序运行依赖一些动态库和 GStreamer 插件，因此部署时需要一起打包 runtime，并在启动脚本里设置 `LD_LIBRARY_PATH` 和 `GST_PLUGIN_PATH`。如果程序启动失败，我会从 rootfs 运行环境角度检查文件权限、库路径、启动脚本、设备节点和日志。

---

# 19. 一句话总结

> **rootfs 是 Linux 用户空间的根目录环境，Kernel 启动后挂载 rootfs，才能运行命令、加载动态库、启动服务和执行你的应用程序。**

你可以把它记成：

```text
Kernel 负责“管理硬件和系统资源”
rootfs 负责“提供用户空间运行环境”
```
先用一句话区分：

> **V4L2 是 Linux 访问摄像头/视频采集设备的底层接口；GStreamer 是用户态搭视频处理流水线的多媒体框架。**

你可以理解为：

```text
IMX307 摄像头
   ↓
MIPI CSI / ISP / 驱动
   ↓
V4L2：Linux 暴露出来的视频设备接口，比如 /dev/video0
   ↓
GStreamer：用 v4l2src/编码器/推流插件搭建视频采集、编码、推流管道
   ↓
RTSP / RTMP / WebRTC / MediaMTX / 浏览器
```

你的项目修改记录里也出现过 RV1126、IMX307、GStreamer 插件、RTMP、WebRTC、MediaMTX、`/dev/video*`、`dmesg` 等联调内容，所以这两个知识点和你的简历项目是强相关的。

---

# 1. V4L2 是什么？

**V4L2 全称是 Video4Linux2。**

它是 Linux 内核提供的一套 **视频设备访问接口**。

简单说：

> Linux 下摄像头、采集卡、视频输入设备，通常会通过 V4L2 暴露给用户程序。

比如板子识别到摄像头后，系统里可能出现：

```bash
/dev/video0
/dev/video1
/dev/video12
```

这些 `/dev/video*` 就是 V4L2 设备节点。

---

# 2. V4L2 解决什么问题？

假设你有一个 IMX307 摄像头。

摄像头本身接在 RV1126 的 MIPI CSI 接口上，中间还有 ISP、sensor 驱动、video 驱动。

你的应用程序不可能直接操作摄像头寄存器。

Linux 会把底层复杂驱动封装成统一接口，也就是 V4L2。

应用程序只需要操作：

```text
/dev/video0
```

就可以完成：

```text
打开摄像头
设置分辨率
设置像素格式
设置帧率
申请缓冲区
开始采集
读取视频帧
停止采集
```

---

# 3. V4L2 是内核层还是应用层？

V4L2 既涉及内核，也涉及用户态。

可以这样理解：

```text
内核态：
    摄像头 sensor 驱动
    MIPI CSI 驱动
    ISP 驱动
    V4L2 框架
    /dev/video* 设备节点

用户态：
    应用程序
    GStreamer
    ffmpeg
    v4l2-ctl
```

用户程序通过系统调用和 `ioctl` 访问 V4L2 设备。

---

# 4. V4L2 常见操作流程

如果你自己写 C 程序采集摄像头，大概流程是：

```text
open("/dev/video0")
   ↓
ioctl 查询设备能力
   ↓
设置视频格式，例如 1920x1080、NV12、YUYV
   ↓
申请 buffer
   ↓
mmap 映射 buffer
   ↓
VIDIOC_STREAMON 开始采集
   ↓
循环取帧
   ↓
VIDIOC_STREAMOFF 停止采集
   ↓
close
```

你不一定要手写，但要知道 V4L2 本质上是这样工作的。

---

# 5. 常用 V4L2 调试命令

嵌入式 Linux 面试里很常用。

查看视频设备：

```bash
ls /dev/video*
```

查看设备信息：

```bash
v4l2-ctl --device=/dev/video0 --all
```

查看支持的格式：

```bash
v4l2-ctl --device=/dev/video0 --list-formats-ext
```

测试抓一帧或多帧：

```bash
v4l2-ctl --device=/dev/video0 --stream-mmap --stream-count=100
```

查看内核日志：

```bash
dmesg | grep -i video
dmesg | grep -i imx307
dmesg | grep -i csi
```

---

# 6. GStreamer 是什么？

**GStreamer 是一个多媒体处理框架。**

它运行在用户态，主要用来处理：

```text
音频采集
视频采集
编码
解码
封装
推流
播放
转码
录制
```

一句话理解：

> GStreamer 就是用很多插件拼成一条视频处理流水线。

比如你要从摄像头采集画面、编码成 H.264、推送到 RTMP 服务器，就可以用 GStreamer 管道完成。

---

# 7. GStreamer 的核心概念：Pipeline

GStreamer 最重要的概念是 **pipeline，流水线**。

例如：

```text
视频源 → 格式转换 → 编码器 → 封装器 → 推流输出
```

对应 GStreamer 可能是：

```bash
v4l2src device=/dev/video0 ! videoconvert ! x264enc ! flvmux ! rtmpsink location=rtmp://...
```

这里每一段叫一个 **element，元素/插件**。

比如：

| GStreamer 元素   | 作用                               |
| -------------- | -------------------------------- |
| `v4l2src`      | 从 V4L2 设备读取视频                    |
| `videoconvert` | 视频格式转换                           |
| `x264enc`      | 软件 H.264 编码                      |
| `mpph264enc`   | Rockchip 硬件 H.264 编码，具体是否存在看板端环境 |
| `flvmux`       | 封装成 FLV                          |
| `rtmpsink`     | 推 RTMP 流                         |
| `rtspsrc`      | 拉 RTSP 流                         |
| `decodebin`    | 自动解码                             |
| `appsink`      | 把视频帧交给应用程序                       |

---

# 8. V4L2 和 GStreamer 的关系

它们不是同一个层级。

```text
V4L2：底层视频设备接口
GStreamer：上层多媒体处理框架
```

GStreamer 可以通过 `v4l2src` 使用 V4L2。

也就是说：

```text
GStreamer 的 v4l2src 插件
        ↓
访问 /dev/video0
        ↓
底层走 V4L2
        ↓
驱动访问摄像头硬件
```

所以：

> **V4L2 负责“从摄像头拿帧”；GStreamer 负责“拿到帧之后怎么处理、编码、推流”。**

---

# 9. 结合你的 IMX307 + RV1126 项目

你的链路可以这样讲：

```text
IMX307 摄像头
   ↓
RV1126 MIPI CSI / ISP
   ↓
Linux 摄像头驱动
   ↓
V4L2 设备节点 /dev/video*
   ↓
GStreamer 或板端视频服务采集视频
   ↓
编码为 H.264
   ↓
RTMP 推送到 PC 端 MediaMTX
   ↓
MediaMTX 转 WebRTC/WHEP
   ↓
浏览器播放
```

如果你是直接从摄像头采集，可能类似：

```bash
v4l2src device=/dev/video0 ! videoconvert ! mpph264enc ! flvmux ! rtmpsink location=rtmp://PC_IP:1935/live/cam
```

如果你是从板端已有 RTSP 流转推，可能类似：

```text
rtspsrc → 解码/转码 → H.264 编码 → FLV 封装 → RTMP 推流
```

---

# 10. V4L2 和 GStreamer 面试怎么回答？

如果面试官问：**V4L2 是什么？**

你可以答：

> V4L2 是 Linux 下的视频设备访问框架，主要用于摄像头、采集卡等视频设备。驱动层通过 V4L2 框架向用户态暴露 `/dev/video*` 设备节点，应用程序可以通过 open、ioctl、mmap 等方式设置格式、申请缓冲区并采集视频帧。在嵌入式 Linux 中，摄像头 sensor、MIPI CSI、ISP 等链路最终通常会通过 V4L2 设备节点给用户态使用。

如果面试官问：**GStreamer 是什么？**

你可以答：

> GStreamer 是一个用户态多媒体框架，可以通过插件化 pipeline 实现视频采集、格式转换、编码、封装、推流和播放。比如可以用 `v4l2src` 从 V4L2 设备采集视频，用编码器插件进行 H.264 编码，再通过 `rtmpsink` 推流到 MediaMTX。它的优势是模块化强，适合快速搭建视频处理链路。

如果问：**V4L2 和 GStreamer 有什么区别？**

你可以答：

> V4L2 更偏底层，是 Linux 内核提供的视频设备接口，主要负责让用户态访问摄像头设备；GStreamer 更偏上层，是多媒体处理框架，负责把视频采集、编码、封装、推流等模块串成 pipeline。GStreamer 可以通过 `v4l2src` 调用 V4L2 设备，所以二者是上下游关系，不是替代关系。

---

# 11. 实际排查场景：摄像头没有画面

你可以按层次排查：

## 第一步：看设备节点

```bash
ls /dev/video*
```

如果没有 `/dev/video*`，可能是驱动、设备树、摄像头硬件链路问题。

---

## 第二步：看内核日志

```bash
dmesg | grep -i imx307
dmesg | grep -i video
dmesg | grep -i csi
```

看 sensor 有没有识别、驱动有没有 probe 成功。

---

## 第三步：看 V4L2 能力

```bash
v4l2-ctl --device=/dev/video0 --all
v4l2-ctl --device=/dev/video0 --list-formats-ext
```

确认设备支持哪些分辨率和格式。

---

## 第四步：测试采集

```bash
v4l2-ctl --device=/dev/video0 --stream-mmap --stream-count=100
```

如果这里失败，说明 V4L2 采集层还没通。

---

## 第五步：测试 GStreamer 管道

```bash
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! fakesink
```

如果这个能跑，说明采集链路基本通。

再测试编码推流：

```bash
gst-launch-1.0 v4l2src device=/dev/video0 ! videoconvert ! x264enc ! flvmux ! rtmpsink location=rtmp://PC_IP:1935/live/cam
```

实际 RV1126 上可能优先用 Rockchip 硬编码插件，比如 `mpph264enc`，但要看板端 GStreamer 插件是否安装。

---

# 12. 你需要记住的核心区别

```text
V4L2：
    Linux 视频设备接口
    关注 /dev/video*
    关注驱动、设备节点、格式、buffer、采集

GStreamer：
    多媒体处理框架
    关注 pipeline
    关注采集、转换、编码、封装、推流、播放

二者关系：
    GStreamer 可以通过 v4l2src 读取 V4L2 设备
```

一句话总结：

> **V4L2 管“摄像头怎么给 Linux 输出视频帧”，GStreamer 管“拿到视频帧后怎么处理和推流”。**


下面讲 **Kernel，也就是 Linux 内核**。

一句话理解：

> **Kernel 是 Linux 系统的核心，负责管理硬件资源，并向上层应用提供统一的系统能力。**

你可以把整个嵌入式 Linux 系统理解成：

```text
U-Boot  →  Kernel  →  RootFS  →  应用程序
```

其中：

```text
U-Boot：负责把 Linux 启动起来
Kernel：负责管理硬件和系统资源
RootFS：提供命令、库、配置、启动脚本
应用程序：你的业务逻辑，比如 Modbus 采集、视频推流、Web 服务
```

---

# 1. Kernel 到底负责什么？

Linux Kernel 主要负责这些事情：

```text
进程管理
内存管理
文件系统
网络协议栈
设备驱动
中断管理
系统调用
权限与资源隔离
```

你平时在应用层写程序，比如：

```cpp
open("/dev/video0", ...)
read(...)
write(...)
socket(...)
bind(...)
listen(...)
```

这些函数最后都会进入 Kernel，由 Kernel 去完成真正的硬件和系统资源操作。

---

# 2. 为什么需要 Kernel？

假设没有 Kernel，应用程序想用摄像头、网卡、串口、内存、文件，就要直接操作硬件寄存器。

这会非常复杂，也很危险。

Kernel 的作用就是把底层硬件封装起来，给应用程序提供统一接口。

比如：

| 你在应用层做的事         | Kernel 背后做的事           |
| ---------------- | ---------------------- |
| 读文件              | 通过文件系统驱动访问存储设备         |
| 开 socket         | 调用 TCP/IP 协议栈和网卡驱动     |
| 访问 `/dev/video0` | 调用 V4L2、摄像头、ISP、CSI 驱动 |
| 访问串口             | 调用 UART 驱动             |
| 创建线程             | 调用调度器分配 CPU 时间         |
| 申请内存             | 调用内存管理模块分配虚拟内存         |

所以可以说：

> **Kernel 是应用程序和硬件之间的中间层。**

---

# 3. Kernel 在启动流程中的位置

嵌入式 Linux 启动流程大概是：

```text
上电
 ↓
BootROM
 ↓
SPL / U-Boot
 ↓
U-Boot 加载 Kernel 镜像和 DTB
 ↓
Kernel 启动
 ↓
Kernel 解析设备树
 ↓
Kernel 初始化驱动
 ↓
Kernel 挂载 rootfs
 ↓
Kernel 启动 init 进程
 ↓
进入用户空间
 ↓
运行你的应用程序
```

所以 Kernel 是在 U-Boot 之后启动的。

U-Boot 做的是：

```text
加载 Kernel
加载设备树 DTB
设置 bootargs
跳转到 Kernel
```

Kernel 做的是：

```text
初始化 CPU、内存、驱动、文件系统、网络
挂载 rootfs
启动用户空间
```

---

# 4. Kernel 和 RootFS 的区别

这个很重要。

| 名称     | 作用                     |
| ------ | ---------------------- |
| Kernel | 操作系统内核，负责管理硬件和系统资源     |
| RootFS | 根文件系统，提供命令、库、配置和应用运行环境 |

比如你执行：

```bash
ls
```

`ls` 这个命令本身来自 rootfs：

```text
/bin/ls
```

但是 `ls` 能读取目录，是因为 Kernel 提供文件系统能力。

所以：

```text
Kernel 负责能力
RootFS 提供环境
```

一句话：

> **Kernel 是发动机，RootFS 是车上的工具和空间。**

---

# 5. Kernel 和驱动是什么关系？

驱动是 Kernel 的一部分，或者以模块形式加载进 Kernel。

比如你的 RV1126 + IMX307 视频链路里，可能涉及：

```text
IMX307 sensor 驱动
I2C 驱动
MIPI CSI 驱动
ISP 驱动
V4L2 框架
网卡驱动
UART 驱动
```

这些都属于 Kernel 层能力。

它们的作用是：

```text
把具体硬件封装成 Linux 能识别的设备
```

比如摄像头驱动加载成功后，用户空间可能看到：

```bash
/dev/video0
/dev/video1
/dev/video12
```

然后你的应用或 GStreamer 才能访问视频设备。

你的项目里 RV1126、IMX307、GStreamer、MediaMTX、WebRTC 和设备节点排查是强相关的，所以面试时可以把 Kernel 讲到“驱动加载后生成 `/dev/video*`，应用层通过 V4L2/GStreamer 使用摄像头”这一层。

---

# 6. Kernel 和设备树是什么关系？

设备树描述硬件，Kernel 解析设备树。

比如设备树里写：

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

Kernel 启动后会解析这个节点。

如果 Kernel 里有 IMX307 驱动，并且驱动里的 compatible 也是：

```c
.compatible = "sony,imx307"
```

那 Kernel 就会匹配设备和驱动，然后调用驱动的 `probe()` 函数。

流程是：

```text
U-Boot 传入 DTB
 ↓
Kernel 解析设备树
 ↓
找到 imx307 节点
 ↓
根据 compatible 匹配驱动
 ↓
调用驱动 probe
 ↓
初始化摄像头
 ↓
注册 V4L2 设备
 ↓
生成 /dev/video*
```

---

# 7. 用户态和内核态

Linux 里有两个重要概念：

```text
用户态 user space
内核态 kernel space
```

## 用户态

你的应用程序运行在用户态，比如：

```text
Modbus 采集程序
Web 服务
视频推流程序
Shell 脚本
GStreamer
MediaMTX
```

用户态程序不能直接操作硬件，必须通过 Kernel 提供的接口。

## 内核态

Kernel 和驱动运行在内核态。

内核态权限更高，可以直接管理：

```text
CPU
内存
中断
硬件寄存器
设备驱动
文件系统
网络协议栈
```

所以：

```text
应用程序在用户态
驱动和内核在内核态
```

---

# 8. 用户程序怎么进入 Kernel？

通过 **系统调用 syscall**。

比如：

```cpp
open("/dev/video0", O_RDWR);
```

表面上你调用的是 C 库函数 `open()`。

实际上最终会进入 Kernel，让 Kernel 帮你打开设备文件。

常见系统调用有：

```text
open
read
write
close
ioctl
mmap
socket
bind
listen
accept
connect
send
recv
fork
exec
```

比如 V4L2 摄像头采集就会大量用到：

```text
open
ioctl
mmap
read
```

网络通信会用到：

```text
socket
bind
listen
accept
send
recv
```

---

# 9. Kernel 负责进程管理

你在 Linux 里运行程序：

```bash
./control_and_video
```

Kernel 会为它创建进程。

你可以用：

```bash
ps
top
pidof control_and_video
```

查看进程。

Kernel 会负责：

```text
给进程分配 PID
分配虚拟内存
调度 CPU 时间
管理进程状态
处理进程退出
```

比如你用：

```bash
kill <pid>
```

本质上也是给进程发送信号，由 Kernel 处理。

---

# 10. Kernel 负责内存管理

应用程序不能随便访问物理内存。

Kernel 会给每个进程提供独立的虚拟地址空间。

这有几个好处：

```text
进程之间互不干扰
防止一个程序乱写内存破坏系统
支持内存分页和缓存
支持 mmap
```

比如视频采集里常见的 `mmap`，就是把 Kernel 分配的视频 buffer 映射到用户态，这样应用程序可以高效读取摄像头帧数据。

---

# 11. Kernel 负责文件系统

Linux 里“一切皆文件”。

比如：

```text
普通文件：/etc/profile
设备文件：/dev/video0
进程信息：/proc/1234/status
系统设备：/sys/class/video4linux
```

你访问这些路径，背后都是 Kernel 在处理。

常见虚拟文件系统：

```text
/proc  进程和内核运行状态
/sys   设备、驱动、总线信息
/dev   设备节点
```

比如排查摄像头时：

```bash
ls /dev/video*
ls /sys/class/video4linux
dmesg | grep -i video
```

这些都是在观察 Kernel 暴露出来的信息。

---

# 12. Kernel 负责网络协议栈

你的 Modbus TCP 程序使用 TCP socket。

应用层看到的是：

```cpp
socket()
connect()
send()
recv()
```

Kernel 背后处理：

```text
TCP 三次握手
TCP 重传
拥塞控制
IP 路由
ARP
网卡驱动收发包
端口监听
连接状态维护
```

所以你用：

```bash
ss -lntp
```

能看到端口监听。

比如：

```bash
ss -lntp | grep 8080
ss -lntp | grep 502
```

这些信息来自 Kernel 网络栈。

---

# 13. Kernel 负责设备驱动

设备驱动是 Kernel 和硬件之间的桥梁。

比如：

```text
网卡驱动 → 提供 eth0
串口驱动 → 提供 /dev/ttyS*
摄像头驱动 → 提供 /dev/video*
GPIO 驱动 → 提供 GPIO 控制接口
I2C 驱动 → 让 sensor 驱动能访问 I2C 设备
```

应用程序一般不直接关心寄存器地址，而是通过 Linux 统一接口访问设备。

比如：

```bash
ls /dev/video*
```

看到视频节点，说明 Kernel 层 V4L2 设备注册成功。

---

# 14. Kernel 模块是什么？

Kernel 模块一般是 `.ko` 文件。

比如：

```text
imx307.ko
usbnet.ko
某个网卡驱动.ko
```

它可以在系统运行时加载进内核。

常用命令：

```bash
lsmod
insmod xxx.ko
rmmod xxx
modprobe xxx
```

区别：

```text
insmod：直接加载指定 .ko 文件
modprobe：加载模块时会自动处理依赖
rmmod：卸载模块
lsmod：查看已加载模块
```

如果驱动是编进 Kernel 的，就不一定能在 `lsmod` 里看到。

---

# 15. Kernel 日志 dmesg

`dmesg` 是嵌入式调试非常重要的命令。

它查看 Kernel 日志。

比如：

```bash
dmesg | tail
dmesg | grep -i imx307
dmesg | grep -i video
dmesg | grep -i i2c
dmesg | grep -i csi
```

你可以通过 dmesg 看：

```text
驱动有没有 probe 成功
I2C 是否读写失败
摄像头是否识别
设备节点是否注册
网卡是否初始化
文件系统是否挂载失败
```

面试里你说会结合 `dmesg` 排查板端问题，是非常加分的。

---

# 16. Kernel 常见排查场景

## 场景 1：板子启动卡住

看串口日志，判断卡在哪一层：

```text
U-Boot 阶段？
Kernel 阶段？
RootFS 挂载阶段？
init 启动阶段？
```

如果出现：

```text
VFS: Unable to mount root fs
```

说明 Kernel 找不到或挂不上 rootfs。

---

## 场景 2：没有 `/dev/video*`

排查：

```bash
ls /dev/video*
dmesg | grep -i imx307
dmesg | grep -i video
dmesg | grep -i csi
```

可能原因：

```text
设备树没配好
驱动没编进 Kernel
驱动模块没加载
sensor probe 失败
MIPI/ISP 链路没注册成功
```

---

## 场景 3：网口不通

排查：

```bash
ip addr
ip route
dmesg | grep -i eth
dmesg | grep -i link
ss -lntp
ping <ip>
```

可能原因：

```text
网卡驱动没起来
IP 配置错误
路由错误
网线或交换机问题
服务没监听端口
```

---

## 场景 4：串口没有数据

排查：

```bash
ls /dev/ttyS*
ls /dev/ttyUSB*
dmesg | grep -i tty
stty -F /dev/ttyS1 -a
```

可能原因：

```text
设备节点不对
波特率不对
串口复用没配好
设备树 pinctrl 错误
接线 TX/RX 反了
```

---

# 17. Kernel 和你的项目怎么关联？

你的项目可以这样讲：

> 我项目主要做 RV1126 Linux 用户态应用开发和板端联调。Kernel 层我主要关注驱动加载和设备节点状态，比如 IMX307 摄像头链路是否生成 `/dev/video*`，网络端口是否监听，串口和视频设备是否正常。遇到问题时，会结合 `dmesg`、`/proc`、`/sys`、设备节点、进程状态和应用日志判断是应用层问题、运行时依赖问题，还是 Kernel/驱动/设备树层问题。

这样讲非常稳，因为你没有夸大说自己写了内核驱动，但体现你理解 Kernel 和实际排查。

---

# 18. 面试题：Kernel 是什么？

可以这样答：

> Kernel 是 Linux 操作系统的核心，运行在内核态，负责进程调度、内存管理、文件系统、网络协议栈、设备驱动和系统调用等功能。应用程序运行在用户态，不能直接操作硬件，需要通过系统调用进入内核，由 Kernel 完成硬件访问和资源管理。在嵌入式 Linux 中，Kernel 启动后会解析设备树、初始化驱动、挂载 rootfs，并启动 init 进入用户空间。

---

# 19. 面试题：Kernel 和 U-Boot 区别？

可以答：

> U-Boot 是 Bootloader，运行在 Linux Kernel 之前，负责基础硬件初始化、加载 Kernel 和设备树，并传递 bootargs。Kernel 是操作系统内核，启动后负责驱动硬件、管理进程和内存、提供文件系统和网络协议栈等系统能力。简单说，U-Boot 负责把系统引导起来，Kernel 负责系统真正运行。

---

# 20. 面试题：Kernel 和 RootFS 区别？

可以答：

> Kernel 是操作系统核心，负责资源管理和硬件驱动；RootFS 是根文件系统，提供用户空间环境，包括命令、动态库、配置文件、启动脚本和应用程序。Kernel 启动后会挂载 RootFS，然后启动 init 进程进入用户空间。

---

# 21. 面试题：用户态和内核态区别？

可以答：

> 用户态是普通应用程序运行的空间，权限较低，不能直接访问硬件；内核态是 Kernel 和驱动运行的空间，权限高，可以管理硬件和系统资源。用户态程序通过系统调用进入内核态，比如 open、read、write、ioctl、socket 等。

---

# 22. 面试题：驱动加载失败怎么排查？

可以答：

> 我会先看设备节点是否生成，比如 `/dev/video*`；然后用 `dmesg` 查看驱动 probe 日志，关注 I2C、GPIO、clock、regulator、MIPI 等错误；再检查设备树节点是否启用、compatible 是否和驱动匹配、驱动是否编进内核或模块是否加载。如果是模块驱动，还会用 `lsmod`、`insmod`、`modprobe` 查看加载状态。

---

# 23. 一句话总结

> **Kernel 是 Linux 的核心，负责管理硬件、驱动、进程、内存、文件系统和网络；应用程序通过系统调用使用 Kernel 提供的能力。**
