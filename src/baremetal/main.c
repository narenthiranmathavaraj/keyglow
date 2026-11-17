#include<stdint.h>
#include "crc8.h"
#include "ringbuf.h"
#include "uart.h"

ringbuf_t uart_rx_buffer;

void USART1_Interrupt_Init(void) {
    /* 1. Enable the RXNE [RX buffer not empty] (Read Data Register Not Empty) interrupt in USART1 */
    USART1->CR1 |= USART_CR_RXNEIE;

    /* 2. Set priority (0 = highest, 15 = lowest on STM32F100) and enable in NVIC */
    // NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_ISER->NVIC_ISER1 |= (1U << 5);/* Enable USART1 interrupt in NVIC (IRQ number 37, which is in ISER1) */
}


volatile char received_char;

void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        received_char = (char)(USART1->DR & 0xFF); //0xFF Bitmasking
		ringbuf_push(&uart_rx_buffer, received_char);
    }
}
void SYSTICK_Handler(void){
		char data;
		if (ringbuf_pop(&uart_rx_buffer, &data))
	    {
	        // Wait until transmit data register is empty
	       while(!(USART1->SR & USART_SR_TXE));
		        // Send the received byte back
		        USART1->DR = data;
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
	ringbuf_init(&uart_rx_buffer);
    USART1_Interrupt_Init();
    __asm volatile ("cpsie i"); // Enables interrupts globally

    while (1){
	}

}