#include <stm32l476xx.h>
#include <core_cm4.h>
#include <lib.h>

#define NEWTYPE 1

#define BUZZER_PIN (0)
#define BUZZER_BASE (GPIOA)

#define BUTTON_PIN (13)
#define BUTTON_BASE (GPIOC)

#define X_ODR (GPIOA->ODR)
#define X_BASE (GPIOA)
#define X0 (5)
#define X1 (6)
#define X2 (7)
#define X3 (8)

#define Y_IDR (GPIOB->IDR)
#define Y_BASE (GPIOB)
#define Y0 (12)
#define Y1 (13)
#define Y2 (14)
#define Y3 (15)

unsigned int x_mask = (0x01<<X0) + (0x01<<X1) + (0x01<<X2) + (0x01<<X3);
unsigned int x_pin[4] = {X0, X1, X2, X3};
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3};

void keypad_full_trigger(){
	// Keypad Full-Trigger to Get Press Action
	for(int i=0; i<4; i++){
		X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[i]*2))) | (MODER_O << x_pin[i]*2);
		X_ODR = 0;
	}
}

void keypad_init(){
	// X as OUTPUT // OUTPUT GPIOA 5 6 7 8
	RCC->AHB2ENR = RCC->AHB2ENR | (0x1 << 0);
	for(int i=0; i<4; i++){
		X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[i]*2))) | (MODER_O << x_pin[i]*2);
		X_BASE->OTYPER = (X_BASE->OTYPER & (~(0x1 << x_pin[i]))) | (OTYPER_OD << x_pin[i]);
		X_BASE->PUPDR = (X_BASE->PUPDR & (~(0x3 << x_pin[i]*2))) | (PUPDR_PU << x_pin[i]*2);
		X_BASE->OSPEEDR = (X_BASE->OSPEEDR & (~(0x3 << x_pin[i]*2))) | (OSPEEDR_MEDIUM << x_pin[i]*2);
	}

	// Y as INPUT // INPUT GPIOB 12 13 14 15
	RCC->AHB2ENR = RCC->AHB2ENR | (0x1 << 1);
	for(int i=0; i<4; i++){
		Y_BASE->MODER = (Y_BASE->MODER & (~(0x3 << y_pin[i]*2))) | (MODER_I << y_pin[i]*2);
		Y_BASE->OTYPER = (Y_BASE->OTYPER & (~(0x1 << y_pin[i]))) | (OTYPER_PP << y_pin[i]);
		Y_BASE->PUPDR = (Y_BASE->PUPDR & (~(0x3 << y_pin[i]*2))) | (PUPDR_PU << y_pin[i]*2);
		Y_BASE->OSPEEDR = (Y_BASE->PUPDR & (~(0x3 << y_pin[i]*2))) | (OSPEEDR_MEDIUM << y_pin[i]*2);
	}
}

void buzzerInit(){
	RCC->AHB2ENR |=	RCC_AHB2ENR_GPIOAEN;
	BUZZER_BASE->MODER &= ( ~(0b11 << BUZZER_PIN*2));
	//BUZZER_BASE->MODER |= (MODER_O << BUZZER_PIN*2);
	BUZZER_BASE->MODER |= (MODER_AF << BUZZER_PIN*2);

	//GPIOA->OSPEEDR = (GPIOA->OSPEEDR & (~(0x3 << BUZZER_PIN*2))) | (OSPEEDR_HIGH << BUZZER_PIN*2);
	BUZZER_BASE->AFR[0] &= ~GPIO_AFRL_AFSEL0; // PA0: TIM2_CH1: AF1
	BUZZER_BASE->AFR[0] |= GPIO_AFRL_AFSEL0_0; // PA0: TIM2_CH1: AF1
}

void timerInit(){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1 &= ~TIM_CR1_CMS; // Edge-Aligned

	/*
	 * SoundFrequency = 4MHz / ((PSC+1) * ARR)
	 * Since We fix ARR = 100;
	 * So,
	 * SF = 4e6 / ((PSC+1)*100)
	 * SF = 4e4 / (PSC+1)
	 * PSC = ( 4e4 / SF ) - 1
	 */

	/* Channel 1 */
	//TIM2->PSC = SoundFreq(DO);
	TIM2->PSC = (uint32_t) 152;
	TIM2->ARR = (uint32_t) (100-1);
	TIM2->CCR1 = (uint32_t) (50-1); /* Channel 1 */
	/*
	 * 0110: PWM mode 1 -
	 * In up-counting,
	 * channel 1 is active as long as TIMx_CNT<TIMx_CCR1
	 * else inactive.
	 * In down-counting,
	 * channel 1 is inactive (OC1REF=．0・) as long as TIMx_CNT>TIMx_CCR1
	 * else active (OC1REF=・1・).
	 */
	TIM2->CCMR1 |= ( TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 ); // 0110

	TIM2->CR1 |= TIM_CR1_ARPE;
	TIM2->EGR = TIM_EGR_UG;
	TIM2->CCER = TIM_CCER_CC1E; // TURN ON /* Channel 1 */
}

void timerStart(){
	//GPIOA->BSRR = 1<<BUZZER_PIN; // TEST
	TIM2->CR1 |= TIM_CR1_CEN;
}

void timerStop(){
	//GPIOA->BRR = 1<<BUZZER_PIN; // TEST
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

void GPIO_init(){
	// ===== BUTTON ===== //
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	BUTTON_BASE->MODER &= ( ~(0b11 << BUTTON_PIN*2));
	BUTTON_BASE->MODER |= (MODER_I << BUTTON_PIN*2);
	BUTTON_BASE->PUPDR &= (  ~(0b11 << BUTTON_PIN*2));
	BUTTON_BASE->PUPDR |= (PUPDR_PU << BUTTON_PIN*2);

	// ===== BUZZER ===== //
	buzzerInit();

	// ===== KEYPAD ===== //
	keypad_init();

	// Keypad Full-Trigger to Get Press Action
	keypad_full_trigger();
}

int keypad_scan(){

	for(int i=0; i<4; i++){
		X_ODR = (x_mask) | ((0x01<<x_pin[i])); // set high for z
		X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[i]*2))) | (MODER_I << x_pin[i]*2);
	}

	int press = 0;
	int sum = 0;

	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[0]*2))) | (MODER_O << x_pin[0]*2);
	X_ODR = (x_mask) & (~(0x01 << X0)); // ACTIVE_LOW
	if((Y_IDR & (0x01 << Y0))== 0){ sum+=1; press=1; return sum; } // ACTIVE_LOW
	if((Y_IDR & (0x01 << Y1))== 0){ sum+=4; press=1; return sum; }
	if((Y_IDR & (0x01 << Y2))== 0){ sum+=7; press=1; return sum; }
	if((Y_IDR & (0x01 << Y3))== 0){ sum+=15; press=1; return sum; }
	X_ODR = (x_mask) | ((0x01<<X0)); // set high for z
	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[0]*2))) | (MODER_I << x_pin[0]*2);

	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[1]*2))) | (MODER_O << x_pin[1]*2);
	X_ODR = (x_mask) & (~(0x01 << X1)); // ACTIVE_LOW
	if((Y_IDR & (0x01 << Y0))== 0){ sum+=2; press=1; return sum; }
	if((Y_IDR & (0x01 << Y1))== 0){ sum+=5; press=1; return sum; }
	if((Y_IDR & (0x01 << Y2))== 0){ sum+=8; press=1; return sum; }
	if((Y_IDR & (0x01 << Y3))== 0){ sum+=0; press=1; return sum; }
	X_ODR = (x_mask) | ((0x01<<X1)); // set high for z
	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[1]*2))) | (MODER_I << x_pin[1]*2);

	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[2]*2))) | (MODER_O << x_pin[2]*2);
	X_ODR = (x_mask) & (~(0x01 << X2)); // ACTIVE_LOW
	if((Y_IDR & (0x01 << Y0))== 0){ sum+=3; press=1; return sum; }
	if((Y_IDR & (0x01 << Y1))== 0){ sum+=6; press=1; return sum; }
	if((Y_IDR & (0x01 << Y2))== 0){ sum+=9; press=1; return sum; }
	if((Y_IDR & (0x01 << Y3))== 0){ sum+=14; press=1; return sum; }
	X_ODR = (x_mask) | ((0x01<<X2)); // set high for z
	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[2]*2))) | (MODER_I << x_pin[2]*2);

	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[3]*2))) | (MODER_O << x_pin[3]*2);
	X_ODR = (x_mask) & (~(0x01 << X3)); // ACTIVE_LOW
	if((Y_IDR & (0x01 << Y0))== 0){ sum+=10; press=1; return sum; }
	if((Y_IDR & (0x01 << Y1))== 0){ sum+=11; press=1; return sum; }
	if((Y_IDR & (0x01 << Y2))== 0){ sum+=12; press=1; return sum; }
	if((Y_IDR & (0x01 << Y3))== 0){ sum+=13; press=1; return sum; }
	X_ODR = (x_mask) | ((0x01<<X3)); // set high for z
	X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[3]*2))) | (MODER_I << x_pin[3]*2);

	if(press == 0) return -1;
	else return sum;
}

void alarm_handler(){
	timerStart();
	while((BUTTON_BASE->IDR & (1 << BUTTON_PIN)) != 0){;} // Wait Button Input
	timerStop();
}

void EXTI_config(){
	// INPUT: PB 12 13 14 15
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PB;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PB;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PB;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PB;

	EXTI->IMR1 |= EXTI_IMR1_IM12;
	EXTI->IMR1 |= EXTI_IMR1_IM13;
	EXTI->IMR1 |= EXTI_IMR1_IM14;
	EXTI->IMR1 |= EXTI_IMR1_IM15;

//  EXTI->EMR1 |= EXTI_EMR1_EM12;
//	EXTI->EMR1 |= EXTI_EMR1_EM13;
//	EXTI->EMR1 |= EXTI_EMR1_EM14;
//	EXTI->EMR1 |= EXTI_EMR1_EM15;

	EXTI->FTSR1 |= EXTI_FTSR1_FT12;
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;
	EXTI->FTSR1 |= EXTI_FTSR1_FT14;
	EXTI->FTSR1 |= EXTI_FTSR1_FT15;

}

void NVIC_config(){
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	// NVIC_SetPriority(EXTI15_10_IRQn, 1);
}

void SetSysTickValue(int ticks_second){
	if (ticks_second == 0){
		SysTick->LOAD = (uint32_t)(1); // At Least 1
		SysTick->VAL = 0UL;
	} else if (ticks_second < 0) {
		return;
	} else {
		SysTick->LOAD = (uint32_t)(ticks_second*1000000 - 1UL); // At Least 1
		SysTick->VAL = 0UL;
	}
}


void SysTick_Start(){
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL |= 1 << SysTick_CTRL_ENABLE_Pos;
}

void SysTick_Stop(){
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
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
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL |= 1 << SysTick_CTRL_CLKSOURCE_Pos; // Processor Clock
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= 1 << SysTick_CTRL_TICKINT_Pos; // Assert Exception

	// Set priority
	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
}

# if NEWTYPE == 0
void SysTick_Handler(void){
	SysTick_Stop();
	alarm_handler();

	// Clean Interrupt Request
	EXTI->PR1 |= EXTI_PR1_PIF12;
	EXTI->PR1 |= EXTI_PR1_PIF13;
	EXTI->PR1 |= EXTI_PR1_PIF14;
	EXTI->PR1 |= EXTI_PR1_PIF15;

	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void){
	// Turn Off
	NVIC_DisableIRQ(EXTI15_10_IRQn);

	int value = keypad_scan();
	if(value >= 0){
		SetSysTickValue(value);
		SysTick_Start();
	} else {
		// Clean Interrupt Request
		EXTI->PR1 |= EXTI_PR1_PIF12;
		EXTI->PR1 |= EXTI_PR1_PIF13;
		EXTI->PR1 |= EXTI_PR1_PIF14;
		EXTI->PR1 |= EXTI_PR1_PIF15;

		// Turn On
		NVIC_EnableIRQ(EXTI15_10_IRQn);
	}

	// Keypad Full-Trigger to Get Press Action
	keypad_full_trigger();
}
# endif

# if NEWTYPE == 1
void SysTick_Handler(void){
	SysTick_Stop();
	SysTick->VAL = 0UL;

	alarm_handler();
}

void EXTI15_10_IRQHandler(void){

	int value = keypad_scan();

	if(value >= 0){
		if(SysTick->VAL != 0){
			SysTick_Stop();
			if ( SysTick->VAL > value*1000000 ){
				SysTick->LOAD = (uint32_t)(SysTick->VAL - value*1000000); // At Least 1
				SysTick->VAL = 0UL;
			} else {
				SysTick->LOAD = (uint32_t)(1); // At Least 1
				SysTick->VAL = 0UL;
			}
			SysTick_Start();
		}
		else {
			SetSysTickValue(value);
			SysTick_Start();
		}
	}


	// Clean Interrupt Request
	EXTI->PR1 |= EXTI_PR1_PIF12;
	EXTI->PR1 |= EXTI_PR1_PIF13;
	EXTI->PR1 |= EXTI_PR1_PIF14;
	EXTI->PR1 |= EXTI_PR1_PIF15;

	// Keypad Full-Trigger to Get Press Action
	keypad_full_trigger();

}
# endif

int main(){
	GPIO_init();
	SystemClock_Config();
	NVIC_config();
	EXTI_config();
	timerInit();
}
