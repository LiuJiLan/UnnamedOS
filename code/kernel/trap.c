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
#include "time.h"
#include "plic.h"
#include "uart.h"

extern void panic(char *s);

void STIP_handler(struct trap_regs * regs);
void SEIP_handler(struct trap_regs * regs);
void U_ECALL_handler(struct trap_regs * regs);

struct trap_regs * trap_handler(struct trap_regs * regs) {
    regs_t scause = regs->scause;
    
    /*
     // FOR DEBUG
     regs_t x = r_sstatus();
     if (x & SSTATUS_SPP) {
     panic("TRAP FROM S.");
     } else {
     panic("TRAP FROM U.");
     }
     */
    
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
                SEIP_handler(regs);
                panic("SEIP_finish");
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
    if (r_tp() == 0) {
        time_tick();
        time_ring_clock();
    }
    
    struct proc * myproc = my_proc();
    if (myproc == NULL) {
        //  注意实际上我们应该用sstatus.SPP来作为判断依据
        //  但是由于我们需要设备中断来初始化一些东西
        //  我们之前的设计中myproc是不会在STIP中为NULL的
        
        regs_t x = r_sstatus();
        if ((x & SSTATUS_SPP) == 0) {
            //  如果发生就是真的PANIC
            panic("REAL PANIC!!!");
            panic("STIP with NULL proc in U-Mode.");
        }
        
        panic("STIP NULL proc S-Mode.");
        sbi_set_timer(DEFAULT_INTERVAL);
        return;
    }
    
    regs_t x = r_sstatus();
    if (x & SSTATUS_SPP) {
        panic("REAL PANIC!!!");
        panic("STIP with A proc in S-Mode.");
    }
    
    if (--myproc->remain_time == 0) {
        //  时间片到
        
        //  注意, 此处我们没有上锁就修改了时间片,
        //  是因为时间片一旦被创建就是一个进程绝对私有的
        //  同理进程的context也可以优先保存
        proc_context_copyin(regs, &myproc->context);
        
        pid_t my_pid = myproc->pid;
        
        //  FOR DEBUG
        char str[12] = "time out:0;";
        str[9] += my_pid;
        panic(str);
        
        proc_timeout(my_pid);
        proc_reschedule(my_pid);
    } else {
        //  时间片用完的情况在proc_find_runnable_to_run
        //  重设了时间片
        sbi_set_timer(DEFAULT_INTERVAL);
    }
}

void SEIP_handler(struct trap_regs * regs) {
    int irq = plic_claim();
    
    if (irq == UART0_IRQ) {
        uart_intr();
    //} else if(irq == VIRTIO0_IRQ){
        //virtio_disk_intr();
    } else if(irq){
        char str[6] = "irq:0";
        str[4] = '0' + irq;
        panic(str);
    }
    
    if(irq) {
        plic_complete(irq);
    }
}

void U_ECALL_handler(struct trap_regs * regs) {
    struct proc * myproc = my_proc();
    pid_t mypid = myproc->pid;
    //  保存当前进程的context
    proc_context_copyin(regs, &myproc->context);
    
    proc_handle_syscall(regs, mypid);
}
