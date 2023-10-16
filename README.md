# Project-PL5

Yet another scheme dialect implementation.

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
