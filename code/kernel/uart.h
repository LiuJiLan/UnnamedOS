//
//  uart.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/5/22.
//

#ifndef uart_h
#define uart_h

void uart_init(void);
int uart_putc(char ch);
void uart_intr(void);

#endif /* uart_h */
