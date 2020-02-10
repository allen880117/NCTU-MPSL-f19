#include <stm32l476xx.h>
#include <core_cm4.h>
#include <lib.h>

#define LED_PIN (9)
#define WAIT_TIME ((uint32_t)1000000*3)

void GPIO_init(){
	RCC->AHB2ENR |=	RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= (~(0b11 << LED_PIN*2));
	GPIOA->MODER |= (MODER_O << LED_PIN*2);
	GPIOA->OSPEEDR &= (~(0b11 << LED_PIN*2));
	GPIOA->OSPEEDR |= (OSPEEDR_HIGH << LED_PIN*2);
}

void SystemClock_Config(){
	// === Set SYSCLK
	// TURN ON HSI
	RCC->CR &= ~RCC_CR_HSION;
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);

	// Set SYSCLK SRC = HSI
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV16; // Down to 1MHz

	// === SysTick Timer Setting
	SysTick->LOAD = (uint32_t)(WAIT_TIME - 1UL);
	SysTick->VAL = 0UL;

	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL |= 1 << SysTick_CTRL_CLKSOURCE_Pos; // Processor Clock
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= 1 << SysTick_CTRL_TICKINT_Pos; // Assert Exception

	// Set priority
	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

	// SysTick Timer Enable
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL |= 1 << SysTick_CTRL_ENABLE_Pos;

	// The Following Function Has Equal-Effect with Above SysTick Timer Setting
	// SysTick_Config(WAIT_TIME-1);
}

void SysTick_Handler(void){
	unsigned int cur = (GPIOA->ODR & (1<<LED_PIN));
	if(cur == 0) GPIOA->BSRR |= 1<<LED_PIN;
	else         GPIOA->BRR  |= 1<<LED_PIN;
}

int main(){
	SystemClock_Config();
	GPIO_init();
	while(1){;}
}
