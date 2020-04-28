#ifndef UART_H
#define UART_H

void USART_Init(void);
void USART_SendByte(uint8_t u8Data);
uint8_t USART_ReceiveByte();

#endif