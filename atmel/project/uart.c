#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

void uart_init(uint16_t baudrate){
	uint16_t UBRR_val = (F_CPU/16UL)/(baudrate-1);
	
	UBRR0H = (UBRR_val>>8);
	UBRR0L = UBRR_val;
	UCSR0B = (1<<RXEN0) | (1<<RXCIE0) | (1<<TXEN0);
	UCSR0C |= (1<<UCSZ00 | (1 << UCSZ01));
}

void uart_putc(unsigned char c){
	while(!(UCSR0A & (1<<UDRE0))); // wait until sending is possible
	UDR0 = c; // output character saved in c
}

void uart_puts(char *s){
	while(*s){
		uart_putc(*s);
		s++;
	}
}