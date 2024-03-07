## qemu

```
sudo apt-get install autoconf automake autotools-dev ninja-build pkg-config libglib2.0
git clone https://github.com/qemu/qemu.git
cd qemu
mkdir build
cd build
./configure --target-list=aarch64-linux-user --disable-system
make
```

## toolchain

```
wget https://releases.linaro.org/components/toolchain/binaries/latest-7/aarch64-linux-gnu/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu.tar.xz
tar xvf ./gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu.tar.xz --strip-components 1
ln -s gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu toolchain
```

## env

```
export PATH=toolchain/bin:$PATH
export PATH=qemu/build/bin:$PATH
export QEMU_LD_PREFIX=toolchain/aarch64-linux-gnu/libc
```
