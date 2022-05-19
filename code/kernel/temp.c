//
//  temp.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/19.
//

#include "proc.h"
#include "types.h"
#include "kalloc.h"
#include "vm.h"

extern void panic(char * s);

void sys_brk(struct proc * proc) {
    size_t request = proc->context.a0;
    regs_t ret = 0;
    
    if (request == 0) {
        ret = proc->PROC_BRK;
    } else if (request > proc->PROC_BRK) {
        uptr_t actual_end = proc->PROC_CODE_PG + proc->PROC_CODE_SZ * PGSIZE;
        if (request <= actual_end) {
            //  这里暂时设计的是小于等于
            //  PROC_BRK存的应该是地址的结尾
            //  (头闭尾开)
            proc->PROC_BRK = request;
            ret = 0;
        } else {
            uptr_t new_kva = (uptr_t)kalloc();
            //  BUG!!!!!!只默认增加了一个页
            if (new_kva) {
                vm_kva_map_uva(proc->upgtbl, new_kva, actual_end);
                //  actual_end一定是page对齐的
                //  BUG!!!!!!!!!
                //  有可能因为remap而导师这个函数返回-1
                //  但是我们忽略了它的返回值
                
                proc->PROC_CODE_SZ++;
                proc->PROC_BRK = request;
                ret = 0;
            } else {
                ret = -1;
            }
        }
    } else {    //  request <= 当前brk
        if (request < proc->PROC_END) {
            ret = -1;
        } else {
            //  BUG!!!!
            //  应该要回收
            //  (不会内存泄露, 但是这样如果进程不被kill就不会释放)
            proc->PROC_BRK = request;
            ret = 0;
        }
    }
    
    panic("sys_brk for debug");
    
    proc->context.a0 = ret;
    proc->context.sepc += 4;
}

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
};

static struct utsname un = {
    "UnnamedOS",
    "Unknown",
    "Unknown",
    "0.0.0",
    "RISC-V 64",
    "Unknown"
};

void sys_uname(struct proc * proc) {
    vm_memmove(proc->upgtbl, (uptr_t)&un, proc->context.a0, sizeof(struct utsname), 0);
    proc->context.a0 = 0;
    proc->context.sepc += 4;
}

void sys_times(struct proc * proc) {
    proc->context.a0 = 0;
    proc->context.sepc += 4;
}
