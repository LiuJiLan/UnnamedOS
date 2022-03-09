# xv6-qemu
尝试xv6在qemu-system-riscv64的移植

# 1.资料对照
1.  有关代码中参考的操作系统

* **xv6** 指 
[xv6-public](https://github.com/mit-pdos/xv6-public)
> 基于x86体系, 现已不再维护。  
> 最后的版本为`Commits on Aug 11, 2020`   
> 个人习惯称这个版本为`xv6`

* **xv6-riscv** 指
[xv6-riscv](https://github.com/mit-pdos/xv6-riscv)
> 其与xv6相比更改了部分的实现  
> 我个人还是对xv6中的一些部分更加满意(比如console输出部分)

* **书中版本** 指
《操作系统原型——xv6分析与实验》中的xv6版本
> 没有去仔细对比, 但通过对比`bio.c`中的`binit()`函数,
> 基本可以确定书中版本应该是
> `Commits on Sep 12, 2016`版本之前的版本

* **rCore** 指
[rCore](https://github.com/rcore-os/rCore)
> 基于了SBI, 起始运行在0x8020_0000, S态  
> 而xv6-riscv则起始运行在0x8000_0000, M态  
> 但是要指出, **xv6-riscv的主核心还是在S态的**

* **rCore教学版** 指
[rCore-Tutorial-Book-v3](https://github.com/rcore-os/rCore-Tutorial-Book-v3)
> rCore的教学版本, 在地址映射方面和rCore有一些区别  
> 教程直接可以看[这里](https://rcore-os.github.io/rCore-Tutorial-Book-v3/)


# 2.环境设置

* **RISC-V编译器**
> 我们直接使用apt安装`gcc-riscv64-unknown-elf`
```
$ sudo apt update
$ sudo apt install gcc-riscv64-unknown-elf
```

* **GDB调试**
> 参考rCore教程[gdb相关部分](https://rcore-os.github.io/rCore-Tutorial-deploy/docs/pre-lab/gdb.html)  
> 暂不推荐使用`gdb-multiarch`, 虽然其可以直接apt安装, 
> 但是其使用中存在一些问题

* **QEMU模拟器**
> 参考rCore教程[qemu相关部分](https://rcore-os.github.io/rCore_tutorial_doc/chapter2/part5.html)  
> 注意: apt安装的qemu版本只有4.几  
> 这里放上我配环境的过程:
```
wget https://download.qemu.org/qemu-6.1.1.tar.xz
tar xvJf qemu-6.1.1.tar.xz
cd qemu-6.1.1
./configure --target-list=riscv64-softmmu,riscv32-softmmu
cd build
make -j$(nproc)
make install
```
>   
>  **可能出现的问题**
>  * ```ERROR: Cannot find Ninja```
>> 解决方法: ```sudo apt install ninja-build```
>
>  * ```ERROR: glib-2.56 gthread-2.0 is required to compile QEMU```
>> 参考[此处](https://blog.csdn.net/fuxy3/article/details/104732541)


# 3.项目结构

之后来补齐

# 4.To Do List

## To Do
* [ ] test

## Done


# 5.Note

## qemu -m 参数

手头版本使用`-m 6M`参数会导致dtb的地址为0x8000_0000, 取消此参数dtb的地址将会是默认值0x8700_0000。固我们暂时删除了这个参数。


# 6.XV6中改进点


1.  xv6采用了分页机制与虚拟地址的术语, 但没有实现页帧与磁盘的交换[P119]
>由于没有实现“页帧与磁盘的交换”, 所以没有理论上不会出现所谓缺页的情况, 这里**暂不清楚**xv6是否缺乏对缺页的处理[P152]
2.  xv6没有实现对x86内存总量的测定(`kinit1()`与`kinit2()`)[P120]
3.  xv6对物理页帧的管理非常简单(`kalloc()`与`kfree()`)[P126]
4.  xv6的进程亲缘关系组织比Linux要简单得多, 只有父进程关系, 无法直接知道自己的子进程和兄弟进程[P157]
5.  对比Linux的文件系统, 由于Linux的VFS使用页缓存和文件映射页,...。xv6fs与虚存无关, 因此要相对简单得多。[P333]
6.  ...xv6的块I/O层比较简单,...[P345]
7.  日志操作使得每个文件的写操作都分成两次操作, 降低了写效率。(不知道有没有方法改进)[P401]
8.  IDE驱动(访问硬盘的驱动)程序中并没有使用DMA方法,...[P444]
9.  (硬盘缓存)当块缓存数量很大时, ...线性搜索的方式是比较低效的, 可以考虑使用hash链...[P351]
10. (代码注释)...should be one superblock per disk device, ...run with only one device[P352]
11. (inode的iget函数)...发现没有空闲的缓存,...实际上xv6只是panic而没有进行回收[P359]
12. Linux系统的内存inode是VFS inode的抽象, 而磁盘上则可以是各种具体不同的ext2的inode、xfs的inode等等[P379]
13. ...xv6的设备操作非常简陋, 只有读写操作, 而没有Linux设备中的ioctl()等操作[P379]
