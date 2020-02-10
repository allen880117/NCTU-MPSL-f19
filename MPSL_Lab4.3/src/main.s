	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	// TODO: put 0 to F 7-Seg LED pattern here
	num: .byte 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x72, 0x7F, 0x73, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47
	minus_one: .byte 0x01, 0x30
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

	.equ GPIOC_MODER,   0x48000800
	.equ GPIOC_PUPDR,   0x4800080C
	.equ GPIOC_IDR,     0x48000810

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
	.equ DIGIT_7,       0x08
	// ADDRESS in MAX7219 END
	// Delay Parameter
	.equ X,             16000 // for 500ms
	.equ Y,             120   // for 500ms
	// Delay Parameter END
	// Bound
	.equ Debounce_Bound,  40192 // (4e6 / 100 = 40000) and for rotate
	.equ OneSecond_Bound, 500000 // (4e6 / <alpha>)
	.equ Display_Bound,   100000000
	// Bound  END

main:
	/*
		R2 = Return Value
		R3 = f(n-2)
		R4 = f(n-1)
		R5 = f(n)
	*/
	BL   GPIO_init
	BL   max7219_init

	BL   Reset_Display
	BL   Reset_Fibonacci
	BL   Display_Fibonacci // Input R5

loop:
	/*
		R2 = Return Value
		R3 = f(n-2)
		R4 = f(n-1)
		R5 = f(n)
	*/
	// === Get Button Input
	ButtonInput:
		// R2 will be return value of Debounce_500ms
		BL   Debounce_counter
		cmp  r2, #0 // R2 Return Value
		beq  ButtonInput // No Push

		cmp  r2, #1 // R2 Return Value
		beq  DoDisplay_Normal // Just one touch

		cmp  r2, #2 // R2 Return Value
		beq  DoDisplay_OneSecondLong // Long Press for one second
	// === Get Button Input END

	DoDisplay_Normal:
		BL   Reset_Display
		BL	 Fibonacci_Next

		cmp  R5, #-1
		beq  __Minus_one

		BL   Display_Fibonacci // Input R5
		B    loop

		__Minus_one:
		BL   Display_Minus_One
		B    loop

	DoDisplay_OneSecondLong:
		BL   Reset_Display
		BL   Reset_Fibonacci
		BL   Display_Fibonacci // Input R5
		B    loop

Display_Minus_One:
	PUSH {r0, r1, r9, LR}
	ldr  r9, =minus_one

	ldr  r0, =#DIGIT_1 // Display Minus Sign
	ldrb r1, [r9, #0]
	BL	 max7219_send

	ldr  r0, =#DIGIT_0 // Display One
	ldrb r1, [r9, #1]
	BL	 max7219_send

	POP  {r0, r1, r9, LR}
	BX   LR

Display_Fibonacci:
	// Input Parameter: R5
	/*
		R9 = Array(Numbers)
		R0 = Addr for send
		R1 = Data for send
		R2 = TEMP
		R3 = Q
		R4 = R
		R5 = <INPUT>
		R6 = Display Digit
	*/
	PUSH {r0-r6, r9, LR}
	ldr  r9, =num
	movs r6, #0x01 // Digit_0, Go UP

	Display_Fibonacci_Loop:
		movs r2, #10
		udiv r3, r5, r2 // R3 = R5 / 10 // Calculate Q
		mul  r2, r3, r2 // R2 = R3 * 10
		subs r4, r5, r2 // R4 = R5 - R2 // Calculate R

		movs r0, r6 // ADDR
		ldrb r1, [r9, r4] // DATA
		BL   max7219_send

		add  r6, #1     // Digit++
		movs r5, r3     // R5 = R3
		cmp  r5, #0
		beq  Display_Fibonacci_Return

		B    Display_Fibonacci_Loop

	Display_Fibonacci_Return:
	POP  {r0-r6, r9, LR}
	BX   LR

Fibonacci_Next:
	// Calculate Next Fibonacci
	/*
		R0 = TEMP
		R1 = Display_Bound
		R3 = f(n-2) = f(0) = 0
		R4 = f(n-1) = f(1) = 1
		R5 = f(n)   = f(0) = 0
	*/
	PUSH {R0, R1, LR}
	cmp  R5, #-1
	beq  Fibonacci_Next_Return

	adds R5, R3, R4 // f(n) = f(n-2) + f(n-1)

	ldr  R1, =#Display_Bound
	cmp  R5, R1 // f(n) >= 1 0000 0000
	bgt  Out_Bound

	In_Bound:
		movs R3, R4 // f(n-2) = f(n-1)
		movs R4, R5 // f(n-1) = Temp
		b    Fibonacci_Next_Return

	Out_Bound:
		movs R5, #-1 // f(n) = -1
		b    Fibonacci_Next_Return

	Fibonacci_Next_Return:
	POP  {R0, R1, LR}
	BX   LR


Reset_Fibonacci:
	// Reset Fibonacci
	/*
		R2 = Return Value
		R3 = f(n-2) = f(0) = 0
		R4 = f(n-1) = f(1) = 1
		R5 = f(n)   = f(0) = 0
	*/
	movs R3, #0
	movs R4, #1
	movs R5, #0
	BX   LR

Reset_Display:
	PUSH {r0, r1, r2, r9, LR}

	ldr  r9, =minus_one
	movs r2, #0x01

	Reset_Display_Loop:
		movs r0, r2
		movs r1, #0 // Clear
		BL	 max7219_send

		adds r2, r2, #0x01
		cmp  r2, #0x09
		bne  Reset_Display_Loop

	POP  {r0, r1, r2, r9, LR}
	BX   LR

GPIO_init:
	//TODO: Initialize three GPIO pins as output for max7219 DIN, CS and CLK
	movs r0, #0x6 // 110
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

	// Set PC13 as Input Mode
	ldr  r1, =GPIOC_MODER
	ldr  r2, [r1]
	and  r2, #0xF3FFFFFF
	movs r0, #0x00000000
	orrs r2, r2, r0
	str  r2, [r1]

	// Set PC13 as Pull-up input mode // REF: LEC05 P20
	ldr  r1, =GPIOC_PUPDR
	ldr  r2, [r1]
	and  r2, #0xF3FFFFFF
	movs r0, #0x04000000
	orrs r2, r2, r0
	str  r2, [r1]

	BX   LR

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

max7219_init:
	//TODO: Initialize max7219 registers
	PUSH {r0, r1, LR}

	ldr  r0, =#DECODE_MODE
	ldr  r1, =#0x0 // No Decode for digit 7~"0"
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

	POP  {r0, r1, LR}
	BX   LR

Debounce_counter:
	/*
		Parameter List :
		Return Value:   R2

		Inner Variables:
			R0, ADDR(GPIOC_IDR)
			R1, Debounce Counter
			R3, TEMP
			R4, Bound_Const
	*/
	//TODO: Debounce in 0.5s
	PUSH {R0, R1, R3, R4, LR}

	ldr  R0, =GPIOC_IDR
 	movs R1, #0 // Debounce Counter
	ldr  R4, =#Debounce_Bound // Debounce_Bound

	Debounce_counter_loop:
		ldrh R3, [R0] // Load GPIOC_IDR value
		and  R3, #(1<<13) // Check PC13 input

		cmp  R3, #0 // Button Trigger
		bne  Button_Not_Trigger // Not Stable Continous Input of Button, Return 0

		add  R1, R1, #1 // Counter++
		cmp  R1, R4 // If Counter == 40192 // About 40000 * 0.25us = 0.01s
		beq  Button_Trigger // Stable Input Continous of Button

		b    Debounce_counter_loop

	Button_Trigger:
		ldr R4, =#OneSecond_Bound

		Button_Trigger_loop:
			ldrh R3, [R0] // Load GPIOC_IDR value
			and  R3, #(1<<13) // Check PC13 input

			cmp  R3, #0 // Button Trigger
			bne  Button_Not_Long_Press // Not Long Press, Return 1

			add  R1, R1, #1 // Counter++
			cmp  R1, R4
			beq  Button_Long_Press // Long Press, Return 2

			b    Button_Trigger_loop

	Button_Not_Trigger:
		movs R2, #0
		B    Debounce_counter_Return

	Button_Not_Long_Press:
		movs R2, #1
		B    Debounce_counter_Return

	Button_Long_Press:
		movs R2, #2
		B    Debounce_counter_Return

	Debounce_counter_Return:
	POP  {R0, R1, R3, R4, LR}
    BX   LR
