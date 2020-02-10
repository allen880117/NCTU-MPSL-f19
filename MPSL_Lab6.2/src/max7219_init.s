	.syntax unified
	.cpu cortex-m4
	.thumb
.text
	.global max7219_init
	.equ RCC_AHB2ENR,   0x4002104C

	.equ GPIOB_MODER,   0x48000400
	.equ GPIOB_OTYPER,  0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR,   0x4800040C
	.equ GPIOB_IDR,     0x48000410
	.equ GPIOB_ODR,     0x48000414
	.equ GPIOB_BSRR,    0x48000418
	.equ GPIOB_BRR,     0x48000428

	.equ GPIOB_BASE,    0x48000400
	.equ GPIO_BSRR_OFFSET, 0x18
	.equ GPIO_BRR_OFFSET,  0x28

	// PIN ADDRESS OFFSET
	.equ DIN,           0x08 //PB3 // 0000 1000
	.equ CS,			0x10 //PB4 // 0001 0000
	.equ CLK,			0x20 //PB5 // 0010 0000
	// PIN ADDRESS OFFSET END

	// ADDRESS in MAX7219
	.equ DECODE_MODE,   0x09
	.equ DISPLAY_TEST,  0x0F
	.equ SCAN_LIMIT,    0x0B
	.equ INTENSITY,     0x0A
	.equ SHUTDOWN,      0x0C

	.equ DIGIT_0,       0x01
	.equ DIGIT_1,       0x02
	.equ DIGIT_2,       0x03
	.equ DIGIT_3,       0x04
	.equ DIGIT_4,       0x05
	.equ DIGIT_5,       0x06
	.equ DIGIT_6,       0x07
	// ADDRESS in MAX7219 END

max7219_init:
	//TODO: Initialize max7219 registers
	PUSH {r0, r1, r2, LR}

	ldr  r0, =#DECODE_MODE
	ldr  r1, =#0xFF
	BL   max7219_send

	ldr  r0, =#DISPLAY_TEST
	ldr  r1, =#0x0 // Turn Off "Display Test Mode"
	BL   max7219_send

	ldr  r0, =#SCAN_LIMIT
	ldr  r1, =#0x7 // The Digits allowed to display, 0 means 1 digit, 1 means 2 digits
	BL   max7219_send

	ldr  r0, =#INTENSITY
	ldr  r1, =#0xA // 0~F
	BL   max7219_send

	ldr  r0, =#SHUTDOWN
	ldr  r1, =#0x1 // Turn on "Normal Operation"
	BL   max7219_send

	POP  {r0, r1, r2, LR}
	BX   LR
