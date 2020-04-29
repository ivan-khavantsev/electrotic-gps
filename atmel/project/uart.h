#ifndef UART_H
#define UART_H

void uart_init(uint16_t baudrate);
void uart_putc(unsigned char c);
void uart_puts(char *s);

#endif