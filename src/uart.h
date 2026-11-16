#ifndef UART_H
#define UART_H


#include<stdint.h>

#define USART_SR_TXE	(1 << 7)
#define USART_SR_RXNE	(1 << 5)

#define USART_CR_SBK	(1 << 0)
#define USART_CR_RWU	(1 << 1)
#define USART_CR_RE	(1 << 2)
#define USART_CR_TE	(1 << 3)
#define USART_CR_IDLEIE	(1 << 4)
#define USART_CR_RXNEIE	(1 << 5)
#define USART_CR_TCIE	(1 << 6)
#define USART_CR_TXEIE	(1 << 7)
#define USART_CR_PEIE	(1 << 8)
#define USART_CR_PS	(1 << 9)
#define USART_CR_PCE	(1 << 10)
#define USART_CR_WAKE	(1 << 11)
#define USART_CR_M	(1 << 12)
#define USART_CR_UE	(1 << 13)



struct USART {
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
};

struct NVIC_ISER
{
	volatile uint32_t NVIC_ISER0;
	volatile uint32_t NVIC_ISER1;
	volatile uint32_t NVIC_ISER2;
	volatile uint32_t NVIC_ISER3;
	volatile uint32_t NVIC_ISER4;
	volatile uint32_t NVIC_ISER5;
	volatile uint32_t NVIC_ISER6;
};


#define USART1        ((struct USART *) 0x40013800)
#define USART2        ((struct USART *) 0x40013800)
#define NVIC_ISER      ((struct NVIC_ISER *) 0xE000E100)


#endif
