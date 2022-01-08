# xv6-qemu
尝试xv6在qemu-system-riscv64的移植

# 1.环境设置

如果选用Ubuntu 20.04的环境, 可以直接用apt来配环境。

```
$ sudo apt update
$ sudo apt install build-essential gcc make perl dkms git gcc-riscv64-unknown-elf gdb-multiarch qemu-system-misc
```

去年用的riscv64-unknown-elf-gdb但由于要源码编译就暂时懒得用了。
