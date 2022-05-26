//
//  uart.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/22.
//

#ifndef uart_h
#define uart_h

#define UART0_BASE 0x10000000UL
#define UART0_SIZE 0x100UL
#define UART0_IRQ 10

void uart_init(void);
int uart_putc(char ch);
void uart_intr(void);

#endif /* uart_h */
