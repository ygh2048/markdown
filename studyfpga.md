# 代码
## 模块
### 代码范例


```
module MUX2_1(out, a, b, sel);  
    output out;          // 定义输出端口 out  
    input a, b, sel;    // 定义输入端口 a, b 和 sel  

    // 声明内部连接的线（wire）  
    wire sel_, a1, b1;  

    not not1(sel_, sel); // 反相器：对选择信号 sel 进行反转，得到 sel_  
    and and1(a1, a, sel_); // 与门：当 sel 为 0 时，a1 作为 a 和 sel_ 的与操作，输出需要选择的信号。  
    and and2(b1, b, sel);  // 与门：当 sel 为 1 时，b1 作为 b 和 sel 的与操作，同样输出需要选择的信号。  
    or or1(out, a1, b1);   // 或门：将两个与门的输出 a1 和 b1 进行或操作，获得最终的输出 out。  

endmodule  
//二选一模块
```

### 数据类型

#### 进制
二进制 b或者B  
十进制 d或D或缺省  
十六进制 h或H  
八进制 o或O  

+/- 位宽 ' 进制 数字  
8'b11001100//位宽为8的二进制数11001100  

位宽和 ' 以及进制和数值之间允许出现空格  

' 和 进制之间，数值之间不允许出现空格  

8'b1000_xxxx //x表示不定值
8'b1000_zzzz //z表示高阻

位宽 二进制下数的宽度

---

#### __实数__

十进制表示法  
2.1  .//小数点左右必须有数字
科学计数法  
42_5'1e2    //42500.0

#### 字符串
用于仿真显示    
双引号内的字符序列  
例如："safhqafh"  

#### 参数
使用parameter来定义符号常量 
parameter 参数名=表达式;

### 变量数据类型
#### 物理数据类型
__连线型类型__  
*定义*
wire 数据名1，数据名2，，，，;  
`wire a,b;`
 *用法*
 对应硬件电路的物理信号连线，没有电荷保持
 *驱动*
 1 结构描述中将其链接到一个门元件或模块的输出端
 2 用连续赋值语句assign对其进行赋值

__寄存器类型__  
*用法*
具有状态保持作用的硬件电路元件


#### 抽象数据类型
__整型__    
__实型__    
__时间型__  



#### 运算符
```
& 与    
~& 与非
| 或
~| 或非
^ 异或
^~  同或
>> 右移 a>>n;//a右移n位，零补齐 
<< 左移 
x ? a : b 条件运算符 x则a，！x则b
{} 位拼接运算符
```
### 补充
建议不要对同一个变量进行多次赋值（简称多重驱动），以避免出现多个信号同时驱动一个输出变量的情况  




### 模块列举
```
module h_adder(A,B,SO,CO);
    input A,B;
    output SO,CO;
    assign SO = A ^ B;//将A,B异或后赋值给SO
    assign CO = A & B;//j将AB与后赋值给CO
endmodule
半加器
```
---
```
module MUX41a(a, b, c, d, s1, s0, y);  
    // 输入端口声明  
    input a, b, c, d;  // 4 个数据输入  
    input s1, s0;      // 2 个选择信号，用于选择输入  
    output y;          // 输出端口  
    reg y;             // 声明 y 为 reg 类型，以便在 always 块中赋值  

    // 组合逻辑部分，根据选择信号更新 y 的值  
    always @(a or b or c or d or s1 or s0) // 监听输入 a, b, c, d 和选择信号 s1, s0 的变化  
        begin   : MUX41 // 块语句开始，命名为 MUX41  
            case({s1, s0}) // 将选择信号组合成一个 2 位向量  
                2'b00: y <= a; // 当 s1s0 为 00 时，输出选择 a  
                2'b01: y <= b; // 当 s1s0 为 01 时，输出选择 b  
                2'b10: y <= c; // 当 s1s0 为 10 时，输出选择 c  
                2'b11: y <= d; // 当 s1s0 为 11 时，输出选择 d  
            endcase  
        end  
endmodule
四选一

```

```
module count(out, data, load, reset, clk);  
    // 模块名为 count，包含以下端口:  
    // 输出端口:  
    output [7:0] out;    // 8 位输出，表示计数的结果  
    // 输入端口:  
    input [7:0] data;    // 8 位输入，数据输入，通常用于加载计数器的初始值  
    input load;          // 负载信号，当其为高时，计数器从 data 加载值  
    input reset;         // 重置信号，当其为低时，计数器将重置为 0  
    input clk;           // 时钟信号，用于同步计数器的操作  
    
    // 寄存器类型，用于存储 count 的值  
    reg [7:0] out;       // 定义寄存器 out，表示当前计数值  

    // 时序逻辑块，每当时钟上升沿触发  
    always @(posedge clk)   
    begin  
        // 检查重置信号  
        if (!reset)   
            out = 8'h00;  // 如果 reset 为低，计数器被重置为 0  
        else if (load)   
            out = data;   // 如果 load 为高，计数器加载 input data 的值  
        else   
            out = out + 1; // 否则，计数器递增 1  
    end  
endmodule

```


```
module MUX41a(A, B, C, D, S1, S0, Y);  
    // 模块名为 MUX41a，表示 4 路复用器，具有以下端口：  
    
    // 输入端口：  
    input A, B, C, D;    // 四个输入信号（A, B, C, D）  
    input S1, S0;        // 两个选择信号，用于选择哪个输入作为输出  
    
    // 输出端口：  
    output Y;            // 复用器的输出信号 Y  

    // 中间线网声明，用于选择信号的组合  
    wire AT, BT;         // 中间线网 AT 和 BT  

    // 根据选择信号 S0 选择输入 C 或 D  
    // 当 S0 为 1 时，AT 选择 D；当 S0 为 0 时，AT 选择 C  
    assign AT = S0 ? D : C;   
    
    // 根据选择信号 S0 选择输入 A 或 B  
    // 当 S0 为 1 时，BT 选择 B；当 S0 为 0 时，BT 选择 A  
    assign BT = S0 ? B : A;   

    // 根据选择信号 S1 选择线网 AT 或 BT  
    // 当 S1 为 1 时，Y 选择 AT；当 S1 为 0 时，Y 选择 BT  
    assign Y = S1 ? AT : BT;   
endmodule  
四选一
```

```
module f_adder(ain, bin, cin, cout, sum);  
    // 模块名为 f_adder，表示一个全加器，具有以下端口：  
    
    // 输出端口：  
    output cout; // 输出进位，若有进位则为 1  
    output sum;  // 输出和，表示加法结果的某一位  

    // 输入端口：  
    input ain;   // 输入 A 的一位  
    input bin;   // 输入 B 的一位  
    input cin;   // 输入 C 的一位（来自低位的进位）  

    // 中间网线声明，用于连接和计算  
    wire net1, net2, net3; // net1: 半加器 U1 的进位输出  
                            // net2: 半加器 U1 的和输出  
                            // net3: 半加器 U2 的进位输出  

    // 实例化第一个半加器 U1，进行 A 和 B 的加法  
    h_adder U1(ain, bin, net1, net2);  
    // U1: 计算输入 A 和 B 的和，产生进位 net1 和和 net2  

    // 实例化第二个半加器 U2，计算 net1（来自 U1 的和）和 cin 的和  
    h_adder U2(.A(net1), .B(cin), .SO(sum), .CO(net3));  
    // U2: 计算 net1 和 cin 的和，生成输出 sum 和进位 net3  

    // 最后，通过或门计算最终的进位输出 cout  
    or U3(cout, net2, net3);  
    // U3: 输出 cout 为 net2（U1 的进位）和 net3（U2 的进位）的逻辑或  
endmodule

```
---
#### 语句用法
always@(敏感信号列表)
    功能描述语句

initial
    功能描述语句


```
reg Y;
initial
    Y=2;//Y在0时刻被赋值为2；
reg C;
initial
    #2 C=1;在时刻2被赋值为1；
```
__注意__    
---    
一个模块可以包括多个initial和always     
并行执行        
过程块都不能嵌套        
initial和always中被赋值的对象只能是寄存器型变量，是一种动态触发的存储结构       
initial语句在仿真开始时执行，即在0时刻开始执行，主要用于初始化和波形产生    
initial语句的各个进程语句仅仅执行一次   
initial语句不可综合 
always也是从0时刻执行，执行完最后一条语句后继续循环执行 
always语句中`@(敏感语句列表)`是可选项目 
在`always@(clk or reset)`中只要clk和reset有一个信号的电平发生变化，就会触发后面的begin end







## testbench

`always #5 CLK= ~CLK`产生周期为10的波形  