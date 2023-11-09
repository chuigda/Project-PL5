#!/usr/bin/env bash

rm -rf dist
mkdir -p dist
mkdir -p dist/include
mkdir -p dist/lib
mkdir -p dist/lib/ext
mkdir -p dist/bin

EXTRA_CFLAGS="-O2 -g" make
cp mscm dist/bin
cp libmscm.so dist/lib
cp libcore.so dist/lib/ext

if ! command -v cargo &> /dev/null
then
    echo "Rust toolchain not installed, rust extensions will not be built"
    exit 0
fi

cd ext/vec
cargo clean
cargo build --release

cd ../struct
cargo clean
cargo build --release

cd ../..

cp ext/vec/target/release/libvec.so dist/lib/ext
cp ext/struct/target/release/libstruct.so dist/lib/ext
