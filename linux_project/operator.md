# ADB
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
