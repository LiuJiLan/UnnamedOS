//
//  sbi_riscv.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/4/18.
//

#include "sbi_types.h"

#ifndef sbi_riscv_h
#define sbi_riscv_h

//  读写mstatus
static inline regs_t r_mstatus() {
  regs_t x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}

static inline void w_mstatus(regs_t x) {
  asm volatile("csrw mstatus, %0" : : "r" (x));
}

//  写mtvec
static inline void w_mtvec(regs_t x) {
  asm volatile("csrw mtvec, %0" : : "r" (x));
}

//  读写medeleg & mideleg
static inline regs_t r_medeleg() {
  regs_t x;
  asm volatile("csrr %0, medeleg" : "=r" (x) );
  return x;
}

static inline void w_medeleg(regs_t x) {
  asm volatile("csrw medeleg, %0" : : "r" (x));
}

static inline regs_t r_mideleg() {
  regs_t x;
  asm volatile("csrr %0, mideleg" : "=r" (x) );
  return x;
}

static inline void w_mideleg(regs_t x) {
  asm volatile("csrw mideleg, %0" : : "r" (x));
}

//  读写mie & mip
static inline regs_t r_mie() {
  regs_t x;
  asm volatile("csrr %0, mie" : "=r" (x) );
  return x;
}

static inline void w_mie(regs_t x) {
  asm volatile("csrw mie, %0" : : "r" (x));
}

//  读写mie
static inline regs_t r_mip() {
  regs_t x;
  asm volatile("csrr %0, mip" : "=r" (x) );
  return x;
}

static inline void w_mip(regs_t x) {
  asm volatile("csrw mip, %0" : : "r" (x));
}

//  写mepc
static inline void w_mepc(regs_t x) {
  asm volatile("csrw mepc, %0" : : "r" (x));
}

//  写pmpaddr0 & pmpcfg0
static inline void w_pmpcfg0(regs_t x) {
  asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline void w_pmpaddr0(regs_t x) {
  asm volatile("csrw pmpaddr0, %0" : : "r" (x));
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

#endif /* sbi_riscv_h */
