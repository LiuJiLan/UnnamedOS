# xv6-qemu
尝试xv6在qemu-system-riscv64的移植

# 1.环境设置

如果选用Ubuntu 20.04的环境, 可以直接用apt来配环境。

```
$ sudo apt update
$ sudo apt install build-essential gcc make perl dkms git gcc-riscv64-unknown-elf gdb-multiarch qemu-system-misc
```

注意: apt安装的qemu版本只有4.几

## 安装 riscv64-unknown-elf-gdb

可以参考:
https://rcore-os.github.io/rCore-Tutorial-deploy/docs/pre-lab/gdb.html

## 安装 riscv64-unknown-elf-gdb

可以参考:
https://rcore-os.github.io/rCore_tutorial_doc/chapter2/part5.html
此处我选择了最新的stable版的qemu-6.1.1
```
wget https://download.qemu.org/qemu-6.1.1.tar.xz
tar xvJf qemu-6.1.1.tar.xz
cd qemu-6.1.1
./configure --target-list=riscv64-softmmu,riscv32-softmmu
cd build
make -j$(nproc)
make install
```


另外由于Maix Dock的k210的特权级版本是1.9, 所以涉及到一些处理, 可以参考
https://github.com/riscvarchive/riscv-qemu/wiki

### 本地编译出的问题:

```ERROR: Cannot find Ninja```
解决方法: ```sudo apt install ninja-build```

```ERROR: glib-2.56 gthread-2.0 is required to compile QEMU```
https://blog.csdn.net/fuxy3/article/details/104732541

# 2.项目结构

- xv6-qemu
- ├── backup                  //  草稿的备份, 有一些掉过的坑, 感兴趣可以瞅瞅
- ├── code                    //  代码
- │   ├── app (暂时还没有创建)  //用户程序
- │   ├── bootblock           // bootloader
- │   └── kernel              // 内核
- └── README.md

# 3.To Do List

## To Do
* [ ] 实现SBI         entry.S
* [ ] li和la问题       见kernel README entry.S部分

## Done


# 4.Note

## qemu -m 参数

- 手头版本使用`-m 6M`参数会导致dtb的地址为0x8000_0000, 
- 取消此参数dtb的地址将会是默认值0x8700_0000
- 固我们暂时删除了这个参数
