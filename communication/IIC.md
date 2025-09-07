# IIC

## 基本知识     
IIC,即I²C，全称 Inter-Integrated Circuit，字面上的意思是集成电路之间，它其实是I²C Bus简称，所以中文应该叫 集成电路总线 ，它是一种串行通信总线，使用多主从架构，由飞利浦公司在1980年代为了让主板、嵌入式系统或手机用以连接低速周边设备而发展。       



IIC_delay()
{
    delay_us(2)   //延时2us,速度小于250KHZ       
}
**一个IIC总线上，可以挂载多个外接设备。**
## 引脚     
接线：要搭建IIC的通信线路，出除去电源之外，还需要两条线，分别是SDA和SCLK        
SDA:数据信号线，用于传输数据        
SCLK:时钟信号线，用于产生时钟频率，控制时序，实现协议过程       
由此可以看出，由于是单总线进行数据传输，所以IIC协议是半双工的。     


**线与**：只有所有门输出的都是高组态（1） 最终输出才是1，有一个门输出为低(0),最终输出就是0
门需要是集电极开路（OC门）/漏极开路（OD门）
## 时序     
![alt text](image-7.png)    
     
协议规定，在数据的传输过程中，SCLK为高电平时，外设模块开始采集SDA数据线上的数据，此时要求SDA数据线上的电平状态必须稳定，当SCLK为低电平时才允许SDA线上的数据跳变成另外一种状态     
我们在发完开始信号之后，此时SDA数据线的电平状态为低电平，SCLK信号依然是高电平。难道这个时候外设就要开始读取数据了吗？这显然不是的，从发完开始信号到真正的数据传输之间，会有一段缓冲时间，让我们去准备数据，在准备数据阶段，先将SCLK信号拉低一段时间，在这期间将SDA数据线**拉高**一段时间（即数据1），然后再将SCLK信号拉高，此时这个时钟信号的高电平被外设检测到的话，外设就知道要读取数据了，从而SDA上的数据就会被外设读到了。依次类推，传输下一位数据。    
![alt text](image-9.png)    
协议规定，主机每传完一个字节的数据即外设每收到一个字节的数据，外设就要在第9个时钟脉冲到来的时候，将SDA数据线拉低进行**应答**（ACK）,且必须是稳定的低电平，表示已经收到了一个字节的数据，拉高表示不进行应答      
![alt text](image-8.png)    

## 信号
I2C 总线在传送数据过程中共有三种类型信号， 它们分别是：**开始信号**、**结束信号**和**应答信号**。
- 开始信号： SCL 为高电平时， SDA 由高电平向低电平跳变，开始传送数据。
- 结束信号： SCL 为高电平时， SDA 由低电平向高电平跳变，结束传送数据。
- 应答信号：接收数据的 IC 在接收到 8bit 数据后，向发送数据的 IC 发出特定的低电平脉冲，
![alt text](image-10.png)   

## 性质     
IIC可以接多个主设备，多个从设备（外围 设备）。如下图，存在多个主机、多个从机。  
当多主机会产生总线裁决问题。当多个主机同时想占用总线时，企图启动总线传输数据，就叫做总线竞争。I2C通过总线仲裁，以决定哪台主机控制总线       
上拉电阻一般在4.7k~10k之间，默认拉高。  

### 最多挂载地址        
由IIC地址决定，8位地址，减去1位广播地址，是7位地址，2^7=128，但是地址0x00不用，那就是127个地址， 所以理论上可以挂127个从器件。

但是IIC协议没有规定总线上device最大数目，但是规定了总线电容不能超过400pF。
管脚都是有输入电容的，PCB上也会有寄生电容，所以会有一个限制。实际设计中经验值大概是不超过8个器件。

## 速率
- 标准模式  100 Kbps
- 快速模式  400 Kbps
- 高速模式  3.4 Mbps


## 总线仲裁


### 线与
多主机时候总线具有线与功能，即只要有一个节点发送低电平，总线上就标线为低电平        

### SDA回读
总线被启动后，多个主机在每发送一个数据位后，都要对自己的输出电平进行检测，只要检测的电平与自己发出的电平一致，就会继续占用总线      
### 低电平优先

由于线与的存在，多主机时，谁先发送低电平，谁就掌握对于总线的控制权。     


## 硬件IIC

IIC外设上有状态寄存器，在启动总线前回判断总线忙标志     




## 软件IIC
无名创新代码：
*宏定义*
```
#define I2C_READ_SDA  GPIOPinRead(GPIO_I2C,SDA_PIN)			//SDA 
#define I2C_SDA_H  		GPIOPinWrite(GPIO_I2C,SDA_PIN,SDA_PIN)//SDA 
#define I2C_SDA_L 		GPIOPinWrite(GPIO_I2C,SDA_PIN,0)			//SDA
#define I2C_SCL_H  		GPIOPinWrite(GPIO_I2C,SCL_PIN,SCL_PIN)//SCL
#define I2C_SCL_L  		GPIOPinWrite(GPIO_I2C,SCL_PIN,0)			//SCL

```
```
void I2C_GPIO_Config(void)
{	
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO_I2C);
	
	HWREG(GPIO_I2C + GPIO_O_LOCK) = GPIO_LOCK_KEY;//解锁
	HWREG(GPIO_I2C + GPIO_O_CR) |= 0x000000FF;//确认
	HWREG(GPIO_I2C + GPIO_O_LOCK) = 0;//重新锁定
	
  GPIOPinTypeGPIOOutput(GPIO_I2C, SCL_PIN);
  GPIOPinTypeGPIOOutput(GPIO_I2C, SDA_PIN);
	GPIOPadConfigSet(GPIO_I2C,SCL_PIN,GPIO_STRENGTH_I2C,GPIO_PIN_TYPE_STD);
	GPIOPadConfigSet(GPIO_I2C,SDA_PIN,GPIO_STRENGTH_I2C,GPIO_PIN_TYPE_STD);
	
  Delay_Ms(10);
  GPIOPinWrite(GPIO_I2C, SCL_PIN, SCL_PIN);//设置高
  GPIOPinWrite(GPIO_I2C, SDA_PIN, SDA_PIN);//设置高
}
void I2C_SDA_OUT(void)
{
	GPIOPinTypeGPIOOutput(GPIO_I2C, SDA_PIN);
	GPIOPadConfigSet(GPIO_I2C,SDA_PIN,GPIO_STRENGTH_I2C,GPIO_PIN_TYPE_STD);///GPIO_PIN_TYPE_STD);
}

void I2C_SDA_IN(void)
{
  GPIOPinTypeGPIOInput(GPIO_I2C, SDA_PIN);
	GPIOPadConfigSet(GPIO_I2C,SDA_PIN,GPIO_STRENGTH_I2C,GPIO_PIN_TYPE_STD_WPU);
}

// SCL高电平期间，SDA出现下降沿为起始信号
static bool i2cStart()
{
    SDA_OUT;
    SDA_H;
	  SCL_H;   
    i2cDelay();
    if (!sdaRead)  // 如果SDA为低电平，则总线忙，退出
        return false;
    SDA_L;
    if (sdaRead)  // 如果SDA为高电平，则总线忙，退出
        return false;
    SDA_L;
    return true;
}
 
// SCL高电平期间，SDA出现上升沿为停止信号
static void i2cStop(void)
{
    SDA_OUT;
    SDA_L;
	  SCL_L;  
    i2cDelay();  // STOP:when CLK is high DATA from low to high 
    //SCL_H;
    SDA_H;  
    i2cDelay();
}
 
static void i2cAck(void)
{
    SCL_L;
	  SDA_OUT; 
    SDA_L;
    i2cDelay();   
    SCL_H;
    i2cDelay();
    SCL_L;
}
 
static void i2cNoAck(void)
{
    SCL_L;
	  SDA_OUT;
    SDA_H;
    i2cDelay();
    SCL_H;
    i2cDelay();
    SCL_L;
}
 
// SCL高电平期间，SDA电平被从设备拉低表示应答
static bool i2cWaitAck(void)
{
    uint16_t errTimes = 0;
    SDA_H;
	  i2cDelay();
	  SDA_IN;
    SCL_H;
    i2cDelay();
    while (sdaRead) {
        if (errTimes++ > 200) //20
				{
            SCL_L;
            return false;
        }           
        i2cDelay();
    }
    SCL_L;
    return true;
}
 
// 发送数据，数据从高位到低位传输  
static void i2cSendByte(uint8_t byte)  
{
    uint8_t i = 8;
 
    SDA_OUT;
    while (i--) {      
        SCL_L;  // 时钟信号为低电平期间，允许数据线电平变化
        i2cDelay();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L; 
        byte <<= 1; 
        i2cDelay();
        SCL_H;
        i2cDelay();
    }
    SCL_L;
}
 
static uint8_t i2cReceiveByte()  
{
    uint8_t i = 8;
    uint8_t byte = 0;
    SDA_IN;
    SDA_H;
    while (i--) {
        byte <<= 1;
        SCL_H;
        i2cDelay();
        if (sdaRead) {
            byte |= 0x01;
        }
        SCL_L;
        i2cDelay();
    }
    SCL_L;
    return byte; 
}
 
 

 
/**
 * 通过I2C总线写一字节数据
 * @param[in] dev:设备I2C地址
 * @param[in] reg:寄存器地址
 * @param[in] data:要写入的数据
 */
bool i2cWriteOneByte(uint8_t dev, uint8_t reg, uint8_t data)
{
    if (!i2cStart())        
        return false;
    i2cSendByte(dev << 1);  // 从机地址由高7位+读写位构成   
    if (!i2cWaitAck()) {     
        i2cStop();
        return false;
    }
    i2cSendByte(reg);       
    i2cWaitAck();
    i2cSendByte(data);     
    i2cWaitAck();
    return true;
}
 

/**
 *  
 * @param[in] dev:设备I2C地址
 * @param[in] reg:寄存器地址
 * @param[in] len:字节数 
 * @param[in] data:待写入的数据 
 */
uint8_t i2cReadOneBytes(uint8_t dev, uint8_t reg)
{
	unsigned char REG_data;
  i2cStart();
  i2cSendByte(dev<<1);
	i2cWaitAck();
  i2cSendByte(reg);
	i2cWaitAck();
  i2cStart();
  i2cSendByte((dev << 1) | 0x01);
  i2cWaitAck();
	REG_data=i2cReceiveByte();
  i2cStop();
  return REG_data;	
} 


/**
 *  
 * @param[in] dev:设备I2C地址
 * @param[in] reg:寄存器地址
 * @param[in] len:字节数 
 * @param[in] data:待写入的数据 
 */
bool i2cWriteBytes(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *data)
{
    uint8_t i;
 
    if (!i2cStart())        
        return false;
    i2cSendByte(dev << 1);          
    if (!i2cWaitAck()) {     
        i2cStop();
        return false;
    }
    i2cSendByte(dev);   
    i2cWaitAck();
    for (i = 0; i < len; i++) {
        i2cSendByte(data[i]);
        if (!i2cWaitAck()) {
            i2cStop();
            return false;
        }
    }
    i2cStop();
    return true;
}
 
 
/**
 * 从I2C设备中读取数据
 * @param[in] dev:设备I2C地址
 * @param[in] reg:寄存器地址
 * @param[in] len:数据字节数
 * @param[out] data:读出的数据
 */
bool i2cReadBytes(uint8_t dev, uint8_t reg, uint8_t len, uint8_t *data)
{
    if (!i2cStart())        
        return false;
    i2cSendByte(dev << 1);      
    if (!i2cWaitAck()) {     
        i2cStop();
        return false;
    }
    i2cSendByte(reg);     
    i2cWaitAck();
    i2cStart();           
    i2cSendByte((dev << 1) | 0x01);  // 器件地址+读命令    
    i2cWaitAck();
    while (len) {
        *data = i2cReceiveByte();
        if (len == 1)
            i2cNoAck();  // 最后一个字节不应答
        else
            i2cAck();
        data++;
        len--;
    }
    i2cStop();
    return true;
}


```
