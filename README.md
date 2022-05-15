# Unnamed Operating System Project

**PS: 系统不叫Unnamed, 我是真的没想好名字。**



## 注意事项

1. 本文件由MarkDown编辑器软件自动排版, Raw Code会略显混乱, 使用相应软件或网站查看本文档以获得更好的体验。
2. 本文件中可能会使用Mermaid渲染图片。使用集成了Mermaid渲染功能的Markdown编辑器或网站查看本文档以获得更好的体验。
3. 本文件编辑时使用的主题是Typora的Github(浅色主题)与Night(深色主题)。如果使用Typora可以选择这两个主题以获得与作者相同的观看体验。



## 项目介绍

1. 本项目试图实现一个运行在RISC-V 64架构下的Unix-like操作系统内核与部分用户态程序。
2. 本项目有两个主要目的: 一个是为了[操作系统比赛](https://os.educg.net)(OS内核实现赛道), 另一个是为了我个人对CPU、操作系统与编译器的一些练习。所以在本项目中可能会看到面对内核赛与我个人目标的一些不同的实现。
3. 本项目对于我个人的练习目的而言: 先实现操作系统, 然后尽量精简操作系统所使用的指令与外部设备, 依据这些指令与外设用FPGA实现一个基于RISC-V ISA的子集SoC系统, 然后实现一个基于这个操作系统的自己写的编译器。
4. 本项目曾用名`xv6-qemu`, 正如其名, 原本是想对[xv6-public](https://github.com/mit-pdos/xv6-public)进行模仿, 但是最终秉承着**改别人代码不如自己重头写**的观点推翻重做。在本项目也学习了[xv6-riscv](https://github.com/mit-pdos/xv6-riscv)(部分函数, 页表函数的部分处理)、[rCore](https://github.com/rcore-os/rCore)(高地址内存的分布)、[RVOS](https://github.com/plctlab/riscv-operating-system-mooc)(UART、CLINT、PLIC的一些寄存器读写)与[内核赛测试用例](https://gitee.com/oscomp/testsuits-for-oskernel/tree/main)(用户态的crt, 部分库)。
5. 除主要目标之外, 还希望本项目能给他人一些参考, 做一些微小的贡献。



## 环境配置

本项目在Ubuntu系统上编译、模拟、调试。

> 我个人是在Parallels Desktop虚拟机上跑的Ubuntu, 平时写代码用的是Xcode。
>
> - PS: 不知道用M1芯片的Mac能不能正常跑虚拟机并运行, 我用的是Intel芯片的。
> - PS: 用macOS的同学不要用macOS本身来编译与模拟, 即使你用brew安了gcc也不要这样做。你可能遇到一些意想不到的问题。(例: macOS和Windows一样不区分大小写)
> - PS: Windows用户强烈不推荐WSL, 另外文件尽量不要用Windows来中转。如果很不幸不能直接在虚拟机中下载, 也请直接导压缩包然后去Linux中去解压。规避一些文件被转换的命运。

用到的工具有:

- gcc-riscv64-unknown-elf
- QEMU
- riscv64-unknown-elf-gdb



### GCC的安装

apt安装即可。



### QEMU的安装

极其不推荐apt的版本。强烈推荐源码下载编译, 可以参考一下[rCore教程](https://rcore-os.github.io/rCore_tutorial_doc/chapter2/part5.html)。但不得不说下载编译很麻烦。

可能遇到的问题如下:

- ERROR: Cannot find Ninja

  解决方法: ```sudo apt install ninja-build```

- ERROR: glib-2.56 gthread-2.0 is required to compile QEMU

  参考[这个博客](https://blog.csdn.net/fuxy3/article/details/104732541)

我个人的安装过程给一个参考:

```shell
wget https://download.qemu.org/qemu-6.1.1.tar.xz
tar xvJf qemu-6.1.1.tar.xz
cd qemu-6.1.1
./configure --target-list=riscv64-softmmu,riscv32-softmmu
cd build
make -j$(nproc)
make install
```

> PS: qemu对RISC-V的模拟本身就存在一些问题, 可以去给他们提issue。



### GDB的安装

首先不推荐使用RVOS所使用的gdb-multiarch, 使用中确实出了不少问题, 不知道现在是否已经修复了。

个人推荐使用riscv64-unknown-elf-gdb。其安装可以参考[rCore教程](https://rcore-os.github.io/rCore-Tutorial-deploy/docs/pre-lab/gdb.html), 但我个人不推荐使用教程里的那个插件。



## 项目结构

主要的代码都在code文件夹下。

backup文件夹或者标有`.back`或者`.backup`都是写废的文件。有一些掉过的坑, 感兴趣可以瞅瞅, 说不定能解决一些问题。

script里是一些脚本, 一些方便开发用的, **一些参与了编译**(例如将一些代码内嵌进内核)。xv6用Perl, 我则用了Python。



# Design



