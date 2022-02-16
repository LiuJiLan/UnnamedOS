//
//  uart.c
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/16.
//

#include "uart.h"

// 暂时使用的RVOS的代码

#define UART0 0x10000000L

#define UART_REG(reg) ((volatile unsigned char *)(UART0 + reg))


#define RHR 0    // Receive Holding Register (read mode)
#define THR 0    // Transmit Holding Register (write mode)
#define DLL 0    // LSB of Divisor Latch (write mode)
#define IER 1    // Interrupt Enable Register (write mode)
#define DLM 1    // MSB of Divisor Latch (write mode)
#define FCR 2    // FIFO Control Register (write mode)
#define ISR 2    // Interrupt Status Register (read mode)
#define LCR 3    // Line Control Register
#define MCR 4    // Modem Control Register
#define LSR 5    // Line Status Register
#define MSR 6    // Modem Status Register
#define SPR 7    // ScratchPad Register


#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

void uart_init()
{
    
    uart_write_reg(IER, 0x00);

    unsigned char lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg(DLL, 0x03);
    uart_write_reg(DLM, 0x00);

    lcr = 0;
    uart_write_reg(LCR, lcr | (3 << 0));

    unsigned char ier = uart_read_reg(IER);
    uart_write_reg(IER, ier | (1 << 0));
}

int uart_putc(int ch)
{
    while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
    return uart_write_reg(THR, ch);
}

void uart_puts(char *s)
{
    while (*s) {
        uart_putc(*s++);
    }
}

int uart_getc(void)
{
    if (uart_read_reg(LSR) & LSR_RX_READY){
        return uart_read_reg(RHR);
    } else {
        return -1;
    }
}

void uart_isr(void)
{
    while (1) {
        int c = uart_getc();
        if (c == -1) {
            break;
        } else {
            uart_putc((char)c);
            uart_putc('\n');
        }
    }
}
