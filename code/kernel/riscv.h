//
//  riscv.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/25.
//

#ifndef riscv_h
#define riscv_h

#include "types.h"

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

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

static inline void sbi_set_timer(uint64 stime_value) {
    //  忽略返回值
    asm volatile("mv    a0, %0;"
                 "li    a7, 0x054494D45;"
                 "li    a6, 0x0;"
                 "ecall"
                 ://无输出
                 :"r"(stime_value)//输入
                 :"a6", "a7"//损坏
                 );
}

// enable device interrupts
static inline void intr_on() {
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// disable device interrupts
static inline void intr_off() {
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

// are device interrupts enabled?
static inline int intr_get() {
  uint64 x = r_sstatus();
  return (x & SSTATUS_SIE) != 0;
}

#endif /* riscv_h */
