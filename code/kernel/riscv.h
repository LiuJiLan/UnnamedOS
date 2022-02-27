//
//  riscv.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/18.
//

#ifndef riscv_h
#define riscv_h

static inline void satp(unsigned long address) {
    asm volatile("srli    t0, %0, 12;"
                 "li      t1, 0x8 << 60;"
                 "or      t0, t0, t1;"
                 "csrw    satp, t0;"
                 "sfence.vma;"
                 ://    无输出
                 :"r"(address)
                 :"t0", "t1");
}

static inline void clear_sstatus_SIE(void) {
    asm volatile("csrci   mstatus, 0x2;");
}

static inline void set_sstatus_SIE(void) {
    asm volatile("csrsi   mstatus, 0x2;");
}

static inline regs_t get_sstatus() {
    regs_t x;
    asm volatile("csrr %0, sstatus" : "=r" (x) );
    return x;
}


#endif /* riscv_h */
