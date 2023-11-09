#!/usr/bin/env bash

rm -rf dist
mkdir -p dist
mkdir -p dist/include
mkdir -p dist/lib
mkdir -p dist/lib/ext
mkdir -p dist/bin
mkdir -p dist/share
mkdir -p dist/share/scm

make clean
EXTRA_CFLAGS="-O2 -g" make
cp mscm dist/bin
cp pl5 dist/bin
chmod 755 dist/bin/pl5
cp libmscm.so dist/lib
cp libcore.so dist/lib/ext
cp std.scm dist/share/scm
for file in `ls include/*`; do
    cp $file dist/$file;
done

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
