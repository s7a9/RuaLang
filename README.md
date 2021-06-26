# RuaLang
C语言风格的Lua

-------

### 运行流程

1. 逐词读取文本，在类Parser中，分析控制流程(if, elif, else, return, break, continue, while, for)，并将语句转为后缀表达式，存入结构体ControlFlow。

