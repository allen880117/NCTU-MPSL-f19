.syntax unified
.cpu cortex-m4
.thumb

.data
	leds: .byte 0
	counter: .byte 0

.text
.global main
	.equ RCC_AHB2ENR,   0x4002104C

	.equ GPIOA_MODER,   0x48000000 // port Mode register
	.equ GPIOA_OTYPER,  0x48000004 // port Output Type register
	.equ GPIOA_OSPEEDR, 0x48000008 // port Output Speed register
	.equ GPIOA_PUPDR,   0x4800000C // port pull-up/pull-down register
	.equ GPIOA_ODR,     0x48000014 // port Output Data register

	.equ GPIOB_MODER,   0x48000400 // REF: LEC05 P9, P23
	.equ GPIOB_OTYPER,  0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR,   0x4800040C
	.equ GPIOB_ODR,     0x48000414

	.equ X, 0x7D00 // 32000
	.equ Y, 0x001E // 30

main:
	bl   GPIO_init

	movs R1, #1
	ldr  R0, =leds
	strb R1, [R0]

Loop:
	// TODO: Write the display pattern into led variable
	bl   DisplayLED
	bl   Delay
	b    Loop

GPIO_init:
	// TODO: Initial LED GPIO pins as output

	// Enable AHB2 Clock For GPIO_B
	// REF: LEC05 P18
	movs r0, #0x2
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]

	// Set PB3, PB4, PB5, PB6 as Output Mode
	// REF: LEC05 P19
	ldr  r1, =GPIOB_MODER
	ldr  r2, [r1]

	and  r2, #0xFFFFC03F // 1111 1111 1111 1111 1100 0000 0011 1111

	movs r0, #0x00001540 // 0000 0000 0000 0000 0001 0101 0100 0000
	orrs r2, r2, r0

	str  r2, [r1]

	// Set PB3, PB4, PB5, PB6 as Pull-up
	// REF: LEC05 P20
	ldr  r1, =GPIOB_PUPDR
	ldr  r2, [r1]
	and  r2, #0xFFFFC03F
	movs r0, #0x00001540
	orrs r2, r2, r0
	str  r2, [r1]

	// Set PB3, PB4, PB5, PB6 as High-Speed Mode
	// REF: LEC05 P21
	movs r0, #0x00002A80 // 0000 0000 0000 0000 0010 1010 1000 0000
	ldr  r1, =GPIOB_OSPEEDR
	strh r0, [r1]

	// Turn off the Light (when voltageLow , led is on, acive-Low)
	ldr  r1, =GPIOB_ODR
	mvn  r0, #0
	strh r0, [r1]

	// Return
	bx   LR

DisplayLED:
	// TODO: Display LED by leds
	// mvn  r0, #(1<<3) // MOV with bit-wise NOT
	ldr  r2, =leds
	ldrb r0, [r2]

	lsl  r0, #3
	mvn  r0, r0 // MOV with bit-wise NOT

	ldr  r1, =GPIOB_ODR
	strh r0, [r1]

	// counter
	ldr  r3, =counter
	ldrb  r4, [r3]

	cmp  r4, #0
	beq  CASE_0
	cmp  r4, #1
	beq  CASE_1
	cmp  r4, #2
	beq  CASE_2
	cmp  r4, #3
	beq  CASE_3
	cmp  r4, #4
	beq  CASE_4
	cmp  r4, #5
	beq  CASE_5
	cmp  r4, #6
	beq  CASE_6
	cmp  r4, #7
	beq  CASE_7

	CASE_0:
		movs r0, #3 // 0011
		strb r0, [r2] // leds = 0011(binary)
		movs r0, #1
		strb r0, [r3] // counter = 1
		b DONE
	CASE_1:
		movs r0, #6 // 0110
		strb r0, [r2] // leds = 0110(binary)
		movs r0, #2
		strb r0, [r3] // counter = 2
		b DONE
	CASE_2:
		movs r0, #12 // 1100
		strb r0, [r2] // leds = 1100(binary)
		movs r0, #3
		strb r0, [r3] // counter = 3
		b DONE
	CASE_3:
		movs r0, #8 // 1000
		strb r0, [r2] // leds = 1000(binary)
		movs r0, #4
		strb r0, [r3] // counter = 4
		b DONE
	CASE_4:
		movs r0, #12 // 1100
		strb r0, [r2] // leds = 1100(binary)
		movs r0, #5
		strb r0, [r3] // counter = 5
		b DONE
	CASE_5:
		movs r0, #6 // 0110
		strb r0, [r2] // leds = 0110(binary)
		movs r0, #6
		strb r0, [r3] // counter = 6
		b DONE
	CASE_6:
		movs r0, #3 // 0011
		strb r0, [r2] // leds = 0011(binary)
		movs r0, #7
		strb r0, [r3] // counter = 7
		b DONE
	CASE_7:
		movs r0, #1 // 0001
		strb r0, [r2] // leds = 0001(binary)
		movs r0, #0
		strb r0, [r3] // counter = 0
		b DONE

	DONE:
	bx   LR

Delay:
	// 4XY + 5X + 2
	// 1cycle = 0.25 us (1e-6 s)
	// 4XY + 5X + 2 = 4e6
	// 4XY + 5X ~ 4e6
	// X=32000 Y=30

	// REF: LEC05 P35
	ldr  R3, =X
	L1:
		ldr  R4, =Y
	L2:
		subs R4, #1
		bne  L2
		subs R3, #1
		bne  L1

	bx   LR
