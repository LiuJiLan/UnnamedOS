//
//  sbi_riscv.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/1/27.
//

#ifndef sbi_riscv_h
#define sbi_riscv_h

static inline void clear_mstatus_MIE(void) {
  asm volatile("csrci   mstatus, 0x8;");
}

static inline void set_mstatus_MIE(void) {
  asm volatile("csrsi   mstatus, 0x8;");
}

static inline regs_t get_mstatus() {
    regs_t x;
    asm volatile("csrr %0, mstatus" : "=r" (x) );
    return x;
}

#endif /* sbi_riscv_h */
