#include <stm32l476xx.h>
#include <lib.h>

extern void Delay1sWith4Mhz();

// BUZZER GPIOA0
#define BUZZER_PIN (0)
#define DO (261.6)
#define RE (293.7)
#define MI (329.6)
#define FA (349.2)
#define SO (392.0)
#define LA (440.0)
#define SI (493.9)
#define HDO (523.3)
#define SoundFreq(TONE) ((uint32_t)(40000 / (uint32_t)(TONE)) -1)

#define X_BASE GPIOA->ODR
#define X0 (5)
#define X1 (6)
#define X2 (7)
#define X3 (8)

#define Y_BASE GPIOB->IDR
#define Y0 (12)
#define Y1 (13)
#define Y2 (14)
#define Y3 (15)

unsigned int x_mask = (0x01<<X0) + (0x01<<X1) + (0x01<<X2) + (0x01<<X3);
unsigned int x_pin[4] = {X0, X1, X2, X3};
unsigned int y_pin[4] = {Y0, Y1, Y2, Y3};

// TODO: initial keypad GPIO pin, X as Output, Y as Input
void keypad_init(){
	// X as OUTPUT
	// OUTPUT GPIOA 5 6 7 8
	RCC->AHB2ENR = RCC->AHB2ENR | (0x1 << 0);

	for(int i=0; i<4; i++){
		GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[i]*2))) | (MODER_O << x_pin[i]*2);
		GPIOA->OTYPER = (GPIOA->OTYPER & (~(0x1 << x_pin[i]))) | (OTYPER_OD << x_pin[i]); // open-drain
		GPIOA->PUPDR = (GPIOA->PUPDR & (~(0x3 << x_pin[i]*2))) | (PUPDR_PU << x_pin[i]*2);  // 1 = Z, 0 = low
		GPIOA->OSPEEDR = (GPIOA->OSPEEDR & (~(0x3 << x_pin[i]*2))) | (OSPEEDR_MEDIUM << x_pin[i]*2);
	}

	// Y as INPUT
	// INPUT GPIOB 12 13 14 15
	RCC->AHB2ENR = RCC->AHB2ENR | (0x1 << 1);
	for(int i=0; i<4; i++){
		GPIOB->MODER = (GPIOB->MODER & (~(0x3 << y_pin[i]*2))) | (MODER_I << y_pin[i]*2);
		GPIOB->OTYPER = (GPIOB->OTYPER & (~(0x1 << y_pin[i]))) | (OTYPER_PP << y_pin[i]); // open-drain
		GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << y_pin[i]*2))) | (PUPDR_PU << y_pin[i]*2); // non-input = Z, 1 = high, 0 = low
		GPIOB->OSPEEDR = (GPIOB->PUPDR & (~(0x3 << y_pin[i]*2))) | (OSPEEDR_MEDIUM << y_pin[i]*2);
	}
}

int keypad_scan(){

	for(int i=0; i<4; i++){
		X_BASE = (x_mask) | ((0x01<<x_pin[i])); // set high for z
		GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[i]*2))) | (MODER_I << x_pin[i]*2);
	}

	int press = 0;
	int sum = 0;

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[0]*2))) | (MODER_O << x_pin[0]*2);
	X_BASE = (x_mask) & (~(0x01 << X0)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=1; press=1; return sum; } // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=4; press=1; return sum; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=7; press=1; return sum; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=15; press=1; return sum; }
	X_BASE = (x_mask) | ((0x01<<X0)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[0]*2))) | (MODER_I << x_pin[0]*2);

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[1]*2))) | (MODER_O << x_pin[1]*2);
	X_BASE = (x_mask) & (~(0x01 << X1)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=2; press=1; return sum; }
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=5; press=1; return sum; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=8; press=1; return sum; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=0; press=1; return sum; }
	X_BASE = (x_mask) | ((0x01<<X1)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[1]*2))) | (MODER_I << x_pin[1]*2);

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[2]*2))) | (MODER_O << x_pin[2]*2);
	X_BASE = (x_mask) & (~(0x01 << X2)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=3; press=1; return sum; }
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=6; press=1; return sum; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=9; press=1; return sum; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=14; press=1; return sum; }
	X_BASE = (x_mask) | ((0x01<<X2)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[2]*2))) | (MODER_I << x_pin[2]*2);

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[3]*2))) | (MODER_O << x_pin[3]*2);
	X_BASE = (x_mask) & (~(0x01 << X3)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=10; press=1; return sum; }
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=11; press=1; return sum; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=12; press=1; return sum; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=13; press=1; return sum; }
	X_BASE = (x_mask) | ((0x01<<X3)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[3]*2))) | (MODER_I << x_pin[3]*2);

	if(press == 0) return -1;
	else return sum;
}

void buzzerInit(){
	RCC->AHB2ENR |=	RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= (  ~(0b11 << BUZZER_PIN*2));
	GPIOA->MODER |= (MODER_AF << BUZZER_PIN*2);

	//GPIOA->OSPEEDR = (GPIOA->OSPEEDR & (~(0x3 << BUZZER_PIN*2))) | (OSPEEDR_HIGH << BUZZER_PIN*2);
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0; // PA0: TIM2_CH1: AF1
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_0; // PA0: TIM2_CH1: AF1
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
	//TIM2->PSC = (uint32_t) (DO-1) ;
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


int main(){
	keypad_init();
	buzzerInit();
	timerInit();
	timerStart();
	int counter = 50-1;
	while(1){
		int input = keypad_scan();
		if(input == -1){
			continue;
		} else if ( input == 1){
			//TIM2->PSC = SoundFreq(DO);
			counter = (counter - 5 < 9 ) ? 9 : counter-5;
		} else if ( input == 2){
			//TIM2->PSC = SoundFreq(RE);
			counter = (counter + 5 > 99 ) ? 99 : counter+5;
		}
		TIM2->CCR1 = (uint32_t)counter;
		Delay1sWith4Mhz(); // Modify, Here is 0.01 Now
	}
}
