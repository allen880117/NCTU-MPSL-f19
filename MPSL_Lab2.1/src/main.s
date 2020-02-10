	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	user_stack: .zero 128
	expr_result: .word 0

.text
	.global main
	// postfix_expr: .asciz "-100 10 20 + - 10 +"
	// postfix_expr: .asciz "-100    10 20 + - 10 + "
	postfix_expr: .asciz "3 5 4 - +"
	// postfix_expr: .asciz "1 -1 +"
	// postfix_expr: .asciz "3 5 -4 - +"
	// postfix_expr: .asciz "1 -1  1 -1  1 -1  + + + + +"

	.align // For memory alignment

main:

	// Modify MSP to the "Bottom-1" of "My Stack"
	// Use MSR msp, reg
	// p.s. MRS reg, msp // means read msp into reg

	// TODO: Setup stack pointer to end of user_stack and calculate \
	// the expression using PUSH, POP operators, and store the result into
    // expr_result

	ldr  R1, =user_stack
	adds R1, R1, #128

	msr MSP, R1

	// Start to do atoi
	ldr  R0, =postfix_expr
	BL atoi

program_end:
	B program_end

atoi:
	// TODO: implement a "convert string to integer" function

	// Set Temp
	movs R2, #0

	// Set Index
	adds R3, R0, #0 // Index 1
	adds R4, R0, #1 // Index 2

	// Set Minus Flag
	movs R7, #0

	// Set 10
	movs R8, #10

	// Set Oprends
	movs R9, #0
	movs R10, #0

	LOOP:
		// Get ASCII(Byte) Value in Buffer
		ldrb R5, [R3]
		ldrb R6, [R4]

		// Check is Done
		cmp R5, #0x00
		beq DONE

		//Not Done Yet, Check What we get
		//Is Minus?
		cmp R5, #0x2D
		beq GET_MINUS

		//Is Add?
		cmp R5, #0x2B
		beq GET_ADD

		//Is Num?
		cmp R5, #0x30
		blt NOT_NUM

		cmp R5, #0x39
		bgt NOT_NUM

		b GET_NUM

		NOT_NUM:
		//Is Space?
		cmp R5, #0x20
		beq GET_SPACE

		//Others?
		b GET_OTHERS

		GET_MINUS:
			// Is unary
			cmp R6, #0x20
			beq UNARY_MINUS // Go to UNARY_MINUS

			cmp R6, #0x00
			beq UNARY_MINUS_DONE // Go to UNARY_MINUS

			// NO
			movs R7, #1
			b GO_FORWARD // Go to GO_FORWARD

			// YES
			UNARY_MINUS:
				// push {R5}
				// Calculate
				POP {R10}
				POP {R9}
				subs R5, R9, R10
				PUSH {R5}
				// Calculate_DONE
				b GO_FORWARD_2X // Go to GO_FORWARD_2X

			UNARY_MINUS_DONE:
				// push {R5}
				// Calculate
				POP {R10}
				POP {R9}
				subs R5, R9, R10
				PUSH {R5}
				// Calculate_DONE
				b DONE


		GET_ADD:
			// Is unary
			cmp R6, #0x20
			beq UNARY_ADD // Go to UNARY_ADD

			cmp R6, #0x00
			beq UNARY_ADD_DONE // Go to UNARY_ADD

			// NO
			b GO_FORWARD // Go to GO_FORWARD

			// YES
			UNARY_ADD:
				// push {R5}
				// Calculate
				POP {R10}
				POP {R9}
				adds R5, R9, R10
				PUSH {R5}
				// Calculate_DONE
				b GO_FORWARD_2X // Go to GO_FORWARD_2X

			UNARY_ADD_DONE:
				// push {R5}
				// Calculate
				POP {R10}
				POP {R9}
				adds R5, R9, R10
				PUSH {R5}
				// Calculate_DONE
				b DONE

		GET_NUM:
			subs R5, R5, #0x30
			movs R8, #10
			mla  R2, R2, R8, R5 // R2 = R2 * 10 + R5

			cmp R6, #0x20 // is the next_one is space?
			beq READY_TO_PUSH

			cmp R6, #0x00 // is the next_one is \0 ?
			beq READY_TO_PUSH_DONE

			b GO_FORWARD // Else, Keep going get Num

			READY_TO_PUSH:
				cmp R7, #1
				beq MINUS_NUM // Go to MINUS_NUM

				// Positive
				push {R2}
				movs R2, #0
				b GO_FORWARD // Go to GO_FORWARD

				// Negative
				MINUS_NUM:
					movs R7, #0

					rsb R2, R2, #0 // R2 = 0-R2
					push {R2}
					movs R2, #0
					b GO_FORWARD // GO to GO_FORWARD

			READY_TO_PUSH_DONE:
				cmp R7, #1
				beq MINUS_NUM_DONE // Go to MINUS_NUM

				// Positive
				push {R2}
				movs R2, #0
				b DONE // Go to GO_FORWARD

				// Negative
				MINUS_NUM_DONE:
					movs R7, #0

					rsb R2, R2, #0 // R2 = 0-R2
					push {R2}
					movs R2, #0
					b DONE // GO to GO_FORWARD

		GET_SPACE:
		GET_OTHERS:
		GO_FORWARD:
			adds R3, R3, #1
			adds R4, R4, #1
			b LOOP // GO to LOOP

		GO_FORWARD_2X:
			adds R3, R3, #2
			adds R4, R4, #2
			b LOOP // GO to LOOP

	DONE:
		ldr R3, =expr_result
		POP {R4}
		str R4, [R3] // Save Result in expr_result
		BX LR

