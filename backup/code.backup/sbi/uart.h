//
//  uart.h
//  xv6-qemu
//
//  Created by LiuJiLan on 2022/2/16.
//

#ifndef uart_h
#define uart_h

void uart_init(void);
int uart_putc(int ch);
int uart_getc(void);

#endif /* uart_h */
