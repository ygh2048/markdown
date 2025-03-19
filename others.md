# SPI

## 基本知识
同步串行通信协议    
**SPI**是一个同步的数据总线，也就是说它用单独的数据线和一个单独的时钟信号来保证发送端和接收端的完美同步 
时钟是一个振荡信号，它告诉接收机在确切的时机对数据线上的信号进行采样    

**全双工**:具有单独的发送和接收线路     
*因此可以在同一时间发送和接收数据*
*SPI的接收硬件可以是一个简单的移位寄存器，这比异步串行通信所需的完整UART要简单的多，并且更加便宜*   
*SPI并未明确数据是高位在前还是低位在前，一般回采用高位在前（MSB）方式传传输*    

## 引脚
- SCLK:串行时钟信号，由主机产生发送给从机   
- MOSI(master output slave input):主机输出，从机输入，数据来自主机     
- MISO(master input slave output):主机输入，从机输出 ，数据来自于从机      
- SS/CS:片选信号:由主机发送，控制与哪个从机通信，通常是低电平有效信号。    

**主机**：产生时钟的一侧称为主机，另一侧称为从机    
总是只有一个主机（一般是MCU/微控制器），但是可以有多个从机  

## 数据传输过程
- 主机先将*NSS*信号拉低，这样可以保证开始接收数据   
- 当接收端检测到时钟的边沿信号（可以是上升沿也可以是下降沿，这个看配置）时，它将立即读取数据线上的信号，这样就得到了一位数据
- 主机发送到从机时：主机产生相应的时钟信号，然后数据一位一位得从MOSI信号线上进行发送到从机  
- 主机接收从机数据：如果从机需要将数据发送回主机，则主机将继续生成预定数量的时钟信号，并且从机会将数据通过MISO信号线发送    

## 配置     
### 时钟频率   

### 时钟极性 CPOL/CKP
它是指时钟信号在空闲状态下是高电平还是低电平，当时钟空闲时为低电平即 CPOL=0，反之则 CPOL=1。
### 时钟相位 CPHA/CKE
它是指时钟信号开始有效的第一个边沿和数据的关系。当时钟信号有效的第一个边沿处于数据稳定期的正中间时定义CPHA=0，反之时钟信号有效的第一个边沿不处于数据稳定期的正中间定义CPHA=1。所以在时钟信号SCK的第一个跳变沿采样即CPHA=0，再时钟信号SCK的第二个跳变沿采样为CPHA=1。

4种SPI通信操作模式

![alt text](image-5.png)
![alt text](image-4.png)

### 协议规定
    ```
    SPI协议规定一个SPI设备不能在数据通信过程中仅仅充当一个发送者（Transmitter）或者接受者（Receiver）。在片选信号CS为0的情况下，每个clock周期内，SPI设备都会发送并接收1bit数据，相当于有1bit数据被交换。

    MOSI及 MISO的数据在SCK的上升沿期间变化输出，在 SCK的下降沿时被采样。即在 SCK 的下降沿时刻，MOSI及 MISO 的数据有效，高电平时表示数据“1”，为低电平时表示数据“0”。在其它时刻，数据无效，MOSI 及 MISO为下一次表示数据做准备。

    SPI 每次数据传输可以 8 位或 16 位为单位，每次传输的单位数不受限制。
    ```
## 多从机模式   

### 多NSS

每个从机单独一个NSS线

### 菊花链
数据信号串行传输

## 优缺点

### 优点
- 全双工串行通信
- 告诉数据传输速率
- 简单的软件配置
- 机器灵活的数据传输，不限于8位，它可以是任意大小的字
- 非常简答的硬件结构，从站不需要唯一地址（与IIC不同）。从机使用主机时钟，不需要精密时钟振荡器/晶振（与UART不同），不需要收发器（与CAN不用）

### 缺点
- 没有硬件从机应答信号（主机可能在不知情情况下无处发送）
- 通常仅仅支持一个主设备
- 通常需要更多引脚
- 没有定义硬件级别的错误检查协议
- 与RS-232和CAN总线相比，只能支持非常短的距离

HAL库
```
static void MX_SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;				//主机模式
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;	//全双工
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;		//数据位为8位
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;		//CPOL=0
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;			//CPHA为数据线的第一个变化沿
    hspi1.Init.NSS = SPI_NSS_SOFT;					//软件控制NSS
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;//2分频，32M/2=16MHz
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;			//最高位先发送
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;			//TIMODE模式关闭
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;//CRC关闭
    hspi1.Init.CRCPolynomial = 10;					//默认值，无效
    if (HAL_SPI_Init(&hspi1) != HAL_OK)				//初始化
    {
        _Error_Handler(__FILE__, __LINE__);
    }
}
    
//发送数据
HAL_StatusTypeDef  
HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, 
                 uint8_t *pData, 
                 uint16_t Size, 
                 uint32_t Timeout);
//接收数据
HAL_StatusTypeDef  
HAL_SPI_Receive(SPI_HandleTypeDef *hspi, 
                uint8_t *pData, 
                uint16_t Size, 
                uint32_t Timeout);

```

## 软件SPI
### 配置
这里模拟的时序是SPI的模式3（CPOL=1,CPHA=1），原因有两点：   

1、模式3的SCK空闲电平为高，有高电平向低电平翻转较为容易和快，   

2、模式3在偶数边沿采样，防止第一个信号没采到。  

🌘首先，对于软件模拟SPI的GPIO初始化，可以参考SPI的GPIO初始化的配置，我们不用使用复用功能就行，使用普通推挽输出和浮空输入。  

```
void SPI_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	/*SPI CS GPIO Confgi*/
	GPIO_InitStruct.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin	=	EEPROM_SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_SPI_CS_PORT,&GPIO_InitStruct);
	
	/*SPI SCK GPIO Config*/
	GPIO_InitStruct.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin	=	EEPROM_SPI_CLK_PIN;
	GPIO_InitStruct.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_SPI_CLK_PORT,&GPIO_InitStruct);
	
	/*MISO GPIO Config*/
	GPIO_InitStruct.GPIO_Mode	=	GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin	=	EEPROM_SPI_MISO_PIN;
	GPIO_Init(EEPROM_SPI_MISO_PORT,&GPIO_InitStruct);
	
	/*MOSI GPIO Config*/
    GPIO_InitStruct.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin	=	EEPROM_SPI_MOSI_PIN;
	GPIO_InitStruct.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(EEPROM_SPI_MOSI_PORT,&GPIO_InitStruct);
 
	EEPROM_SPI_CS_HIGH(); //位选拉高
	EEPROM_SPI_CLK_HIGH();//时钟线拉高
}
```
### 接受发送函数       

其中注意，使用SPI时，发送和接收其实是同一个函数，通过判断RXNE来确认发送结束，此时也接收完数据，接收数据同样要主机产生时序，时序通过主机发送数据产生，所以会发送无用的Dummy数据。
软件模拟不需要，直接接收自己本身会产生时序，不用发送DUMMY数据。Delay函数随便设置，不要小于手册的时间即可。 


void SPI_SendData(uint8_t data)
{
	uint8_t cnt;
	
	for(cnt=0;cnt<8;cnt++)
	{
		EEPROM_SPI_CLK_LOW();//拉低CLK
		SPI_Delay(10);//这个延时时间任意，但要大于芯片数据手册上的(纳秒级的)
		if(data &0x80)
		{
			EEPROM_SPI_MOSI_HIGH();
		}
		else
		{
			EEPROM_SPI_MOSI_LOW();
		}
		data <<= 1;
		EEPROM_SPI_CLK_HIGH();//拉高CLK
		SPI_Delay(10);
	}
 
}
```
# IIC


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

# UART