#include <lib.h>
#include <stm32l476xx.h>

#define DECODE_MODE  0x09
#define DISPLAY_TEST 0x0F
#define	SCAN_LIMIT   0x0B
#define	INTENSITY    0x0A
#define	SHUTDOWN     0x0C

#define DIN_OFFSET   0x08 //PB3 // 0000 1000
#define CS_OFFSET    0x10 //PB4 // 0001 0000
#define CLK_OFFSET	 0x20 //PB5 // 0010 0000

void GPIO_init(){
	//TODO: Initialize GPIO
	// OUTPUT GPIOB 3 4 5 FOR 7-SEG
	RCC->AHB2ENR = RCC->AHB2ENR | (0x1 << 1);

	GPIOB->MODER = (GPIOB->MODER & (~(0x3 << 3*2))) | (MODER_O << 3*2);
	GPIOB->MODER = (GPIOB->MODER & (~(0x3 << 4*2))) | (MODER_O << 4*2);
	GPIOB->MODER = (GPIOB->MODER & (~(0x3 << 5*2))) | (MODER_O << 5*2);

	GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << 3*2))) | (PUPDR_PU << 3*2);
	GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << 4*2))) | (PUPDR_PU << 4*2);
	GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << 5*2))) | (PUPDR_PU << 5*2);

	GPIOB->OSPEEDR = (GPIOB->OSPEEDR & (~(0x3 << 3*2))) | (OSPEEDR_MEDIUM << 3*2);
	GPIOB->OSPEEDR = (GPIOB->OSPEEDR & (~(0x3 << 4*2))) | (OSPEEDR_MEDIUM << 4*2);
	GPIOB->OSPEEDR = (GPIOB->OSPEEDR & (~(0x3 << 5*2))) | (OSPEEDR_MEDIUM << 5*2);

	/*
	// INPUT GPIOB 12 13 14 15
	RCC->AHB2ENR = RCC->AHB2ENR | (0x1 << 1);

	GPIOB->MODER = (GPIOB->MODER & (~(0x3 << 12*2))) | (MODER_I << 12*2);
	GPIOB->MODER = (GPIOB->MODER & (~(0x3 << 13*2))) | (MODER_I << 13*2);
	GPIOB->MODER = (GPIOB->MODER & (~(0x3 << 14*2))) | (MODER_I << 14*2);
	GPIOB->MODER = (GPIOB->MODER & (~(0x3 << 15*2))) | (MODER_I << 15*2);

	GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << 12*2))) | (PUPDR_PU << 12*2);
	GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << 13*2))) | (PUPDR_PU << 13*2);
	GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << 14*2))) | (PUPDR_PU << 14*2);
	GPIOB->PUPDR = (GPIOB->PUPDR & (~(0x3 << 15*2))) | (PUPDR_PU << 15*2);

	// INPUT GPIOC 12 13 14 15
	RCC->AHB2ENR = RCC->AHB2ENR | (0x1 << 2);

	GPIOC->MODER = (GPIOC->MODER & (~(0x3 << 12*2))) | (MODER_I << 12*2);
	GPIOC->MODER = (GPIOC->MODER & (~(0x3 << 13*2))) | (MODER_I << 13*2);
	GPIOC->MODER = (GPIOC->MODER & (~(0x3 << 14*2))) | (MODER_I << 14*2);
	GPIOC->MODER = (GPIOC->MODER & (~(0x3 << 15*2))) | (MODER_I << 15*2);

	GPIOC->PUPDR = (GPIOC->PUPDR & (~(0x3 << 12*2))) | (PUPDR_PU << 12*2);
	GPIOC->PUPDR = (GPIOC->PUPDR & (~(0x3 << 13*2))) | (PUPDR_PU << 13*2);
	GPIOC->PUPDR = (GPIOC->PUPDR & (~(0x3 << 14*2))) | (PUPDR_PU << 14*2);
	GPIOC->PUPDR = (GPIOC->PUPDR & (~(0x3 << 15*2))) | (PUPDR_PU << 15*2);
	*/
}

void max7219_init(){
	//TODO: Initialize max7219 registers
	max7219_send((unsigned char)DECODE_MODE, (unsigned char)0x7F);
	max7219_send((unsigned char)DISPLAY_TEST, (unsigned char)0x0);
	max7219_send((unsigned char)SCAN_LIMIT, (unsigned char)0x6);
	max7219_send((unsigned char)INTENSITY, (unsigned char)0xA);
	max7219_send((unsigned char)SHUTDOWN, (unsigned char)0x1);
}

void max7219_send(unsigned char address, unsigned char data){
	unsigned short transfer_data;
	transfer_data = ( address << 8 ) + data;

	int i = 16;
	while(i>0){
		unsigned short bit_mask = ( 1 << (i-1) );

		GPIOB->BRR = CLK_OFFSET; // CLK = 0

		if( (transfer_data & bit_mask) != 0 )
			GPIOB->BSRR = DIN_OFFSET;
		else
			GPIOB->BRR  = DIN_OFFSET;

		GPIOB->BSRR = CLK_OFFSET; // CLK = 1

		i--;
	}

	GPIOB->BRR = CS_OFFSET; // CS = 0
	GPIOB->BSRR = CS_OFFSET; // CS = 1
}

int max7219_display(int data, int num_digs){

	// === illegal data range
	if(num_digs > 8) return -1;

	// === Display data
	int digit_addr = 0x01;
	int remainder;
	while(digit_addr<=num_digs){
		remainder = data%10;
		max7219_send((unsigned char)digit_addr, (unsigned char)remainder);

		data = data/10;
		digit_addr++;
	}

	// === Success
	return 0;
}

void reset_max7219_display(){
	int digit_addr = 0x01;
	while(digit_addr<=8){
		max7219_send((unsigned char)digit_addr, (unsigned char)0xF);
		digit_addr++;
	}
}
