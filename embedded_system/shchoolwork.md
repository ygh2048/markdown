# 就绪表算法
```c
/*
已知 任务优先级号为64个，每个用1个bit位的位置号表示，每8个bit一个字节，共有8个字节，定义为 char rdyTbl[8]。
则rdyTbl[0]的bit0为0号优先级bitN为N号优先级
rdyTbl[1]的bit0为（8*1+0）号优先级，bitN为（8*1+N）号优先级
rdyTbl[M]的bit0为（8*M+0）号优先级，bitN为（8*M+N）号优先级
bit位代表的数字越小表示优先级越高。
*/

char const table[] = {
    0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
    4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
};


/**
 * @brief 通过查表法获取最高优先级任务索引
 * 
 * 该函数根据就绪表(Ready Table)确定最高优先级任务的索引。
 * 就绪表由8个字节组成，每个字节表示一个优先级组(共8组)的就绪状态，
 * 每个字节的8位分别表示该组内8个任务的就绪状态。
 * 
 * @param rdyTBL 就绪表数组，包含8个char元素(每个char表示一个组的状态)
 * @return int 最高优先级任务的索引(0-63)，如果没有就绪任务则返回-1
 */
int getHighestPrioritytable(char rdyTBL[8]) {
    // 初始化组就绪位图(RdyGrp)，初始值为0b11111111
    unsigned char RdyGrp = 0b11111111;
    
    // 构建组就绪位图：遍历就绪表数组(从高索引到低索引)
    for (int i = 7; i >= 0; i--) {
        if (rdyTBL[i] == 0) {
            // 如果当前组没有就绪任务，左移位图并在最低位补0
            RdyGrp = (RdyGrp << 1) | 0;
        } else {
            // 如果当前组有就绪任务，左移位图并在最低位补1
            RdyGrp = (RdyGrp << 1) | 1;
        }
    }
    // 注意：循环结束后，RdyGrp的低8位表示组就绪状态
    // 最高位(MSB)对应rdyTBL[7]，最低位(LSB)对应rdyTBL[0]

    // 通过查表获取最高优先级组的索引(0-7)
    char high3 = table[RdyGrp];  // table是预定义的优先级查找表
    
    // 计算组内偏移量：组索引左移3位(相当于乘以8)
    char high = (char)(high3 << 3);
    
    // 获取该组内的任务就绪位图，并查表找到组内最高优先级任务索引(0-7)
    char low = table[rdyTBL[high3]];

    // 如果组就绪位图非空(有就绪任务)
    if (RdyGrp) {
        // 组合结果：全局索引 = 组索引 × 8 + 组内索引
        return high + low;
    } else {
        // 没有就绪任务时返回-1
        return -1;
    }
}

```

# 实验二

```c
//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{

		OSSemPost(TSem);	//发送信号量给任务
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}



/*
 * @Author: ygh && “ygh2048009576@outlook.com”
 * @Date: 2025-04-12 20:57:59
 * @LastEditors: ygh && “ygh2048009576@outlook.com”
 * @LastEditTime: 2025-04-21 08:23:34
 * @FilePath: \experient2\USER\task.c
 * @Description: 任务函数具体实现
 * 
 * Copyright (c) 2025 by ygh, All Rights Reserved. 
 */
#include "task.h"

OS_STK START_TASK_STK[START_STK_SIZE];
OS_STK WAVE_TASK_STK[WAVE_STK_SIZE];
OS_STK OUTPUT_TASK_STK[OUTPUT_STK_SIZE];
OS_STK CTL_TASK_STK[CTL_STK_SIZE];

OS_EVENT* TSem;		//定时器信号量指针
OS_EVENT* PSem;		//中断信号量指针

u16 adc_value = 0;
u8 ctl_flag = 0;
                                     
/**
 * @description: 启动任务，创建其他任务，并创建信号量
 * @param {void} *pdata
 * @return {*}
 */
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	TSem = OSSemCreate(0);		//创建信号量
	PSem = OSSemCreate(0);		//创建信号量
	printf("start task\r\n");
  OS_ENTER_CRITICAL();			 
	OSTaskCreate(task_wave,(void *)0,(OS_STK*)&WAVE_TASK_STK[WAVE_STK_SIZE-1],WAVE_TASK_PRIO); 	
	OSTaskCreate(task_output,(void *)0,(OS_STK*)&OUTPUT_TASK_STK[OUTPUT_STK_SIZE-1],OUTPUT_TASK_PRIO);
	OSTaskCreate(task_ctl,(void *)0,(OS_STK*)&CTL_TASK_STK[CTL_STK_SIZE-1],CTL_TASK_PRIO);			   
	OSTaskSuspend(START_TASK_PRIO);	
	OS_EXIT_CRITICAL();				
} 

/**
 * @description: 负责在收到一次定时器发来的信号量时进行一次ADC,并把ADC值发给串口输出任务
 * @param {void} *pdata
 * @return {*}
 */
void task_wave(void *pdata)
{	 	

	INT8U err;
	while(1)
	{
		OSSemPend(TSem,0,&err);	//等待信号量
		if(ctl_flag)
		{
		adc_value = Get_Adc_Average(ADC_Channel_5,20);	//读取通道5的ADC值,20次取平均	
		}
		delay_ms(20);
		LED0=!LED0; //LED闪烁测试
	} 
}

/**
 * @description: 串口输出任务, 负责把收到的ADC值转换为ASCII码并通过串口输出
 * @param {void} *pdata
 * @return {*}
 */
void task_output(void *pdata)
{	  
	float temp;
	u16 adcx;
	while(1)
	{
		if(ctl_flag)
		{
			//printf("ADC Value: %d\r\n", adc_value);	//输出ADC值
			temp=(float)adc_value*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
			adcx=temp;
			temp-=adcx;
			delay_ms(20);
			temp*=1000;
			printf("ADC value: %d.%d\r\n",adcx,(u16)temp);	//输出ADC值
			delay_ms(100);
			LED1=!LED1;	//LED闪烁测试
		}
		else{
		delay_ms(400);
		LED1=!LED1;	//LED闪烁测试
		}
	}
}

/**
 * @description: 负责接收按键中断发来的信号，短按则开始进行波形采集和发送，长按则关闭波形采集和发送
 * @param {void} *pdata
 * @return {*}
 */
void task_ctl(void *pdata)
{	  
	INT8U err;
	u32 time_press = 0;	//按下时间

	while(1)
	{
    // 等待按键信号量
     OSSemPend(PSem, 0, &err);
     time_press = 0;
        // 记录按下时间
     delay_ms(10);
        // 等待按键释放
     while(WK_UP == 1)
		{
			delay_ms(20);
			time_press++;
		}
		if(time_press > 35)
		{
			ctl_flag = 0;	// 长按超过1秒，停止采集
			printf("stop collect!\r\n");
		}
		else if(time_press >= 4)
		{
			ctl_flag = 1;	// 短按，开始采集
			printf("start collect!\r\n");
		}
		delay_ms(500);//防止按键抖动
	}

}


```

# 实验三
## task1


## task2

## task3
 

## task4

## task5

## task6

## task7


## task8




# 作业3

# 作业6

```c

/*
 * @Author: ygh && “ygh2048009576@outlook.com”
 * @Date: 2025-04-02 19:26:23
 * @LastEditors: ygh && “ygh2048009576@outlook.com”
 * @LastEditTime: 2025-04-06 12:28:06
 * @FilePath: \task6\user\task.c
 * @Description: 
 * 
 * Copyright (c) 2025 by ygh, All Rights Reserved. 
 */
# include "task.h"

 
//任务堆栈空间大小
OS_STK Start_Task_STK[Start_STK_Size];
 
//任务堆栈空间大小
OS_STK LED_Task_STK[LED_STK_Size];
 
//任务堆栈空间大小
OS_STK KEY_Task_STK[KEY_STK_Size];
 
/**
 * @description: 
 * @param {void *} pdata
 * @return {*}
 */
void start_task(void * pdata)
{
OS_CPU_SR cpu_sr=0;
 
pdata = pdata;
OS_ENTER_CRITICAL();  //进入临界区，无法被中断打断

//printf("\r\n start task\r\n");
	
OSTaskCreate(LED_task, (void *)0, (OS_STK *)&LED_Task_STK[LED_STK_Size-1], LED_Task_PRIO);
OSTaskCreate(KEY_task, (void *)0, (OS_STK *)&KEY_Task_STK[KEY_STK_Size-1], KEY_Task_PRIO);

OSTaskSuspend(Start_Task_PRIO);  //挂起起始任务
OS_EXIT_CRITICAL();  //退出临界区，可以被中断打断
}

/**
 * @description: LED控制任务
 * @param {void *} pdata
 * @return {*}
 */
void LED_task(void * pdata)
{
while(1)
{
	//printf("this is task_led \r\n");
	LEDON(3);
	OSTimeDlyHMSM(0,0,1,0); //延时1000ms
	//DelayMs(1000);
 	LEDOFF(3);
	OSTimeDlyHMSM(0,0,1,0); //延时1000ms
  	//DelayMs(1000);
}
}

/**
 * @description: 按键任务
 * @param {void *} pdata
 * @return {*}
 */
void KEY_task(void * pdata)
{
	while(1)
	{
		LEDToggle(0);
		while (!(GPIOA->IDR & GPIO_Pin_1) )
		{
			LEDON(2);
			OSTimeDly(1);
		}
		LEDToggle(0);
		OSTimeDlyResume(2);
		while((GPIOA->IDR & GPIO_Pin_1) )
		{
			//printf("key pressed\r\n");
			OSTimeDly(1);
		}
	}
}


```