#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "uart.h"
#include "ringbuf.h"
#include <stdint.h>


ringbuf_t uart_rx_buffer;
volatile char received_char;
static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;

// Settings
static const uint8_t buf_len = 20;

// Globals
static int led_delay = 500;   // ms


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

void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_RXNE)
    {
        received_char = (char)(USART1->DR & 0xFF); //0xFF Bitmasking
		ringbuf_push(&uart_rx_buffer, received_char);
    }
}


void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    (void)task;
    (void)name;

    while (1)
    {
        // Stack overflow detected
    }
}

void vApplicationIdleHook(void)
{
    __asm volatile ("wfi"); //wait for interrupt
}


volatile char received_char;

void uart_init(void)
{

	USART1->CR1 = USART_CR_UE | USART_CR_TE | USART_CR_RE; // no effect in simulation, but needed for real hardware
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	USART1->GTPR = 0; //Adds a "Breathing Room" Pause
	USART1->BRR = (8000000U + (115200U / 2U)) / 115200U; // Evaluates to 69 (0x45)
}


void serial_input(void *arg)
{
    (void)arg;
    char c;
    char buf[buf_len];
    memset(buf, 0, buf_len)
    while (1)
    {
        if (USART1->SR & USART_SR_RXNE)
            {
                received_char = (char)(USART1->DR & 0xFF); //0xFF Bitmasking
        		ringbuf_push(&uart_rx_buffer, received_char);
            }
    }
}
void backlight_state(void *arg)
{
    bool LED_state = 0;
    while (1)
    {
            LED_state = !LED_state;
            USART1_SendString("LED ON\n");\
            vTaskDelay(1000/portTICK_PERIOD_MS);
            LED_state = !LED_state;
            USART1_SendString("LED OFF\n");\
            vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
int main(void)
{
    uart_init();
    BaseType_t result1 = xTaskCreate(
        serial_input,
        "Serial Input",
        128,
        NULL,
        1,
        &task1
    );
    BaseType_t result2 = xTaskCreate(
        backlight_state,
        "Backlight State",
        128,
        NULL,
        1,
        &task2
    );
    vTaskStartScheduler();

    while (1);
}

