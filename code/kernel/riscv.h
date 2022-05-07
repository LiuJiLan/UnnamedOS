//
//  riscv.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/25.
//

#ifndef riscv_h
#define riscv_h

#include "types.h"

static inline regs_t r_sstatus() {
  regs_t x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}

static inline void w_sstatus(regs_t x) {
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

//  读写tp
static inline regs_t r_tp() {
  regs_t x;
  asm volatile("mv %0, tp" : "=r" (x) );
  return x;
}

static inline void w_tp(regs_t x) {
  asm volatile("mv tp, %0" : : "r" (x));
}

static inline void w_satp(regs_t x) {
  asm volatile("csrw satp, %0" : : "r" (x));
}

static inline regs_t r_satp() {
  regs_t x;
  asm volatile("csrr %0, satp" : "=r" (x) );
  return x;
}

static inline void sfence_vma() {
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}

#endif /* riscv_h */
