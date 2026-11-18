#include<stdint.h>
#include "crc8.h"
#include "ringbuf.h"
#include "uart.h"

ringbuf_t uart_rx_buffer;
volatile uint32_t tick_ms = 0; //Declare tick_ms with the volatile keyword so the CPU always reads the fresh value from RAM.

const uint32_t delay = 333;

uint32_t current_time = 0;
uint32_t previous_time = 0;

uint32_t millis(void)
{
    return tick_ms;
}




void USART1_Interrupt_Init(void) {
    /* 1. Enable the RXNE [RX buffer not empty] (Read Data Register Not Empty) interrupt in USART1 */
    USART1->CR1 |= USART_CR_RXNEIE;

    /* 2. Set priority (0 = highest, 15 = lowest on STM32F100) and enable in NVIC */
    // NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_ISER->NVIC_ISER1 |= (1U << 5);/* Enable USART1 interrupt in NVIC (IRQ number 37, which is in ISER1) */
}
void USART1_SendString(const char *str) {
    while (*str) {
        // 1. Wait until Transmit Data Register Empty (TXE) flag is set
        while (!(USART1->SR & (1 << 7))) {
            // Wait
        }

        // 2. Load the next character into the Data Register
        USART1->DR = *str++;
    }

    // 3. Optional: Wait until Transmission Complete (TC) flag is set
    while (!(USART1->SR & (1 << 6))) {
        // Wait
    }
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
    	tick_ms++;

		current_time = millis();

		if(current_time - previous_time> delay){
			char data;
			if (USART1->SR & USART_SR_TXE)
		    {
		        // Wait until transmit data register is empty
		       if(ringbuf_pop(&uart_rx_buffer, &data)){
			        USART1->DR = data;
               }
			        // Send the received byte back
		    }
		    previous_time = current_time;
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
    USART1_SendString(
        "\r\nUART Bare-Metal Communication Ready!\r\n"
        "Type your message below at any pace the message will be displayed at steady pace of "
        "3 letters per second (3 LPS).\r\n\r\n"
    );
    //\r — Carriage Return (move cursor to beginning of line).
    // \n — Line Feed (move cursor down one line).
    while (1){
	}

}