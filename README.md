# Project-PL5

只是另一个 Scheme 方言实现

## 语言特性

### 核心特性
- [x] 使用 `define` 定义函数
- [x] 使用 `define` 定义变量
- [x] `lambda` 与高阶函数
- [ ] `let` 与 `let*` 语法
- [x] 垃圾回收
- [x] 内建的 `if`, `cond` 和 `begin` 控制流
- [ ] 内建的 `loop` 与 `break` 控制流
- [x] 整数，浮点，字符串和符号类型
- [x] 内建序对和列表
- [x] Quote 语法（作为构造常量对象的语法糖）
- [x] 通过 native 模块扩展
  - [x] 支持 Rust
  - [ ] 支持 Safe Rust

### 标准库 `stdlib`
- [x] 内建序对与列表操作
- [x] 命令式编程：`set!`, `set-car!` 和 `set-cdr!`
- [x] 算术运算符
- [x] 比较运算符
- [ ] 逻辑运算符
- [ ] 内建字符串操作
- [ ] 类型转换

### 计划中的 native 模块
- [ ] Vector
- [ ] File I/O

### 杂项
- [ ] 求值多个脚本
- [ ] REPL

### 不会实现
- 尾递归和尾调用优化
- Quasi-quote, eval 和 apply, 原编程
- 宏
- `call/cc`
- 异常处理
- 模块系统

## 使用 GNU make 构建

```bash
make
```

### 带选项构建

```bash
CFLAGS="..." make
EXTRA_CFLAGS="..." make # 不会覆盖 Makefile 里原有的那部分 CFLAGS
```

### 在 Windows 上构建

Makefile 没有适配 Windows CMD，你需要 bash。

- MinGW
```bash
# 使用 CC 指定使用 gcc 编译器，不然 MinGW make 可能会自作主张使用 cc
# 使用 WIN32=1 告诉 Makefile 你在用 Windows，从而生成 .dll 而不是 .so
CC=gcc WIN32=1 mingw32-make
```

- Cygwin
```bash
# Cygwin GCC 也不能识别 .so
WIN32=1 make
```

## 玩耍

### Unix
```bash
LD_LIBRARY_PATH=. ./mscm libmscmstd.so sample/first-class-function.scm
```

### Windows
```
./mscm libmscmstd.dll sample/first-class-function.scm
```

## 扩展

参见 `stdlib.c` 和 `ext/mscm-sys-rs` 以及 `ext/mscm-vec`。
你可以编写并构建你自己的 native 模块，然后在命令行参数中包含它，
让 `mscm` 加载它，就像 stdlib 一样。无限可能，尽在掌控。

## 贡献与捐赠

You don't.
