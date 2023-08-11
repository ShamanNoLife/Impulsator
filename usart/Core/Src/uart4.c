/*
 * uart.c
 *
 *  Created on: Jul 21, 2023
 *      Author: Karol
 */
/*
 * pc10-tx
 * pc11-rx
 * */
#define SYS_Freq			2097000
#define UART_Baudrate		115200

#include"uart4.h"

static void uart_set_baudrate(USART_TypeDef *USARTx,uint32_t periph_clk, uint32_t baudrate);

void UART2_init(void){

	RCC->IOPENR  |= RCC_IOPENR_GPIOAEN;

	GPIOA->MODER = ((GPIOA->MODER & ~(GPIO_MODER_MODE2)) | (GPIO_MODER_MODE2_1));
	GPIOA->MODER = ((GPIOA->MODER & ~(GPIO_MODER_MODE3)) | (GPIO_MODER_MODE3_1));
	GPIOA->AFR[0]|=1U<<18;

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	uart_set_baudrate(USART2,SYS_Freq, UART_Baudrate);

	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART2->CR1 |= USART_CR1_RXNEIE;

//	NVIC_SetPriority(USART2_IRQn, 3);
//	NVIC_EnableIRQ(USART2_IRQn);

	USART2->CR1 |=USART_CR1_UE;
}

void uart_write(uint8_t *data){
	while(!(USART2->ISR & USART_ISR_TXE)){}
	USART2->TDR=*data;
}

uint8_t uart_read(USART_TypeDef *USARTx){
	while(!(USARTx->ISR & USART_ISR_RXNE)){}
	return USARTx->RDR;
}

static void uart_set_baudrate(USART_TypeDef *USARTx,uint32_t periph_clk, uint32_t baudrate){
double mantissa;
double fraction;

uint16_t USARTDIV_MANT;
uint16_t USARTDIV_FRAC;

mantissa = (double)periph_clk / (double)(baudrate*16);
fraction = ((mantissa - ((long)mantissa)))*16;
if (fraction - ((long)fraction) >= .5)
fraction++;
USARTDIV_MANT = mantissa;
USARTDIV_FRAC = fraction;
USARTx->BRR = (USARTDIV_FRAC << 0);
USARTx->BRR |= (USARTDIV_MANT << 4);
}




