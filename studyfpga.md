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
module count(out,data,load,reset,clk);
    output[7:0] out;
    input[7:0] data;
    input load,clk,reset;
    reg[7:0] out;
    always @(posedge clk)
    begin
    if(!reset) out=8'h00;




```
---
always@(敏感信号列表)
    功能描述语句


