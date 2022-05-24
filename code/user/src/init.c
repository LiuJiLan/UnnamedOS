//
//  init.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/9.
//

#include "syscall.h"
#include "stddef.h"

//  早期版本有些不同

int main(void) {
    pid_t cpid = 0;
    
    getpid();
    getppid();
    
    //char buf[10] = "abcdefghij";
    
    //write(1, buf, 8);
    
    for (int i = 0; i < 3; i++) {
        cpid = fork();
        if (cpid == 0) {
            int count2 = 0;
            while (1) {
                int conut = 0x10000;
                while (conut--) {
                }
                count2++;
                getpid();
                if (i == 2) {
                    sched_yield();
                } else {
                    sleep(1);
                    return 0;
                }
                if (count2 == 3) {
                    return 0;
                }
            }
        }
    }
    
    cpid = fork();
    if (cpid == 0) {
        getpid();
        return 0;
    } else {
        waitpid(cpid, NULL, 0, NULL);
    }
    
    cpid = fork();
    if (cpid == 0) {
        while (1) {
            int conut = 0x10000;
            while (conut--) {
            }
            getpid();
            sleep(1);
        }
    }
    
    //size_t cur = brk(0);
    //brk(cur + 128);
    //brk(cur + 64);
    //brk(cur - 64);
    
    
    while(1) {
        //  由于sleep的bug, 我们现在必须要先这样子
        //  之后要设计开S态下的中断
        //waitpid(-1, NULL, 0x1, NULL);
        wait(NULL);
    }
    return 0;
}
