.syntax unified
.cpu cortex-m4
.thumb

.text
	.global main
	.equ N, -3

fib:
	// --- N_Range_Check
	movs R1, #1 // temp = 1
	cmp R0, R1 // N < 1
	blt fib_N_Out_Range

	movs R1, #100 // temp = 100
	cmp R0, R1 // N > 100
	bgt fib_N_Out_Range
	// --- N_Range_Check End

	// --- Check_N==1
	movs R1, #1
	cmp R0, R1
	beq fib_N_eq_One
	// --- Check_N==1 END

	// --- Positive_Limit
	// movs R6, #1
	// lsl R6, #31 // Shift Left 31
	// --- Positive_Limit_ END

	// --- Fib_Generate
	movs R1, #0
	movs R2, #1
	movs R5, #1 // Counter(R5) = 1
	loop:
		adds R3, R1, R2
		bvs fib_Overflow
		//and R7, R3, R6
		//cmp R7, #0 // R3[31] != 0
		//bne fib_Overflow // Goto fib_Overflow

		adds R5, R5, #1
		cmp R5, R0
		beq fib_Normal

		movs R1, R2
		movs R2, R3
		b loop // Back to start of loop
	// --- Fib_Generate End

	// Out_Range
	fib_N_Out_Range:
		movs R1, #0
		subs R1, R1, #1
		movs R4, R1
		b fib_end // Goto fib_end

	// N==1
	fib_N_eq_One:
		movs R4, #1
		b fib_end // Goto fib_end

	// Normal_end
	fib_Normal:
		movs R4, R3
		b fib_end // Goto fib_end

	// Overflow
	fib_Overflow:
		movs R1, #0
		subs R1, R1, #2
		movs R4, R1
		b fib_end // Goto fib_end

	// End of Function
	fib_end:
		bx lr

main:
	movs R0, #N
	bl fib

L: b L
