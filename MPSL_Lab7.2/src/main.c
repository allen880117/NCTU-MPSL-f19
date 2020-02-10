#include <stm32l476xx.h>
#include <core_cm4.h>
#include <lib.h>

#define LED_PIN (9)
#define LED_BASE (GPIOA)

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

extern void Delay05sWith4Mhz();

void keypad_full_trigger(){
	// Keypad Full-Trigger to Get Press Action
	for(int i=0; i<4; i++){
		X_BASE->MODER = (X_BASE->MODER & (~(0x3 << x_pin[i]*2))) | (MODER_O << x_pin[i]*2);
		X_ODR &= ~x_mask;
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

	// Keypad Full-Trigger to Get Press Action
	keypad_full_trigger();
}

void GPIO_init(){
	// ===== LED ===== //
	RCC->AHB2ENR |=	RCC_AHB2ENR_GPIOAEN;
	LED_BASE->MODER &= (~(0b11 << LED_PIN*2));
	LED_BASE->MODER |= (MODER_O << LED_PIN*2);
	LED_BASE->OSPEEDR &= (~(0b11 << LED_PIN*2));
	LED_BASE->OSPEEDR |= (OSPEEDR_HIGH << LED_PIN*2);

	// ===== KEYPAD ===== //
	keypad_init();
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

void blink_handler(int value){
	for(int i=0; i<value; i++){
		LED_BASE->BSRR = 1 << LED_PIN;
		Delay05sWith4Mhz();
		LED_BASE->BRR = 1 << LED_PIN;
		Delay05sWith4Mhz();
	}
	LED_BASE->BSRR = 1 << LED_PIN;
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
	NVIC_SetPriority(EXTI15_10_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
}

void EXTI15_10_IRQHandler(void){
	blink_handler(keypad_scan());

	EXTI->PR1 |= EXTI_PR1_PIF12;
	EXTI->PR1 |= EXTI_PR1_PIF13;
	EXTI->PR1 |= EXTI_PR1_PIF14;
	EXTI->PR1 |= EXTI_PR1_PIF15;

	// Keypad Full-Trigger to Get Press Action
	keypad_full_trigger();
}

int main(){
	NVIC_config();
	EXTI_config();
	GPIO_init();
	LED_BASE->BSRR = 1 << LED_PIN;
	while(1){;}
}
