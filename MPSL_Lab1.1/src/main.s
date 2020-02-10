	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	result: .byte 0

.text
	.global main
.equ X, 0x1234
.equ Y, 0x4567

hamm:
	movs R5, #0 // Temp Result
	eor R3, R0, R1 //XOR

	movs R6, #31 // Counter
	loop:
		and R4, R3, #1
		add R5, R5, R4 // Result = Result + (R4 == 1)
		lsr R3, #1 // Logic Shift Right 1

		sub R6, R6, #1 // Check is Counter = 0
		cmp R6, #0 // Compare R6 R0
		bne loop // If not equal, go to loop

	str R5, [R2] // Store Result
	bx lr

main:
	// mov R0, #X // Error, How to fix
	// mov R1, #Y // Error, How to fix
	/* Method 1
	movs R0, #0x55
	lsl  R0, #8
	adds R0, R0, #0xAA

	movs R1, #0xAA
	lsl  R1, #8
	adds R1, R1, #0x55
	*/
	// Method 2
	ldr R0, =X // Load constant X from address(=X) (in .text section)
	ldr R1, =Y // Load constant Y from address(=Y) (in .text section)

	ldr	 R2, =result
	bl   hamm

L:
	b L

// LSL n // Logic Shift Left n bits
