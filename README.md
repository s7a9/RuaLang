# RuaLang
C语言风格 / 类似Lua / 解释器
=======

### 运行流程

1. 逐词读取文本，分析控制流程，将语句转为由压栈和计算符号等命令构成的后缀表达式。
2. 如果从文本中读取文件，则从main函数开始运行；如果以交互式运行，则直接运行代码。

### 程序示例

**文件** 

    pow = function(a, n) { # 整数指数计算
       ret = 1;
        for (i = 0; i < n; i = i + 1){
    		ret = ret * a;
    	}
    	return ret;
    };
    
    gcd = function(a, b) {  # 最大公约数
    	if (b == 0) return a;
    	else return gcd(b, a % b);
    };
    
    main = function() {
    	print("(a,b)=", gcd(int(input("a=")), int(input("b="))));
    	return pow(2, 10);
    };

输出 （<>中内容代表键盘输入）

    a=<99>
    b=<88>
    (a,b)=11

**交互式** （<>中内容代表键盘输入）
    
    >>> <for(i=0;i<5;i=i+1)print(i,"\t", i+1, "\n");>
    0       1
    1       2
    2       3
    3       4
    4       5

    >>> <l=["a","b","c","e"];>

    >>> <for(i=0;i<4;i=i+1)print(l[i]*i);>
    bcceee
    >>> <exit>

### 文档

1.  控制语句
    **if elif else return break continue while for** 格式与C语言一致。
2.  变量

    | RuaLang变量类型 | 实际C语言类型 |
    | --- | --- |
    | Boolean | bool |
    | Inteager | 64位整数 |
    | Float | 64位浮点数 |
    | String | std::string |
    | List | std::vector |
    | Table | std::map |
    | Function | struct |

3.  函数
    
        func = function(para){commands};
    
    函数是变量，所以声明函数时注意赋值语句结尾的分号；同时支持匿名函数。

4.  内置函数

    **print(para[, ...]) : void**
    打印变量（不支持打印Table和Function）。

    **input(para[, ...]) : string**
    打印传入变量（相当于print）再接收命令行输入。

    **int(para)** **float(para)** **str(para)**
    类型转换

    **len(para : list) : integer**
    获取list长度

    **read_file(filename : str)**
    读取文件（文件必须存在）

    **exist(filename : str)**
    检查文件是否存在

    **write_file(filename : str, content : str)**
    向文件中写入内容

    **exec(command : str)**
    执行字符串所表示的代码

    **is_null(object)**
    判断变量是否为空

    **append(list, item[, item1, ...])**
    把变量加入数组中

    **typeof(object)**
    返回物体的类型（具体见./examples/math.rua中定义）

    **exit([object])**
    推出程序（展示传入的内容）

    **str_to_list(str)**
    把字符串转化为数组

    **list_to_str(list)**
    把数组转化为字符串

    **breakpoint([object])**
    设置断点（输出传入对象）

    **\_id\_(object)**
    展示对象的编号

    **system(command : str)**
    系统命令

    **table()**
    返回一个空Table

    **randn()**
    64位随机数

    **remove(list, index : int[, index1, ...])**
    移除列表中指定下标的内容

    **sleep(milliseconds : int)**
    挂起线程

5.  运算符
        
        +  -  *  /  \(整除)  <<  >>  %
        <  >  <=  >=  ==  != 
        = .
        &&  ||
        del(删除引用)  [](下标)

#### 注意事项
-   层出不穷的BUG
-   由于要将函数指针转为void*，程序无法通过g++正常编译，VISUAL C++ 2019编译通过。
