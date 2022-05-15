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



# Design

