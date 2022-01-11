# bootblock

- 第一版变量名、函数名尽量与xv6中保持一致

# 1.存在的问题

# 2.测试的问题

## 暂未上板子

## qemu测试 —— 测试用程序
- 测试程序是RVOS, 程序入口在0x80000000, 由bootblack由0x01800加载到位
- (程序入口这个值是由测试用elf文件决定的, 也可以是其他的值, 由bootblock去处理)
- 测试程序内容是向uart串口输出"Hello, RVOS!\n"

# 后记

## Error: offset too large
- 之前尝试把启动栈栈顶设置在0x803FFFF0, 然后报错```Error: offset too large```
- 这个错误提示很有误导性, 它让我们觉得问题出在la的那个目标太远了。
- 但我们翻阅[riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)的源码, 能在[这里](https://github.com/riscv-collab/riscv-binutils-gdb/blob/116a737f438d03a1bd6aa706b6ea0b4022f3b7e2/gas/testsuite/gas/riscv/lla64-fail.l)这里找到这个警告的文本文件。
- 返回上级目录能看到这份警告的文件注释:```RISC-V: Accept constant operands in la and lla```
- 问题很明确了,  **la后面只能接标号** 。

- 反思:
- x86写习惯了就总觉得寄存器、地址、立即数都能用一个命令搞定, 
- 会想当然的认为他们在数值上的值是相同的, 编译器会帮我们搞定编码的问题。
- 知道“Symbol”本质上就是一个地址, 而且在编程中有可能明确知道这个Symbol的地址, 
- 虽然这时```la t0, symbol```和```li t0, symbol的地址```对于t0的结果有着相同的作用, 但还是得按规范来。

