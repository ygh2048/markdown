# 内核

## 状态
- DORMANT(睡眠态)
  - 任务已创建但未被激活，或者执行完毕被删除
  - OSTaskCreate()后进入READY态
- READY(就绪态)
  - 任务已经准备好，等待被调度器调度使用
  - 已经在就绪表 就绪队列中登记
  - OSTaskDel()后进入DORMANT态 
  - OSStart()后进入RUNNING态
- RUNNING(运行态)
  - 被调度器调度使用--最高优先级就绪的任务
  - CPU正在运行
  - OSTimedly()后进入WAITING态
- WAITING(等待态)
  - **现场保存到堆栈，TCB被保存到等待队列中**
  - 任务被挂起后，进入WAITING态，TCB放入等待队列
  - OSTimeDlyResume()后进入READY态
- ISR(中断服务态)
  - 软件中断 - SWI指令
  - 硬件中断 - GPIO,定时器中断

# 常见操作系统

## UCOS

## FREERTOS

## RTThread


# RTOS 消息机制

## 信号量

## 消息邮箱

## 消息队列

# RTOS 任务管理与调度


# RTOS 软件实现
## 软件定时器
# RTOS 中间件

# RTOS软件分析方法


## 任务划分方法 -- DARTS

### 数据流分析

### 划分任务

- I/O依赖性
- 功能的时间关键性
- 计算需求
- 时间内聚
- 周期内聚

# 跳转
[跳转链接list点这里](../README.md)









