#include <stm32l476xx.h>
#include <lib.h>

#define BUTTON_PIN  (13)
#define BUTTON_BASE (GPIOC)
#define UART_BASE   (GPIOC)
#define UART_TX     (10)
#define UART_RX     (11)
// RX = Green (FOR PC not STM32, STM32 is TX)
// TX = White (FOR PC not STM32, STM32 is RX)
// GND = Black
// VCC = Red ( Not Connect )

int UART_Transmit(uint8_t *arr, uint32_t size){
	//TODO: Sent string to UART and return how many bytes are successfully transmitted.
	for(unsigned int i=0; i<size; i++){
		USART3->TDR = *(arr+i);
		while((USART3->ISR & USART_ISR_TC) == 0){}
	}
	return size;
}

void init_UART(){
	//TODO: Initialize UART registers
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;

	RCC->APB1RSTR1 |= RCC_APB1RSTR1_USART3RST; // RESET USART3
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USART3RST; // STOP RESET

	// === Turn off CLKEN, Due to asynchronous
	USART3->CR2 &= ~USART_CR2_CLKEN;

	// === Turn off Other Mode
	USART3->CR2 &= ~USART_CR2_LINEN;
	USART3->CR3 &= ~USART_CR3_SCEN;
	USART3->CR3 &= ~USART_CR3_IREN;
	USART3->CR3 &= ~USART_CR3_HDSEL;

	// === Set Over-sampling 16
	USART3->CR1 &= ~USART_CR1_OVER8;

	// === Set Baud Rate
	USART3->BRR &= ~USART_BRR_DIV_MANTISSA;
	USART3->BRR |= (26 << 4);
	USART3->BRR &= ~USART_BRR_DIV_FRACTION;
	USART3->BRR |= (0000 << 0);

	// === Set WorldLength(8bit)
	USART3->CR1 &= ~USART_CR1_M;

	// === Set StopBits(1bit)
	USART3->CR2 &= ~USART_CR2_STOP;

	// === Set Parity Bit(NO)
	USART3->CR1 &= ~USART_CR1_PCE;

	// === Set HardwareFlowControl(NO)
	USART3->CR3 &= ~USART_CR3_RTSE;
	USART3->CR3 &= ~USART_CR3_CTSE;

	// === Enable Rx/Tx
	USART3->CR1 |= USART_CR1_RE;
	USART3->CR1 |= USART_CR1_TE;

	// === Enable UART
	USART3->CR1 |= USART_CR1_UE;
}

void init_GPIO(){
	// ---- BUTTON
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	MODER_SETTING(BUTTON_BASE, BUTTON_PIN, MODER_I)e
	PUPDR_SETTING(BUTTON_BASE, BUTTON_PIN, PUPDR_PU)
	OSPEEDR_SETTING(BUTTON_BASE, BUTTON_PIN, OSPEEDR_HIGH)

	// ----- USART
	RCC->AHB2ENR |=	RCC_AHB2ENR_GPIOCEN;

	MODER_SETTING(UART_BASE, UART_TX, MODER_AF)
	OTYPER_SETTING(UART_BASE, UART_TX, OTYPER_PP)
	PUPDR_SETTING(UART_BASE, UART_TX, PUPDR_NO_PP)
	OSPEEDR_SETTING(UART_BASE, UART_TX, OSPEEDR_HIGH)

	MODER_SETTING(UART_BASE, UART_RX, MODER_AF)
	OTYPER_SETTING(UART_BASE, UART_RX, OTYPER_PP)
	PUPDR_SETTING(UART_BASE, UART_RX, PUPDR_NO_PP)
	OSPEEDR_SETTING(UART_BASE, UART_RX, OSPEEDR_HIGH)

	UART_BASE->AFR[1] &= ~GPIO_AFRH_AFSEL10;
	UART_BASE->AFR[1] |= GPIO_AFRH_AFSEL10_0;
	UART_BASE->AFR[1] |= GPIO_AFRH_AFSEL10_1;
	UART_BASE->AFR[1] |= GPIO_AFRH_AFSEL10_2; // AF7 // USART3 TX// PC10

	UART_BASE->AFR[1] &= ~GPIO_AFRH_AFSEL11;
	UART_BASE->AFR[1] |= GPIO_AFRH_AFSEL11_0;
	UART_BASE->AFR[1] |= GPIO_AFRH_AFSEL11_1;
	UART_BASE->AFR[1] |= GPIO_AFRH_AFSEL11_2; // AF7 // USART3 RX// PC11
}

int main(){
	init_UART();
	init_GPIO();
	while((GPIOC->IDR & 1<<BUTTON_PIN)!=0){}
	UART_Transmit("HELLO WORLD!\012\015", 14);

		/*
	while(1){
		if((GPIOC->IDR & 1<<BUTTON_PIN)==0)

	}
	*/
}
