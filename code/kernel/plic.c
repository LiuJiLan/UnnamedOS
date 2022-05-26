//
//  plic.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/26.
//

#include "plic.h"
#include "memlayout.h"
#include "riscv.h"
#include "uart.h"
#include "types.h"

#define PLIC P2V_WO(PLIC_BASE)

#define PLIC_PRIORITY           (PLIC + 0x0)
#define PLIC_PENDING            (PLIC + 0x1000)
#define PLIC_MENABLE(hart)      (PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart)      (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart)    (PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart)    (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart)       (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart)       (PLIC + 0x201004 + (hart)*0x2000)

void plic_init(void) {
    *(volatile uint32 *)(PLIC + UART0_IRQ * 4) = 1;
    //*(uint32*)(PLIC + VIRTIO0_IRQ*4) = 1;
}

void plic_init_hart(void) {
    int hart = (int)r_tp();
    
    //*(uint32*)PLIC_SENABLE(hart)= (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);
    *(volatile uint32 *)PLIC_SENABLE(hart)= (1 << UART0_IRQ);
    *(volatile uint32 *)PLIC_SPRIORITY(hart) = 0;
}

int plic_claim(void) {
    int hart = (int)r_tp();
    int irq = *(volatile uint32 *)PLIC_SCLAIM(hart);
    return irq;
}

void plic_complete(int irq) {
    int hart = (int)r_tp();
    *(volatile uint32 *)PLIC_SCLAIM(hart) = irq;
}
