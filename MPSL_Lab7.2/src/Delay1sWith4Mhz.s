.syntax unified
.cpu cortex-m4
.thumb
.text
.global Delay05sWith4Mhz
.global Delay1sWith4Mhz
.global Delay1sWith4MhzWithDebounce
	.equ GPIOC_IDR,     0x48000810 // port Input Data register, ref: lec05, p23
	.equ X, 0x7D00 // 32000
	.equ X002, 0x0140 // 320
	.equ X05, 0x3E80
	.equ Y, 0x001E // 30

Delay05sWith4Mhz:
	PUSH {R3-R9, LR}
	ldr  R3, =X05
	S05L1:
		ldr  R4, =Y
	S05L2:
		subs R4, #1
		bne  S05L2
		subs R3, #1
		bne  S05L1

	S05Delay_return:
		POP  {R3-R9, LR}
		bx   LR

Delay1sWith4Mhz:
	PUSH {R3-R9, LR}
	ldr  R3, =X
	L1:
		ldr  R4, =Y
	L2:
		subs R4, #1
		bne  L2
		subs R3, #1
		bne  L1

	Delay_return:
		POP  {R3-R9, LR}
		bx   LR

Delay1sWith4MhzWithDebounce: // Check Button Input

	PUSH {R4-R9, LR}
	ldr R0, =GPIOC_IDR
	movs R5, #0 /* debounce counter: R5 */
	movs R6, #0 /* debounce trigger(boolean): R6 */

	ldr  R3, =X
	DCL1:
		ldr  R4, =Y
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
		movs R0, R6;
		POP {R4-R9, LR} // Return_Value
		bx   LR
