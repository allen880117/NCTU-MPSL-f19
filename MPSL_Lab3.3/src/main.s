.syntax unified
.cpu cortex-m4
.thumb

.data
	leds: .byte 0x0F
	password: .byte 0x07

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
	.equ GPIOB_IDR,     0x48000410
	.equ GPIOB_ODR,     0x48000414

	.equ GPIOC_MODER,   0x48000800 // REF: LEC05 P9, P23
	.equ GPIOC_PUPDR,   0x4800080C
	.equ GPIOC_IDR,     0x48000810 // port Input Data register, ref: lec05, p23

	.equ X, 0x1F40 // 8000
	.equ Y, 0x001E // 30
	.equ X_50ms, 0x0640 // 1600
	.equ Y_50ms, 0x001E // 30

main:
	bl   GPIO_init
	bl   OffLED

Loop:
	// TODO: Write the display pattern into led variable
	bl   Debounce_Check_50ms // Check Button Input, too
	POP  {R0} // Get Return Value
	cmp  R0, #1
	bne  Loop // Button Not Trigger, Back to Loop Begin

	bl   Compare_Password
	POP  {R0} // Get Return Value
	cmp  R0, #1
	beq  Blink_3times
	b    Blink_1times

	Blink_3times:
		bl   DisplayLED
		bl   Delay
		bl   OffLED
		bl   Delay
		bl   DisplayLED
		bl   Delay
		bl   OffLED
		bl   Delay
		bl   DisplayLED
		bl   Delay
		bl   OffLED
		bl   Delay
		b    Loop

	Blink_1times:
		bl   DisplayLED
		bl   Delay
		bl   OffLED
		bl   Delay
		b    Loop

GPIO_init: // TODO: Initial LED GPIO pins as output
/*AHB2*/
	// Enable AHB2 Clock For GPIO_B and GPIO_C // REF: LEC05 P18
	movs r0, #0x6
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]
/*ANB2_END*/
/*PBs*/
	// Set PB3 , PB4 , PB5 , PB6  as Output Mode // REF: LEC05 P19
	ldr  r1, =GPIOB_MODER
	ldr  r2, [r1]
	and  r2, #0xFFFFC03F // 0000 0000 1111 1111 1100 0000 0011 1111
	movs r0, #0x00001540 // 0000 0000 0000 0000 0001 0101 0100 0000
	orrs r2, r2, r0
	str  r2, [r1]

	// Set PB3, PB4, PB5, PB6 as High-Speed Mode // REF: LEC05 P21
	movs r0, #0x00002A80 // 0000 0000 0000 0000 0010 1010 1000 0000
	ldr  r1, =GPIOB_OSPEEDR
	strh r0, [r1]

	// Set PB3, PB4, PB5, PB6 as Pull-up
	// REF: LEC05 P20
	ldr  r1, =GPIOB_PUPDR
	ldr  r2, [r1]
	and  r2, #0xFFFFC03F
	movs r0, #0x00001540
	orrs r2, r2, r0
	str  r2, [r1]

	// Set PB12, PB13, PB14, PB15 as Input Mode
	ldr  r1, =GPIOB_MODER
	ldr  r2, [r1]
	and  r2, #0x00FFFFFF // 0000 0000 1111 1111 1100 0000 0011 1111
	movs r0, #0x00000000 // 0000 0000 0000 0000 0001 0101 0100 0000
	orrs r2, r2, r0
	str  r2, [r1]

	// Set PB12, PB13, PB14, PB15 as Pull-up mode
	ldr  r1, =GPIOB_PUPDR
	ldr  r2, [r1]
	and  r2, #0x00FFFFFF
	movs r0, #0x55000000
	orrs r2, r2, r0
	str  r2, [r1]
/*PBs_End*/
/*PCs*/
	// Set PC13 as Input Mode // REF: LEC05 P19
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
/*PCs_END*/

	// Return
	bx   LR

DisplayLED: // TODO: Display LED by leds
	// mvn  r0, #(1<<3) // MOV with bit-wise NOT
	ldr  r2, =leds
	ldrb r0, [r2]
	lsl  r0, #3
	mvn  r0, r0 // MOV with bit-wise NOT
	ldr  r1, =GPIOB_ODR
	strh r0, [r1]

	DisplayLED_DONE:
		bx   LR

OffLED:
	movs r0, #0x0 // 0 means OFF, But LED is active-Low, so MVN
	lsl  r0, #3
	mvn  r0, r0 // MOV with bit-wise NOT
	ldr  r1, =GPIOB_ODR
	strh r0, [r1]
	OffLED_DONE:
		bx	 LR

Delay: // 0.25s
	ldr  R3, =X
	L1:
		ldr  R4, =Y
	L2:
		subs R4, #1
		bne  L2
		subs R3, #1
		bne  L1

	Delay_return:
		bx   LR

Debounce_Check_50ms: // Check Button Input

	ldr R0, =GPIOC_IDR
	movs R5, #0 /* debounce counter: R5 */
	movs R6, #0 /* debounce trigger(boolean): R6 */

	ldr  R3, =X_50ms
	DCL1:
		ldr  R4, =Y_50ms
	DCL2:
		subs R4, #1
		bne  DCL2

		debounce:
			cmp  R5, #20 // Count 20 times
			beq  debounce_trigger

			ldrh R2, [R0] // Load GPIOC_IDR value
			and  R2, #(1<<13) // Check PC13 input
			cmp  R2, #0 // Button Trigger
			beq  debounce_add
			b    debounce_end

		debounce_add:
			adds R5, R5, #1
			b 	debounce_end

		debounce_trigger:
			movs R6, #1

		debounce_end:

		subs R3, #1
		bne  DCL1

	Debounce_Check_50ms_return:
		PUSH {R6} // Return_Value
		bx   LR

Compare_Password:
	ldr  R0, =password
	ldrb R0, [R0]
	ldr  R1, =GPIOB_IDR
	ldrh R1, [R1]
	lsr  R1, #12	// Tricky, Since there're only 4 input from switch
	eor  R0, R0, R1 // Because Input Low means On
	cmp  R0, 0x0F   // So If they're same, Pass XOR Input = 0x0F(1111)
	beq  Success
	b	 Failure
	Success:
		movs R0, #1
		b    Compare_Password_return
	Failure:
		movs R0, #0
		b    Compare_Password_return
	Compare_Password_return:
		PUSH {R0}
		bx   LR
