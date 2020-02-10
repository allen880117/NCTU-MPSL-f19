	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global max7219_send
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

max7219_send:
	// Input parameter: r0 is ADDRESS, r1 is DATA
	//TODO: Use this function to send a message to max7219
	PUSH {R0-R9, LR}

	lsl  r0, r0, #8 // R0 = {ADDRESS, 8'b0}
	add  r0, r0, r1 // R0 = {ADDRESS, DATA}

	ldr  r1, =#GPIOB_BASE
	ldr  r2, =#CS  // PIN OFFSET
	ldr  r3, =#DIN // PIN OFFSET
	ldr  r4, =#CLK // PIN OFFSET
	ldr  r5, =#GPIO_BSRR_OFFSET
	ldr  r6, =#GPIO_BRR_OFFSET

	mov  r7, #16 // r7 = i (as Counter of # of Input Bits)

	max7219_send_loop:
		mov  r8, #1
		sub  r9, r7, #1
		lsl  r8, r8, r9 // r8 = mask

		str  r4, [r1, r6] // BRR[CLK_PIN_OFFSET] = 1 means CLK=0

		tst  r0, r8 // {ADDRESS, DATA} & MASK == 0 ? means get {ADDRESS, DATA}[r7];
		beq  din_bit_set_0 // True, goto din_bit_set_0 // bit_not_set

		din_bit_set_1:
		str  r3, [r1, r5] // BSRR[DIN_PIN_OFFSET] = 1 means DIN=1
		b    if_done

		din_bit_set_0:
		str  r3, [r1, r6] // BRR[DIN_PIN_OFFSET] = 1 means DIN=0
		b    if_done

		if_done:
		str  r4, [r1, r5] // BSRR[CLK_PIN_OFFSET] = 1 means CLK=1

		subs r7, r7, #1
		bgt  max7219_send_loop // if r7 > 0, goto max7219_send_loop

		// Send Rising Edge of CS to latch Data
		str  r2, [r1, r6] // BRR[CS_PIN_OFFSET] = 1 means CS=0
		str  r2, [r1, r5] // BSRR[CS_PIN_OFFSET] = 1 means CS=1
		// Send Rising Edge of CS to latch Data END

	POP  {R0-R9, LR}
	BX   LR
