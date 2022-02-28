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

xv6-qemu  
├── backup                  //  草稿的备份, 有一些掉过的坑, 感兴趣可以瞅瞅  
├── code                    //  代码  
│   ├── app (暂时还没有创建)  //用户程序  
│   ├── bootblock           // bootloader  
│   └── kernel              // 内核  
└── README.md  

# 3.To Do List

## To Do
* [ ] 实现SBI         entry.S
* [ ] li和la问题       见kernel README entry.S部分
* [ ] 替换掉锁里的编译器预设原子指令   见kernel spinlock部分
* [ ] cprintf中有关%x、%p的部分有误, 暂时是32位的状态   见sbi、kernel中的console部分
* [ ] 暂时使用的还是xv6fs

## Done


# 4.Note

## qemu -m 参数

手头版本使用`-m 6M`参数会导致dtb的地址为0x8000_0000, 取消此参数dtb的地址将会是默认值0x8700_0000。固我们暂时删除了这个参数。

# 5.Temp

## dtb解读
```sudo apt  install device-tree-compiler```


# 6.XV6中改进点

1.  xv6采用了分页机制与虚拟地址的术语, 但没有实现页帧与磁盘的交换[P119]
>由于没有实现“页帧与磁盘的交换”, 所以没有理论上不会出现所谓缺页的情况, 这里**暂不清楚**xv6是否缺乏对缺页的处理[P152]
2.  xv6没有实现对x86内存总量的测定(`kinit1()`与`kinit2()`)[P120]
3.  xv6对物理页帧的管理非常简单(`kalloc()`与`kfree()`)[P126]
4.  xv6的进程亲缘关系组织比Linux要简单得多, 只有父进程关系, 无法直接知道自己的子进程和兄弟进程[P157]
5.  对比Linux的文件系统, 由于Linux的VFS使用页缓存和文件映射页,...。xv6fs与虚存无关, 因此要相对简单得多。[P333]
6.  ...xv6的块I/O层比较简单,...[P345]
