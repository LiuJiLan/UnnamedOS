# kernel

- 第一版变量名、函数名尽量与xv6中保持一致

# 笔记

## entry.S

### M态页表
- 根据[rCore教程](https://rcore-os.github.io/rCore-Tutorial-Book-v3/chapter4/3sv39-implementation-1.html)所说, M使用的地址是物理地址还是虚拟地址取决于硬件配置。

### 大页模式
- 大页模式下, offset会变长, 参考[rCore教程](https://rcore-os.github.io/rCore-Tutorial-Book-v3/chapter4/3sv39-implementation-1.html)
- 如果不完整的使用三级页表，未用到的页索引会和虚拟地址的12位默认页内偏移一起形成一个位数更多的**大页**页内偏移

