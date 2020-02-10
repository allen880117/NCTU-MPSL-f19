#ifndef WANGXR_MPSL_LIB_H
#define WANGXR_MPSL_LIB_H

#define MODER_I 0x00
#define MODER_O 0x01

#define OTYPER_PP 0x00
#define OTYPER_OD 0x01

#define OSPEEDR_LOW       0x00
#define OSPEEDR_MEDIUM    0x01
#define OSPEEDR_HIGH      0x02
#define OSPEEDR_VERY_HIGH 0x03

#define PUPDR_NO_PP      0x00
#define PUPDR_PU         0x01
#define PUPDR_PD         0x02
#define PUPDR_RESERVED   0x03

enum PINS {
    PA0 = 100, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15, 
    PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15, 
    PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15, 
    PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15, 
    PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15, 
    PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15, 
    PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15, 
    PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15, 
    NONE,
};

typedef struct _SEV_SEG_PINS_t{
    PINS SEV_SEG_CLK;
    PINS SEV_SEG_DIN;
    PINS SEV_SEG_CS;
} SEV_SEG_PINS ;

SEV_SEG_PINS* new_sev_seg(PINS clk, PINS din, PINS cs);
void init_sev_seg();

typedef struct _KEY_PAD_PINS_t{
    PINS KEY_PAD_INPUT_O;
    PINS KEY_PAD_INPUT_1;
    PINS KEY_PAD_INPUT_2;
    PINS KEY_PAD_INPUT_3;
    PINS KEY_PAD_OUTPUT_0;
    PINS KEY_PAD_OUTPUT_1;
    PINS KEY_PAD_OUTPUT_2;
    PINS KEY_PAD_OUTPUT_3;
} KEY_PAD_PINS ;

void GPIO_init();
void max7219_init();
void max7219_send(unsigned char address, unsigned char data);
int  max7219_display(int data, int num_digs);
void reset_max7219_display();

#endif
