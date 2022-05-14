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
    
    for (int i = 0; i < 3; i++) {
        cpid = fork();
        if (cpid == 0) {
            while (1) {
                int conut = 0xFFFFFF;
                while (conut--) {
                }
                getpid();
                if (i == 2) {
                    sched_yield();
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
            int conut = 0xFFFFFF;
            while (conut--) {
            }
            getpid();
        }
    }
    
    while(1) {
        
    }
    return 0;
}
