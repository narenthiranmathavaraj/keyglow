#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "ringbuf.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "semphr.h"

ringbuf_t uart_rx_buffer;
volatile char received_char;
static TaskHandle_t welcome_screen_task = NULL;
static TaskHandle_t rtos_monkeytype_task = NULL;
static TaskHandle_t rtos_motivation_task = NULL;
volatile uint8_t uart_buf_locked = 1;
SemaphoreHandle_t uart_rx_semaphore;


volatile char received_char;

typedef enum{
    MOTO,
    WPM,
    INVALID
} user_input_task;


typedef struct {
    BaseType_t WPM;
    BaseType_t accuracy;
} wpm_t;

void int_to_string(size_t num, char *str)
{
    BaseType_t i = 0;
    BaseType_t negative = 0;

    if (num < 0)
    {
        negative = 1;
        num = -num;
    }

    do
    {
        str[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    if (negative)
        str[i++] = '-';

    str[i] = '\0';

    // Reverse the string
    for (BaseType_t j = 0; j < i / 2; j++)
    {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
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


void USART1_SendString_RTOS(const char *str, TickType_t delay) {
    while (*str) {
        // 1. Wait until Transmit Data Register Empty (TXE) flag is set
        while (!(USART1->SR & (1 << 7))) {
            // Wait
        }

        // 2. Load the next character into the Data Register
        USART1->DR = *str++;
        vTaskDelay(delay);
    }

    // 3. Optional: Wait until Transmission Complete (TC) flag is set
    while (!(USART1->SR & (1 << 6))) {
        // Wait
    }
}


void USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (USART1->SR & USART_SR_RXNE)
    {
        received_char = (char)(USART1->DR & 0xFF);

        if (uart_buf_locked == 0)
        {
            ringbuf_push(&uart_rx_buffer, received_char);

            xSemaphoreGiveFromISR(
                uart_rx_semaphore,
                &xHigherPriorityTaskWoken
            );
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //requests a context switch after the ISR.
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


BaseType_t countWords(const char *str) {
    BaseType_t count = 0;
    bool inWord = false;

    for (BaseType_t i = 0; str[i] != '\0'; i++) {
        // Cast to unsigned char to safely pass to isspace()
        if (isspace((unsigned char)str[i])) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            count++;
        }
    }
    return count;
}

BaseType_t WPM_accuracy(const char *str) {
    char *text = "The quick brown fox jumps over the lazy dog.";
    BaseType_t len_text = 44;
    BaseType_t correct = 0;
    float accuracy = 0;
    for (BaseType_t i = 0; i < len_text; i++) {
        if(text[i] == str[i])
            correct++;
    }
    return correct;
}


void WPM_score(const char *str, wpm_t *wpm_score){
    BaseType_t no_of_words = countWords(str);
    BaseType_t accuracy = WPM_accuracy(str);
    float acc_f = (((float)accuracy)/44)*100;
    wpm_score->accuracy = (BaseType_t)acc_f;
    wpm_score->WPM = no_of_words;
}


uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}


uint32_t random_n(uint32_t *state, uint32_t n) {
    if (n == 0) {
        return 0; // Prevent division-by-zero fault
    }
    return xorshift32(state) % n;
}


void rtos_motivation(void *arg)
{

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        const char *quotes[] = {
            "Take the mutex of your dreams. Don't let failure acquire it! \r\n"
            "                                        -RTOS Master Oogway",

            "If life blocks you, don't panic. Just wait for the next notification!\r\n"
            "                                                 -RTOS Master Oogway",

            "Be like a high-priority task: know what matters and execute!\r\n"
            "                                        -RTOS Master Oogway",


            "Don't busy-wait for success. Keep working while the scheduler handles the rest!\r\n"
            "                                                           -RTOS Master Oogway",


            "When life suspends you, remember: vTaskResume() is always an option!\r\n"
            "                                                -RTOS Master Oogway",


            "Protect your happiness with a mutex. Not everyone deserves access!\r\n"
            "                                              -RTOS Master Oogway",


            "Even the smallest task can achieve greatness. Just give it enough stack!\r\n"
            "                                                    -RTOS Master Oogway",


            "Don't let a deadlock stop your dreams. Find a way forward and keep moving!\r\n"
            "                                                      -RTOS Master Oogway",


            "Life is a scheduler. Stay Ready, keep Running, and never remain Blocked!\r\n"
            "                                                    -RTOS Master Oogway"

        };
        uint32_t seed = xTaskGetTickCount() ^ (uint32_t)xTaskGetCurrentTaskHandle();
        uint32_t idx = random_n(&seed, 9);
        bool task_finished = false;
		while(!task_finished)
	    {
            uart_buf_locked = 1;
            USART1_SendString_RTOS("\r\n",pdMS_TO_TICKS(50));
            USART1_SendString_RTOS(quotes[idx],pdMS_TO_TICKS(50));
            USART1_SendString_RTOS("\r\n",pdMS_TO_TICKS(50));
            uart_buf_locked = 0;
            task_finished = true;
            xTaskNotifyGive(welcome_screen_task);
        }
    }
}


void rtos_monkeytype(void *arg)
{
   while(1)
   {
        uart_buf_locked = 1;
        // Sleep until user selects WPM

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        USART1_SendString_RTOS(
        "\r\n"
        "========================================\r\n"
        "          RTOS MONKEYTYPE TEST\r\n"
        "========================================\r\n"
        "\r\n"
        "Get ready to test your typing speed!\r\n"
        "Type the passage below as quickly and accurately as possible.\r\n"

        "\r\n"
        "The quick brown fox jumps over the lazy dog."
        "\r\n"
        "\r\n"

        "Start typing now [No Backspace Allowed]! Good WPM: >50 and Accuracy: 95% Press ENTER when finished.\r\n"
        "\r\n"
        ,pdMS_TO_TICKS(10)
       );
        uart_buf_locked = 0;

        char temp[100] = "";
        wpm_t wpm_score;
        char temp_WPM[10];
        char temp_acc[10];
        bool task_finished = false;
        TickType_t start_time = pdMS_TO_TICKS(xTaskGetTickCount());
		while(!task_finished)
        {
           char data;
    		if (USART1->SR & USART_SR_TXE)
    	    {
    	        // Wait until transmit data register is empty
               xSemaphoreTake(uart_rx_semaphore, portMAX_DELAY);
    	       if(ringbuf_pop(&uart_rx_buffer, &data)){
                    strncat(temp, &data, 1); // Reads only 1 byte from ch and adds '\0'
                    if(data == '\r'){
                        TickType_t finish_time = pdMS_TO_TICKS(xTaskGetTickCount());
                        float total_time = (float)(finish_time-start_time);
                        WPM_score(temp, &wpm_score);
                        float actual_wpm = (((float)(wpm_score.WPM))/(total_time))*60000;
                        memset(temp, 0, sizeof(temp));
                        uart_buf_locked = 1;
                        USART1_SendString_RTOS("\r\n",pdMS_TO_TICKS(10));
                        USART1_SendString_RTOS("Your WPM: ",pdMS_TO_TICKS(10));
                        int_to_string(actual_wpm, temp_WPM);
                        USART1_SendString_RTOS(temp_WPM,pdMS_TO_TICKS(10));
                        USART1_SendString_RTOS(" accuracy: ",pdMS_TO_TICKS(10));
                        int_to_string(wpm_score.accuracy, temp_acc);
                        USART1_SendString_RTOS(temp_acc,pdMS_TO_TICKS(10));
                        USART1_SendString_RTOS("\r\n",pdMS_TO_TICKS(10));
                        uart_buf_locked = 0;
                        memset(temp, 0, sizeof(temp));
                        task_finished = true;
                        xTaskNotifyGive(welcome_screen_task);
                    }
    		        USART1->DR = data;
               }
    	    }
        }
   }

}


void welcome_screen(void *arg)
{
    while (1)
    {
        uart_buf_locked = 1;

        USART1_SendString_RTOS("\r\n"
        "Choose your typing challenge:\r\n"
        "\r\n"
        "1. WPM Test (Words Per Minute)\r\n"
        "2. Or Wanna Hear a RTOS Motivation Quote ...\r\n"
        "\r\n"
        "Enter your choice (1 or 2): "
        "\r\n",pdMS_TO_TICKS(10));
        uart_buf_locked = 0;
        char temp[100] = "";
        user_input_task task = INVALID;
        char data;
        bool task_finished = false;
		while(!task_finished)
	    {
            if (USART1->SR & USART_SR_TXE)
            {
                xSemaphoreTake(uart_rx_semaphore, portMAX_DELAY);
                if(ringbuf_pop(&uart_rx_buffer, &data)){
                    strncat(temp, &data, 1); // Reads only 1 byte from ch and adds '\0'
                    if(data == '\r'){

                        if(!strcmp(temp, "1\r")){
                            task = WPM;
                        }
                        else if (!strcmp(temp, "2\r"))
                        {
                            task = MOTO;
                        }
                        else{
                            task = INVALID;
                        }

                        switch (task)
                        {
                            case WPM:
                                uart_buf_locked = 1;
                                USART1_SendString("\r\n");
                                xTaskNotifyGive(rtos_monkeytype_task);
                                ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Pauses welcome_screen_task until quote finishes

                                break;
                            case MOTO:
                                uart_buf_locked = 1;
                                USART1_SendString("\r\n");
                                xTaskNotifyGive(rtos_motivation_task);   // Wakes rtos_motivation
                                ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Pauses welcome_screen_task until quote finishes

                                break;
                            default:
                                uart_buf_locked = 1;
                                USART1_SendString("\r\n");
                                USART1_SendString_RTOS("\r\n"
                                "INVALID CHOICE\r\n",pdMS_TO_TICKS(10));
                                uart_buf_locked = 0;
                                break;
                        }

                        memset(temp, 0, sizeof(temp));
                        task_finished = true;
                    }
    		        USART1->DR = data;
               }
            }
	    }
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


void main(void)
{

    uart_init();
	ringbuf_init(&uart_rx_buffer);
    USART1_Interrupt_Init();
    USART1_SendString(
    "\r\n========================================\r\n"
    "       RTOS SPEED TYPING CHATBOT\r\n"
    "========================================\r\n"
    "\r\n"
    "Welcome to the RTOS Speed Typing Challenge!\r\n"
    "Improve your typing speed, accuracy, and consistency.\r\n"
    "\r\n"
    );


    uart_rx_semaphore = xSemaphoreCreateBinary();
    BaseType_t result1 = xTaskCreate(
        welcome_screen,
        "Welcome Screen",
        128,
        NULL,
        3,
        &welcome_screen_task
    );


    BaseType_t result2 = xTaskCreate(
        rtos_monkeytype,
        "RTOS Monkey Type",
        128,
        NULL,
        2,
        &rtos_monkeytype_task
    );

    BaseType_t result3 = xTaskCreate(
        rtos_motivation,
        "RTOS Motivation",
        128,
        NULL,
        1,
        &rtos_motivation_task
    );

    if (result1 != 0 && result2 != 0  && result3 != 0){
       vTaskStartScheduler();
    }
    while (1);
}

