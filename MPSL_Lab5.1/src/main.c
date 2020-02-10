#include<stm32l476xx.h>

extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

int display(int data, int num_digs){

	// === illegal data range
	if(num_digs > 8) return -1;
	if(data >= 100000000) return -1;

	// === Reset Displayer
	int digit_addr = 0x01;
	while(digit_addr<=0x08){
		max7219_send((unsigned char)digit_addr, (unsigned char)(0xF));
		digit_addr++;
	}
	// === Display data
	digit_addr = 0x01;
	while(digit_addr<=num_digs){
		int remainder = data%10;
		max7219_send((unsigned char)digit_addr, (unsigned char)(remainder));

		data = data/10;
		digit_addr++;
	}

	// === Success
	return 0;
}

int main(){

	// === Initialize GPIO and 7-Segment
	int student_id = 616309;
	GPIO_init();
	max7219_init();

	// === Do
	display(student_id, 7);
}
