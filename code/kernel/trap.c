//
//  trap.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/22.
//

#include "trap.h"
#include "riscv.h"
#include "proc.h"
#include "hart.h"
#include "types.h"
#include "syscall.h"

extern void panic(char *s);

void STIP_handler(struct trap_regs * regs);
void U_ECALL_handler(struct trap_regs * regs);

struct trap_regs * trap_handler(struct trap_regs * regs) {
    regs_t scause = regs->scause;
    
    if (scause & (0x1UL << 63)) {   //  中断
        scause &= ~(1UL << 63);
        switch (scause) {
            case Supervisor_Software_Interrupt:
                panic("SSIP!!!");
                break;
                
            case Supervisor_Timer_Interrupt:
                //panic("STIP");
                STIP_handler(regs);
                break;
                
            case Supervisor_External_Interrupt:
                panic("SEIP");
                break;
                
            default:
                break;
        }
        return regs;
    }
    
    switch (scause) {
        case Environment_Call_From_U_mode:
            U_ECALL_handler(regs);
            
            break;
            
        default:
            panic("S MODE Exception!");
            break;
    }
    
    return regs;
}

void STIP_handler(struct trap_regs * regs) {
    struct proc * myproc = my_hart()->myproc;
    if (myproc == NULL) {
        //  在我们现在的设计下,
        //  不应该出现一个CPU被STIP中断却没有拥有进程的情况
        //  因为sstatus.SIE只有在用户态才会被置1
        //  (S态不开sstatus.SIE)
        //  而一个CPU只有拥有了进程才有可能进入用户态
        panic("NO PROC BUT STIP!");
    }
    
    if (--myproc->remain_time == 0) {
        //  时间片到
        
        vm_2_kpgtbl();
        
        //  注意, 此处我们没有上锁就修改了时间片,
        //  是因为时间片一旦被创建就是一个进程绝对私有的
        //  同理进程的context也可以优先保存
        proc_context_copyin(regs, &myproc->context);
        
        pid_t my_pid = myproc->pid;
        my_hart()->myproc = NULL;
        
        char str[12] = "time out:0;";
        str[9] += my_pid;
        panic(str);
        
        proc_timeout(my_pid);
        proc_find_runnable_to_run(regs, my_pid);
    } else {
        //  时间片用完的情况在proc_find_runnable_to_run
        //  重设了时间片
        sbi_set_timer(DEFAULT_SBI_TIMER);
    }
}

void U_ECALL_handler(struct trap_regs * regs) {
    struct proc * myproc = my_hart()->myproc;
    pid_t mypid = myproc->pid;
    //  保存当前进程的context
    proc_context_copyin(regs, &myproc->context);
    
    proc_handle_syscall(regs, mypid);
}
