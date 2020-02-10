	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	// TODO: put 0 to F 7-Seg LED pattern here
	arr: .byte 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x72, 0x7F, 0x73, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47

.text
	.global main
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
	// ADDRESS in MAX7219 END
	// Delay Parameter
	.equ X,             32000
	.equ Y,             120
	// Delay Parameter END

main:
	BL   GPIO_init
	BL   max7219_init

loop:
	BL   Display0toF
	B    loop

GPIO_init:
	//TODO: Initialize three GPIO pins as output for max7219 DIN, CS and CLK
	movs r0, #0x2
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]

	ldr  r1, =GPIOB_MODER
	ldr  r2, [r1]
	and  r2, #0xFFFFF03F // 1111 1111 1111 1111 1111 0000 0011 1111
	movs r0, #0x00000540 // 0000 0000 0000 0000 0000 0101 0100 0000
	orrs r2, r2, r0
	str  r2, [r1]

	ldr  r1, =GPIOB_PUPDR
	ldr  r2, [r1]
	and  r2, #0xFFFFF03F // 1111 1111 1111 1111 1111 0000 0011 1111
	movs r0, #0x00000540 // 0000 0000 0000 0000 0000 0101 0100 0000
	orrs r2, r2, r0
	str  r2, [r1]

	ldr  r1, =GPIOB_OSPEEDR
	ldr  r2, [r1]
	and  r2, #0xFFFFF03F // 1111 1111 1111 1111 1111 0000 0011 1111
	movs r0, #0x00000540 // 0000 0000 0000 0000 0000 0101 0100 0000
	orrs r2, r2, r0
	str  r2, [r1]

	BX   LR

Display0toF:
	//TODO: Display 0 to F at first digit on 7-SEG LED. Display one per second.
	PUSH {LR}
	ldr  r9, =arr
	ldr  r2, =#0

	Display0toF_Loop:
		ldr  r0, =#DIGIT_0 // r0(ADDR) = 0x01(DIGIT_0)
		ldrb r1, [r9, r2]  // r1(DATA) = arr[r2]
		BL   max7219_send

		BL   Delay // Delay for 1 second

		add  r2, r2, #1
		cmp  r2, #16
		bne  Display0toF_Loop

		mov  r2, #0
		b    Display0toF_Loop

	POP  {LR}
	BX   LR

max7219_send:
	// Input parameter: r0 is ADDRESS, r1 is DATA
	//TODO: Use this function to send a message to max7219
	PUSH {R0, R1, R2, R9, LR}

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

	POP  {R0, R1, R2, R9, LR}
	BX   LR

max7219_init:
	//TODO: Initialize max7219 registers
	PUSH {r0, r1, r2, LR}

	ldr  r0, =#DECODE_MODE
	ldr  r1, =#0x0 // No Decode for digit 7~"0"
	BL   max7219_send

	ldr  r0, =#DISPLAY_TEST
	ldr  r1, =#0x0 // Turn Off "Display Test Mode"
	BL   max7219_send

	ldr  r0, =#SCAN_LIMIT
	ldr  r1, =#0x0 // The Digits allowed to display, 0 means 1 digit, 1 means 2 digits
	BL   max7219_send

	ldr  r0, =#INTENSITY
	ldr  r1, =#0xA // 0~F
	BL   max7219_send

	ldr  r0, =#SHUTDOWN
	ldr  r1, =#0x1 // Turn on "Normal Operation"
	BL   max7219_send

	POP  {r0, r1, r2, LR}
	BX   LR

Delay:
	//TODO: Write a delay 1 second function
	PUSH {R3, R4, LR}
    ldr  R3, =X
    L1:
        ldr  R4, =Y
    L2:
        subs R4, #1
        bne  L2
        subs R3, #1
        bne  L1

	POP  {R3, R4, LR}
    BX   LR
