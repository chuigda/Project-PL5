# Project-PL5

只是另一个 Scheme 方言实现

## 关于 PL5M 分支的注释

会和主分支分开维护

## 语言特性

### 核心特性
- [x] 使用 `define` 定义函数
- [x] 使用 `define` 定义变量
- [x] `lambda` 与高阶函数
- [x] 垃圾回收
- [x] 内建的 `if`, `cond` 和 `begin` 控制流
- [x] 内建的 `loop` 与 `break` 控制流
- [x] 整数，浮点，字符串和符号类型
- [x] 内建序对和列表
- [x] Quote 语法（作为构造常量对象的语法糖）
- [x] 通过 native 模块扩展
  - [x] 支持 Rust
  - [ ] 支持 Safe Rust
- [x] 短路逻辑运算符

### 核心库 `libcore`
- [x] 内建序对与列表操作
- [x] 命令式编程：`set!`, `set-car!` 和 `set-cdr!`
- [x] 算术运算符
- [x] 比较运算符
- [x] 内建字符串操作
- [ ] 类型转换

### 计划中的 native 模块
- [x] Vector
- [x] Struct
- [ ] File I/O

### 杂项
- [x] 求值多个脚本
- [x] REPL

### 不会实现
- 尾递归和尾调用优化
- `let` 与 `let*` 语法
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

### 运行脚本

#### Unix
```bash
LD_LIBRARY_PATH=. ./mscm libcore.so sample/first-class-function.scm
```

#### Windows
```bash
./mscm libcore.dll sample/first-class-function.scm
```

------

一次性指定多个脚本也可以

### 运行 REPL

#### Unix
```bash
LD_LIBRARY_PATH=. ./mscm libcore.so --repl
```

#### Windows
```bash
./mscm libcore.dll --repl
```

------

同时指定脚本和 `--repl` 也可以，Mini Scheme 会先加载并运行指定的脚本，然后进入 REPL。注意 `--repl` 必须是最后一个命令行参数。

## 扩展

参见 `libcore.c`，`ext/mscm-sys-rs`，`ext/vec` 以及 `ext/struct`。
你可以编写并构建你自己的 native 模块，然后在命令行参数中包含它，
让 `mscm` 加载它，就像 libcore 一样。无限可能，尽在掌控。

## 贡献与捐赠

You don't.
