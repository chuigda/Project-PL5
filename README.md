# Project-PL5

Yet another scheme dialect implementation.

## Language features

- [x] Function via `define`
- [x] Variable via `define`
- [x] First-class function via `lambda`
- [x] Garbage collection
- [x] Builtin `if`, `cond` and `begin` control flow
- [x] Integer, float, string and symbol type
- [x] Builtin pair and list type
- [x] Quote syntax (as a syntactic sugar)
- [x] Extension via native module

## stdlib
- [x] Builtin pair and list operations
- [x] Imperative programming via `set!`, `set-car!` and `set-cdr!`
- [x] Arithmetic operators
- [x] Comparison operators
- [ ] Logical operators
- [ ] Builtin string operations
- [ ] Conversion between types

Non-targets:
- `let` syntax
- Tail-call optimisation
- Quasi-quote, eval and apply, meta-programming
- Macros
- Continuation
- Exception handling
- Module system

## Building with GNU make

```bash
make
```

### Building with options

```bash
CFLAGS="..." make
EXTRA_CFLAGS="..." make # this won't overwrite CFLAGS defined inside Makefile
```

### Building on Windows

Makefile not adapted to Windows CMD, you need a bash to use it.

- MinGW
```bash
CC=gcc WIN32=1 mingw32-make
```

- Cygwin
```
WIN32=1 make # Cygwin GCC does not link with .so files
```

## Playing

### Unix
```bash
LD_LIBRARY_PATH=. ./mscm libmscmstd.so sample/first-class-function.scm
```

### Windows
```
./mscm libmscmstd.dll sample/first-class-function.scm
```

## Extending

See `stdlib.c`, you can write and build your own native extension module like that,
and include it in command line arguments to let `mscm` load it, just like stdlib.
Possibilities are infinite.

## Contributing

You don't.
