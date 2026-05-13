# ADB ssh
可以。你项目里的部署可以理解成一句话：

> **在 PC 上交叉编译出 RV1126 能运行的程序，然后通过 SSH 或 ADB 把程序、配置文件、依赖库传到板子上，再在板端启动并验证服务是否正常。**

你的修改记录里确实维护了两条部署链路：`deploy_rv1126_ssh.sh` 和 `deploy_rv1126_adb.sh`，还包括交叉构建、运行库打包、板端启动、健康检查、自启动等内容。

---

# 1. 先理解部署流程

不管是 **SSH 部署** 还是 **ADB 部署**，整体流程都差不多：

```text
PC 端交叉编译
    ↓
生成 RV1126 可执行程序
    ↓
打包程序、配置文件、依赖库、启动脚本
    ↓
传输到 RV1126 板端
    ↓
板端解压 / 设置权限 / 配置环境变量
    ↓
停止旧进程
    ↓
启动新程序
    ↓
检查 /health、进程、端口、日志
```

区别只在于中间“传输和执行命令”的方式不同：

```text
SSH 部署：scp 传文件 + ssh 远程执行命令
ADB 部署：adb push 传文件 + adb shell 执行命令
```

---

# 2. SSH 部署是什么？

SSH 是 Linux 里最常见的远程登录方式。

比如你的 RV1126 板子 IP 是：

```text
192.168.101.5
```

你可以在 PC 上登录：

```bash
ssh root@192.168.101.5
```

传文件可以用：

```bash
scp app root@192.168.101.5:/userdata/control_and_video/
```

远程执行命令可以用：

```bash
ssh root@192.168.101.5 "ps | grep control_and_video"
```

所以 SSH 部署本质就是：

> **通过网络登录板子，把文件传过去，再远程启动程序。**

---

# 3. 你的项目里 SSH 部署怎么做？

简化版流程是这样：

```bash
source scripts/setup_rv1126_local_env.sh
./scripts/build_rv1126.sh
./scripts/deploy_rv1126_ssh.sh --host 192.168.101.5 --user root --remote-dir /userdata/control_and_video
```

这几步分别是什么意思？

## 第一步：准备交叉编译环境

```bash
source scripts/setup_rv1126_local_env.sh
```

它的作用一般是设置这些环境变量：

```bash
RV1126_TOOLCHAIN_PREFIX=...
RV1126_SYSROOT=...
PATH=...
```

也就是告诉系统：

> 我要用 RV1126 的 AArch64 交叉编译器和 sysroot。

---

## 第二步：交叉编译

```bash
./scripts/build_rv1126.sh
```

这一步会用交叉编译工具链生成 RV1126 板端可运行程序。

编译完通常会得到类似：

```text
build_rv1126/backend/cpp/control_and_video
```

然后可以用：

```bash
file build_rv1126/backend/cpp/control_and_video
```

或者：

```bash
readelf -h build_rv1126/backend/cpp/control_and_video
```

确认它是不是：

```text
Machine: AArch64
```

如果是 `x86-64`，说明交叉编译没生效。

---

## 第三步：通过 SSH 部署

```bash
./scripts/deploy_rv1126_ssh.sh --host 192.168.101.5 --user root --remote-dir /userdata/control_and_video
```

这个脚本大概会做这些事情：

```text
1. 检查本地构建产物是否存在
2. 打包可执行程序、配置文件、依赖库、启动脚本
3. 通过 scp 上传到 RV1126
4. 通过 ssh 登录板端
5. 创建 /userdata/control_and_video 目录
6. 解压部署包
7. chmod +x 设置执行权限
8. 停止旧的 control_and_video 进程
9. 启动新的程序
10. 通过 /health 接口验证服务是否正常
```

---

# 4. SSH 部署时板端通常执行什么？

比如脚本传完文件后，板端可能执行类似命令：

```bash
mkdir -p /userdata/control_and_video
tar -xf package.tar -C /userdata/control_and_video
chmod +x /userdata/control_and_video/backend/cpp/control_and_video
```

停止旧进程：

```bash
ps | grep control_and_video
kill <pid>
```

设置运行环境：

```bash
export LD_LIBRARY_PATH=/userdata/control_and_video/runtime/usr/lib:$LD_LIBRARY_PATH
export GST_PLUGIN_PATH=/userdata/control_and_video/runtime/usr/lib/gstreamer-1.0
```

启动程序：

```bash
cd /userdata/control_and_video
./scripts/start_embedded.sh
```

验证：

```bash
curl http://192.168.101.5:8080/health
```

如果返回：

```json
{"status":"ok"}
```

说明服务正常。

---

# 5. ADB 部署是什么？

ADB 全称是 Android Debug Bridge。

虽然它常见于 Android，但很多嵌入式板卡也会提供 ADB 调试能力。

ADB 部署不依赖 SSH 服务。

它常用命令是：

```bash
adb devices
adb push
adb shell
```

比如查看设备：

```bash
adb devices
```

传文件：

```bash
adb push app /userdata/control_and_video/
```

进入板端 shell：

```bash
adb shell
```

远程执行命令：

```bash
adb shell "ps | grep control_and_video"
```

所以 ADB 部署本质是：

> **通过 ADB 通道把文件推到板子上，然后通过 adb shell 在板子上执行命令。**

---

# 6. 你的项目里 ADB 部署怎么做？

你的修改记录里提到新增了：

```text
scripts/deploy_rv1126_adb.sh
```

它是用来替代 SSH/SCP 依赖的部署方式。也就是说，如果板子没有开 SSH，或者网络环境不方便用 SSH，可以用 ADB 部署。

典型流程类似：

```bash
adb devices
./scripts/deploy_rv1126_adb.sh --remote-dir /userdata/control_and_video
```

如果是 TCP ADB，可能先连接：

```bash
adb connect 192.168.101.5:5555
adb devices
```

然后部署：

```bash
./scripts/deploy_rv1126_adb.sh --host 192.168.101.5 --remote-dir /userdata/control_and_video
```

脚本内部大概会做：

```text
1. 检查 adb 是否可用
2. 检查是否连接到 RV1126
3. 本地交叉编译或使用已有产物
4. 打包程序、配置文件、依赖库
5. adb push 上传到板端
6. adb shell 解压、设置权限
7. adb shell 停止旧进程
8. adb shell 启动新进程
9. 可选安装 init.d 自启动脚本
10. 检查 /health 或进程状态
```

---

# 7. SSH 和 ADB 的区别

| 对比项  | SSH 部署          | ADB 部署                     |
| ---- | --------------- | -------------------------- |
| 连接方式 | 网络 SSH          | USB ADB 或 TCP ADB          |
| 传文件  | `scp`           | `adb push`                 |
| 远程命令 | `ssh "command"` | `adb shell "command"`      |
| 依赖   | 板端需要 sshd       | 板端需要 adb 服务                |
| 常见场景 | 标准 Linux 板端部署   | 调试板、Android/Linux SDK 板端调试 |
| 优点   | 标准、通用、稳定        | 不依赖 SSH，适合调试环境             |
| 缺点   | 需要网络和 SSH 服务    | 需要 ADB 通道正常                |

你面试时可以这样说：

> 项目中支持 SSH 和 ADB 两种部署方式。SSH 方式通过 `scp` 上传部署包，再通过 `ssh` 在板端解压、设置权限、重启服务并验证健康接口；ADB 方式通过 `adb push` 上传文件，再通过 `adb shell` 执行板端部署命令，适合 SSH 不可用或调试阶段使用。

---

# 8. 为什么部署时要打包依赖库？

因为你的程序可能依赖：

```text
libmodbus.so
libgstreamer-1.0.so
GStreamer 插件
其他 runtime 动态库
```

PC 上编译时能找到这些库，不代表板端运行时也能找到。

所以部署时通常要把运行库一起传到板端，比如：

```text
/userdata/control_and_video/
├── backend/cpp/control_and_video
├── config/runtime.conf
├── scripts/start_embedded.sh
└── runtime/
    └── usr/lib/
        ├── libmodbus.so.5
        ├── libgstreamer-1.0.so
        └── gstreamer-1.0/
```

启动前设置：

```bash
export LD_LIBRARY_PATH=/userdata/control_and_video/runtime/usr/lib:$LD_LIBRARY_PATH
export GST_PLUGIN_PATH=/userdata/control_and_video/runtime/usr/lib/gstreamer-1.0
```

否则板端可能报：

```text
error while loading shared libraries: libmodbus.so.5: cannot open shared object file
```

或者：

```text
no element "x264enc"
no element "v4l2src"
```

这就是典型的运行时依赖或插件路径问题。

---

# 9. 为什么部署后要做健康检查？

因为“传上去了”不等于“运行正常”。

部署完成后要验证几个层次：

## 1. 进程是否存在

```bash
ps | grep control_and_video
```

## 2. 端口是否监听

```bash
ss -lntp | grep 8080
```

## 3. 健康接口是否正常

```bash
curl http://192.168.101.5:8080/health
```

返回：

```json
{"status":"ok"}
```

## 4. 日志是否有错误

```bash
tail -f /userdata/control_and_video/logs/one_click_app.log
```

或者：

```bash
dmesg | tail
```

所以部署脚本最后做 `/health` 验证，是为了确认服务真正起来了。

---

# 10. 你可以在面试里怎么讲？

如果面试官问：

**你说你用 ADB/SSH 部署 RV1126 程序，具体怎么做的？**

你可以这样回答：

> 我项目里有两套板端部署方式，分别是 SSH 部署和 ADB 部署。整体流程是先在 PC 端使用 RV1126 的 AArch64 交叉编译工具链编译程序，然后把可执行文件、配置文件、运行依赖库和启动脚本打包。
>
> SSH 部署时，通过 `scp` 把部署包上传到 RV1126 的 `/userdata/control_and_video` 目录，再通过 `ssh` 在板端执行解压、权限设置、旧进程清理和服务启动。ADB 部署时，则使用 `adb push` 上传文件，通过 `adb shell` 执行相同的板端命令。
>
> 启动时会配置 `LD_LIBRARY_PATH` 和 GStreamer 插件路径，避免动态库或插件找不到。部署完成后，我会通过 `ps`、`ss`、`curl /health`、运行日志和 `dmesg` 检查进程、端口、服务状态和底层设备状态，确认程序是否真正运行正常。

这段就很贴你简历和岗位。

---

# 11. 一句话总结

```text
SSH 部署 = scp 上传 + ssh 执行命令
ADB 部署 = adb push 上传 + adb shell 执行命令
```

它们最终做的事情是一样的：

```text
传程序 → 解压 → 设置权限 → 配环境变量 → 停旧进程 → 启新进程 → 健康检查
```


你说的应该是 **`dmesg`**，不是 `dmsg`。

一句话理解：

> **`dmesg` 是用来查看 Linux 内核日志的命令，主要用于排查驱动、设备识别、硬件初始化、内核报错等问题。**

在嵌入式 Linux 面试和板端调试里，`dmesg` 非常重要。

---

# 1. `dmesg` 看的是谁的日志？

Linux 系统里大致有两类日志：

```text
应用日志：你的程序自己打印的日志
内核日志：Kernel 和驱动打印的日志
```

`dmesg` 看的是 **内核日志**。

比如这些信息通常会出现在 `dmesg` 里：

```text
内核启动日志
驱动加载日志
设备 probe 成功/失败
USB 插拔日志
网卡初始化日志
摄像头识别日志
I2C/SPI/UART 错误
文件系统挂载错误
内核 panic / oops
```

你项目里如果调 **RV1126 + IMX307 摄像头**，`dmesg` 就可以用来查看：

```text
IMX307 有没有识别
I2C 通信有没有失败
MIPI CSI 有没有报错
V4L2 设备有没有注册
/dev/video* 设备节点为什么没出来
```

---

# 2. 最常用命令

## 查看全部内核日志

```bash
dmesg
```

输出会很多。

---

## 看最后几十行

```bash
dmesg | tail
```

或者：

```bash
dmesg | tail -n 50
```

常用于刚插设备、刚启动服务、刚加载驱动后看最新日志。

---

## 按关键词筛选

比如查摄像头：

```bash
dmesg | grep -i imx307
```

查视频相关：

```bash
dmesg | grep -i video
```

查 V4L2：

```bash
dmesg | grep -i v4l2
```

查 I2C：

```bash
dmesg | grep -i i2c
```

查 MIPI / CSI：

```bash
dmesg | grep -i csi
dmesg | grep -i mipi
```

`-i` 表示忽略大小写。

---

# 3. `dmesg -w` 是什么？

```bash
dmesg -w
```

表示实时跟踪内核日志，类似：

```bash
tail -f
```

比如你插入 USB 设备、加载驱动、启动摄像头采集时，可以开一个终端：

```bash
dmesg -w
```

然后再执行你的操作，观察内核有没有新的日志输出。

如果板端支持，也可以用：

```bash
dmesg -wH
```

`-H` 会让输出更适合人看。

---

# 4. `dmesg` 和应用日志有什么区别？

比如你的程序日志可能在：

```bash
tail -f /userdata/control_and_video/logs/app.log
```

这个日志是你的 **应用程序** 打印的，比如：

```text
Modbus connected
RTMP push failed
Web server started
```

而 `dmesg` 是 **内核/驱动** 打印的，比如：

```text
imx307 probe failed
i2c transfer timeout
video device registered as /dev/video0
eth0: link up
```

区别可以这样记：

```text
应用日志：看你的程序逻辑有没有问题
dmesg：看内核、驱动、硬件识别有没有问题
```

---

# 5. 典型场景 1：摄像头没有 `/dev/video*`

你执行：

```bash
ls /dev/video*
```

发现没有任何设备节点。

这时候就要看 `dmesg`：

```bash
dmesg | grep -i imx307
dmesg | grep -i video
dmesg | grep -i csi
dmesg | grep -i i2c
```

你重点看有没有类似信息：

```text
imx307 probe failed
i2c read failed
sensor not found
mipi csi error
video device registered
```

如果看到：

```text
imx307 probe failed
```

说明驱动加载或 sensor 初始化失败。

如果看到：

```text
i2c transfer timeout
```

可能是 I2C 地址、供电、复位、设备树配置有问题。

如果看到：

```text
video device registered as /dev/video0
```

说明内核已经注册了视频设备，接下来可以去看 `/dev/video0` 和 V4L2/GStreamer 层。

---

# 6. 典型场景 2：网口不通

比如 RV1126 访问不了 PC，或者 PC 访问不了板子。

可以看：

```bash
dmesg | grep -i eth
dmesg | grep -i link
```

可能看到：

```text
eth0: link up
eth0: link down
```

如果一直 `link down`，可能是：

```text
网线没插好
交换机问题
PHY 没初始化
网卡驱动问题
设备树网口配置问题
```

然后配合：

```bash
ip addr
ip route
ping <PC_IP>
ss -lntp
```

一起查。

---

# 7. 典型场景 3：USB / 串口设备识别

插 USB 转串口后，可以实时看：

```bash
dmesg -w
```

正常可能出现：

```text
usb 1-1: new full-speed USB device
ch341-uart converter now attached to ttyUSB0
```

这说明系统生成了：

```text
/dev/ttyUSB0
```

如果没出现，就可能是：

```text
USB 没识别
驱动没编译
供电问题
设备损坏
```

---

# 8. 典型场景 4：驱动模块加载失败

如果你加载一个 `.ko`：

```bash
insmod xxx.ko
```

失败后不要只看终端报错，要马上看：

```bash
dmesg | tail -n 50
```

因为很多详细错误只会出现在内核日志里。

可能看到：

```text
Unknown symbol xxx
module verification failed
probe failed
invalid module format
```

含义大概是：

```text
Unknown symbol：依赖的内核符号不存在
invalid module format：模块和当前内核版本/架构不匹配
probe failed：驱动匹配到了设备，但初始化失败
```

---

# 9. `dmesg` 日志级别

内核日志有不同级别，比如：

```text
emerg
alert
crit
err
warn
notice
info
debug
```

你可以只看错误和警告：

```bash
dmesg --level=err,warn
```

如果系统支持的话，这个命令很好用。

或者用 grep：

```bash
dmesg | grep -i "error"
dmesg | grep -i "fail"
dmesg | grep -i "timeout"
dmesg | grep -i "warn"
```

---

# 10. `dmesg -c` 要小心

有个命令：

```bash
dmesg -c
```

它会显示日志后清空内核环形缓冲区。

初学者不要随便用。

你平时只用：

```bash
dmesg
dmesg | tail
dmesg -w
```

就够了。

---

# 11. 为什么有些旧日志没了？

`dmesg` 读取的是内核的 **ring buffer，环形缓冲区**。

环形缓冲区大小有限。

如果系统运行很久，日志很多，旧日志会被新日志覆盖。

所以排查问题时最好：

```text
问题发生后马上 dmesg | tail
或者提前 dmesg -w 实时观察
```

---

# 12. `dmesg` 和 `journalctl -k`

有些 Linux 系统使用 systemd，可以用：

```bash
journalctl -k
```

它也是看内核日志。

实时看：

```bash
journalctl -kf
```

但很多嵌入式 BusyBox 系统可能没有 systemd，所以最通用还是：

```bash
dmesg
```

---

# 13. 结合你的项目怎么用？

你的项目中可以这样分层排查。

## 应用服务起不来

先看应用日志：

```bash
tail -f app.log
```

如果怀疑底层设备问题，再看：

```bash
dmesg | tail -n 50
```

---

## IMX307 没画面

```bash
ls /dev/video*
dmesg | grep -i imx307
dmesg | grep -i video
dmesg | grep -i csi
```

判断是：

```text
摄像头没被内核识别
还是设备节点有了但 GStreamer 管道失败
```

---

## GStreamer 报 `no element`

这个通常不是 `dmesg` 主查对象，而是用户态插件问题。

应该查：

```bash
gst-inspect-1.0 x264enc
echo $GST_PLUGIN_PATH
```

但如果是 `v4l2src` 打不开 `/dev/video0`，就要结合：

```bash
dmesg | grep -i video
```

---

## 网络推流不通

先看应用日志和端口：

```bash
ss -lntp
ping <PC_IP>
```

再看内核网卡日志：

```bash
dmesg | grep -i eth
dmesg | grep -i link
```

---

# 14. 面试怎么回答：`dmesg` 是什么？

可以这样答：

> `dmesg` 用来查看 Linux 内核日志，主要用于排查内核启动、驱动加载、硬件识别、设备节点注册、文件系统挂载等问题。在嵌入式 Linux 中，比如摄像头、网卡、串口、I2C、MIPI CSI 等外设异常时，我会结合 `dmesg` 查看驱动 probe 是否成功、是否有 timeout、failed、error 等日志。

---

# 15. 面试追问：摄像头没有 `/dev/video*`，你怎么用 `dmesg` 排查？

可以这样答：

> 我会先 `ls /dev/video*` 确认设备节点是否存在。如果没有，就用 `dmesg | grep -i imx307`、`dmesg | grep -i i2c`、`dmesg | grep -i csi`、`dmesg | grep -i video` 查看 sensor、I2C、MIPI CSI 和 V4L2 相关日志。重点关注 probe failed、i2c read failed、clock/gpio/regulator error 等信息，再结合设备树和驱动匹配情况继续定位。

---

# 16. 面试追问：`dmesg` 和应用日志区别？

可以答：

> 应用日志是用户态程序自己打印的日志，主要反映业务逻辑和程序运行状态；`dmesg` 是内核日志，主要反映 Kernel 和驱动层状态，比如设备识别、驱动加载、硬件错误和内核异常。排查问题时，如果应用打不开设备或硬件不工作，我会同时看应用日志和 `dmesg`，判断问题是在应用层还是内核/驱动层。

---

# 17. 你需要背熟的一句话

> **`dmesg` 是查看内核日志的工具，嵌入式调试中主要用来排查驱动加载、硬件识别、设备节点生成、I2C/SPI/UART/网卡/摄像头等底层问题。**
