#include <stm32l476xx.h>
#include <lib.h>
#define SET_REG(REG,SELECT,VAL) {(REG)=((REG)&(~(SELECT)) | (VAL));}
#define LED_PIN 9

void changePLLSetting(int counter);
void GPIO_init(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << LED_PIN*2))) | (MODER_O << LED_PIN*2);
	GPIOA->PUPDR = (GPIOA->PUPDR & (~(0x3 << LED_PIN*2))) | (PUPDR_PU << LED_PIN*2);
	GPIOA->OSPEEDR = (GPIOA->OSPEEDR & (~(0x3 << LED_PIN*2))) | (OSPEEDR_HIGH << LED_PIN*2);

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER = (GPIOC->MODER & (~(0x3 << 13*2))) | (MODER_I << 13*2);
	GPIOC->PUPDR = (GPIOC->PUPDR & (~(0x3 << 13*2))) | (PUPDR_PU << 13*2);
}

extern void Delay1sWith4Mhz();
extern int  Delay1sWith4MhzWithDebounce();

// HSI 16Mhz
// PLLN : 8 ~ 86
// PLLM : 000 = 1, 001 = 2, ... 111 = 8
// PLLR : 00=2, 01=4, 10=6, 11=8
								// PLLN, PLLM, PLLR, PRESCALER
unsigned int PLL_SETTING[5*4] = {  8,    8-1,    0b11, 2,// 1 MHz = 16 * 8 / 8 / 8 /  2
								  12,    4-1,    0b11, 1,// 6 MHz = 16 * 12 / 4 / 8 / 1
								  10,    4-1,    0b01, 1,// 10 MHz = 16 * 10 / 4 / 4 / 1
								  16,    4-1,    0b01, 1,// 16 MHz = 16 * 16 / 4 / 4 / 1
								  40,    4-1,    0b01, 1 }; // 40 MHz

void SystemClock_Config(){
	/*
	// Set PLL SRC = HSI
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
	*/

	//TODO: Change the SYSCLK source and set the corresponding Prescaler value.
	changePLLSetting(0);
}

void changePLLSetting(int counter){

	// Set SYSCLK SRC = MSI
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_MSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI);

	// TURN OFF PLL
	RCC->CR &= (~RCC_CR_PLLON);
	while((RCC->CR & RCC_CR_PLLRDY));

	// Set PLL SRC = HSI
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;

	// Set PLLR ON
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

	// FREQ
	RCC->PLLCFGR = (RCC->PLLCFGR & (~(0b1111111<<8)));
	RCC->PLLCFGR |= PLL_SETTING[counter*4+0] << 8;

	RCC->PLLCFGR = (RCC->PLLCFGR & (~(0b111<<4)));
	RCC->PLLCFGR |= PLL_SETTING[counter*4+1] << 4;

	RCC->PLLCFGR = (RCC->PLLCFGR & (~(0b11<<25)));
	RCC->PLLCFGR |= PLL_SETTING[counter*4+2] << 25;

	if(PLL_SETTING[counter*4+3] == 1){
		RCC->CFGR &= ~RCC_CFGR_HPRE;
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	}
	else if((PLL_SETTING[counter*4+3] == 2)){
		RCC->CFGR &= ~RCC_CFGR_HPRE;
		RCC->CFGR |= RCC_CFGR_HPRE_DIV2;
	}
	else if((PLL_SETTING[counter*4+3] == 4)){
		RCC->CFGR &= ~RCC_CFGR_HPRE;
		RCC->CFGR |= RCC_CFGR_HPRE_DIV4;
	}
	// FREQ END

	// TURN ON HSI
	RCC->CR &= ~RCC_CR_HSION;
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);

	// TURN ON PLL
	RCC->CR &= ~RCC_CR_PLLON;
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == 0);

	// Set SYSCLK SRC = PLL
	RCC->CFGR &= ~RCC_CFGR_SW; // Set SYSCLK to PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;

}

int main(){
	int counter = 0 ;
	SystemClock_Config();
	GPIO_init();
	while(1){
		// make LED light ON
		GPIOA->BSRR = (1<<LED_PIN);

		int trigger = Delay1sWith4MhzWithDebounce();
		if(trigger != 0){
			counter = (counter+1)%5;
			changePLLSetting(counter);
		}

		// make LED light OFF
		GPIOA->BRR = (1<<LED_PIN);

		Delay1sWith4Mhz();
	}
}

