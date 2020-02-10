#ifndef LIB_H
#define LIB_H

#define MODER_I 0b00
#define MODER_O 0b01
#define MODER_AF 0b10
#define MODER_A 0b11

#define OTYPER_PP 0b00
#define OTYPER_OD 0b01

#define OSPEEDR_LOW       0b00
#define OSPEEDR_MEDIUM    0b01
#define OSPEEDR_HIGH      0b10
#define OSPEEDR_VERY_HIGH 0b11

#define PUPDR_NO_PP      0b00
#define PUPDR_PU         0b01
#define PUPDR_PD         0b10
#define PUPDR_RESERVED   0b11

void max7219_init();
void max7219_send(unsigned char address, unsigned char data);
int  max7219_display(int data, int num_digs);
void reset_max7219_display();

#endif
