#include <lib.h>
#include <stm32l476xx.h>

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

extern void GPIO_init();
extern void max7219_init();
extern int max7219_display(int data, int num_digs);
extern void reset_max7219_display();

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

/*  TODO: scan keypad value
	return:
		>=0: key pressed value
		-1: no key press
*/
int keypad_scan(){

	int press = 0;
	int sum = 0;

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[0]*2))) | (MODER_O << x_pin[0]*2);
	X_BASE = (x_mask) & (~(0x01 << X0)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=1; press=1; } // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=4; press=1; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=7; press=1; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=15; press=1; }
	X_BASE = (x_mask) | ((0x01<<X0)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[0]*2))) | (MODER_I << x_pin[0]*2);

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[1]*2))) | (MODER_O << x_pin[1]*2);
	X_BASE = (x_mask) & (~(0x01 << X1)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=2; press=1; }
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=5; press=1; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=8; press=1; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=0; press=1; }
	X_BASE = (x_mask) | ((0x01<<X1)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[1]*2))) | (MODER_I << x_pin[1]*2);

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[2]*2))) | (MODER_O << x_pin[2]*2);
	X_BASE = (x_mask) & (~(0x01 << X2)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=3; press=1; }
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=6; press=1; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=9; press=1; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=14; press=1; }
	X_BASE = (x_mask) | ((0x01<<X2)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[2]*2))) | (MODER_I << x_pin[2]*2);

	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[3]*2))) | (MODER_O << x_pin[3]*2);
	X_BASE = (x_mask) & (~(0x01 << X3)); // ACTIVE_LOW
	if((Y_BASE & (0x01 << Y0))== 0){ sum+=10; press=1; }
	if((Y_BASE & (0x01 << Y1))== 0){ sum+=11; press=1; }
	if((Y_BASE & (0x01 << Y2))== 0){ sum+=12; press=1; }
	if((Y_BASE & (0x01 << Y3))== 0){ sum+=13; press=1; }
	X_BASE = (x_mask) | ((0x01<<X3)); // set high for z
	GPIOA->MODER = (GPIOA->MODER & (~(0x3 << x_pin[3]*2))) | (MODER_I << x_pin[3]*2);

	if(press == 0) return -1;
	else return sum;
}

int main(){

	// === Initialize GPIO and 7-Segment
	GPIO_init();
	keypad_init();
	max7219_init();
	reset_max7219_display();

	int past_press = 0;
	int curr_press = 0;
	int past_sum = -1;
	int curr_sum = -1;

	while(1==1){
		int input = keypad_scan();
		curr_sum = input;

		if(input != -1) curr_press = 1;
		else curr_press = 0;

		if( ((curr_press == 1) && (past_press == 0)) ){
			reset_max7219_display();
			if(input >= 100) max7219_display(input, 3);
			else if(input >= 10) max7219_display(input, 2);
			else max7219_display(input, 1);
		} else if( ((curr_press == 1) && (past_press == 1)) && (curr_sum != past_sum) ){
			reset_max7219_display();
			if(input >= 100) max7219_display(input, 3);
			else if(input >= 10) max7219_display(input, 2);
			else max7219_display(input, 1);
		} else if(curr_press == 0 && past_press == 1) {
			reset_max7219_display();
		} else if(curr_press == 0 && past_press == 0) {
			reset_max7219_display();
		}

		past_press = curr_press;
		past_sum = curr_sum;
	}
}
