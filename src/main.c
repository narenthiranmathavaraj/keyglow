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
#define NVIC_ISER      ((struct NVIC_ISER *) 0xE000E100)


void USART1_Interrupt_Init(void) {
    /* 1. Enable the RXNE [RX buffer not empty] (Read Data Register Not Empty) interrupt in USART1 */
    USART1->CR1 |= USART_CR_RXNEIE;

    /* 2. Set priority (0 = highest, 15 = lowest on STM32F100) and enable in NVIC */
    // NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_ISER->NVIC_ISER1 |= (1U << 5);/* Enable USART1 interrupt in NVIC (IRQ number 37, which is in ISER1) */
}


volatile uint8_t received_char;

void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        received_char = (uint8_t)USART1->DR;
    }
}


void uart_init(void)
{

	USART1->CR1 = USART_CR_UE | USART_CR_TE | USART_CR_RE; // no effect in simulation, but needed for real hardware
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	USART1->GTPR = 0; //Adds a "Breathing Room" Pause
	USART1->BRR = (8000000U + (115200U / 2U)) / 115200U; // Evaluates to 69 (0x45)
}
void main(void) {
    uart_init();
    USART1_Interrupt_Init();
    // const char *s = "hello\n";

    // while (*s) {
    //     USART1->DR = *s++;
    // }
    __asm volatile ("cpsie i"); // Enables interrupts globally
    while (1);

}