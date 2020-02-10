	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	result: .word 0
	max_size: .word 0
.text
	.global main
	m: .word 65
	n: .word 39

GCD:
	/*
		REG LIST:
			r0 : m
			r1 : n
			r2 : TEMP(all-might)
			r3 : addr(result)
			r4 : addr(max_size)
			r6 : history_max_size
			r7 : now_size
			r8 : 2 (Constant Number)
			r9 : temp_result
			r10 : original Stack Pointer
			r11 : highest Statck Pointer
	*/
	IF_M_EQ_ZERO: // if(m==0)
		cmp r0, #0
		bne NOT_M_EQ_ZERO // false

		// -- retrun n
		movs r9, r1 // result = n
		mrs  R11, msp // some tricky method
		b RETURN
		// --end
	NOT_M_EQ_ZERO:


	IF_N_EQ_ZERO: // if(n==0)
		cmp r1, #0
		bne NOT_N_EQ_ZERO // false

		// -- return m
		movs r9, r0 // result = m
		mrs  R11, msp // some tricky method
		B RETURN // Recover state
		// -- end
	NOT_N_EQ_ZERO:


	IF_M_N_ARE_EVEN: // if(m%2==0 && n%2==0)
		and r2, r0, #1
		cmp r2, #1
		beq NOT_BOTH_EVEN // m is ODD

		and r2, r1, #1
		cmp r2, #1
		beq NOT_BOTH_EVEN // n is ODD

		// -- return 2*gcd(m>>1, n>>1)
		PUSH {r0, r1, LR} // Save current state

		lsr r0, #1 // m>>1
		lsr r1, #1 // n>>1
		BL GCD

		mul r9, r9, r8 // result = result*2

		B RETURN // Recover state
		// -- end
		NOT_BOTH_EVEN:

			ELSE_IF_M_IS_EVEN: // else if (m%2 == 0)
				and r2, r0, #1
				cmp r2, #1
				beq NOT_M_EVEN // m is ODD

				// -- return gcd(m>>1, n)
				PUSH {r0, r1, LR} // Save current state

				lsr r0, #1 // m>>1
				BL GCD

				B RETURN // Recover state
				// -- end
			NOT_M_EVEN:

			ELSE_IF_N_IS_EVEN: // else if (n%2 == 0)
 				and r2, r1, #1
				cmp r2, #1
				beq NOT_N_EVEN // n is ODD

				// -- return gcd(m, n>>1)
				PUSH {r0, r1, LR} // Save current state

				lsr r1, #1 // n>>1
				BL GCD

				B RETURN // Recover state
				// -- end
			NOT_N_EVEN:

			ELSE_FOR_EVERYTHING: // else
				cmp r0, r1
				bgt m_gt_n // if m>n, goto m_gt_n
				b	m_le_n // else m_le_n

				m_gt_n:
					// -- return gcd(m-n, n)
					PUSH {r0, r1, LR} // Save current state

					subs r0, r0, r1 // m = m-n
					BL GCD

					B RETURN // Recover state
					// -- end
				m_le_n:
					// -- return gcd(n-m, m)
					PUSH {r0, r1, LR} // Save current state

					movs r2, r0 // temp = m
					subs r0, r1, r0 // m = n-m
					movs r1, r2 // n = m
					BL GCD

					B RETURN // Recover state
					// -- end

	RETURN: // RECOVERY STATE
		STR  r9, [r3] // store result
		POP  {r0, r1, PC}

main:
	ldr r0, =m
	ldr r0, [r0]

	ldr r1, =n
	ldr r1, [r1]

	ldr r3, =result
	ldr r4, =max_size

	movs r6, #0
	movs r7, #0

	movs r8, #2
	movs r9, #1

	mrs r10, msp

	BL GCD

	subs r10, r10, r11
	str  r10, [r4]


	B program_end

program_end:
	B program_end


/*
	PUSH {r0, r1, LR}
		push LR first
		then r1
		then r0

	POP  {r2, r3, PC}
		pop to r2 first
		then__ r3
		then__ PC
*/
