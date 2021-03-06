#!/bin/bash
NDK=D:/Android/android-ndk-r13b
SYSROOT=$NDK/platforms/android-21/arch-x86/
TOOLCHAIN=$NDK/toolchains/x86-4.9/prebuilt/windows-x86_64
PREFIX=./android/x86
function build_one { 
./configure \
    --prefix=$PREFIX \
    --enable-shared \
    --disable-static \
    --disable-doc \
    --disable-ffmpeg \
    --disable-ffplay \
    --disable-ffprobe \
    --disable-ffserver \
    --disable-avdevice \
    --disable-doc \
    --disable-symver \
    --cross-prefix=$TOOLCHAIN/bin/i686-linux-android- \
    --target-os=linux \
    --arch=x86 \
    --enable-cross-compile \
    --sysroot=$SYSROOT \
    --extra-cflags="-Os -fpic $ADDI_CFLAGS" \
    --extra-ldflags="$ADDI_LDFLAGS" \
    $ADDITIONAL_CONFIGURE_FLAG
    make clean
    make
    make install
 }
ADDI_CFLAGS="-march=k8"
build_one