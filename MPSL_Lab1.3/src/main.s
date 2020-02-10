.syntax unified
.cpu cortex-m4
.thumb

.data
	arr1: .byte 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
	arr2: .byte 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC
	arr3: .byte 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01
	arr4: .byte 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08

.text
	.global main

do_sort:
	movs r5, #0 // int i(r5)=0
	movs r6, #0 // int j(r6)=0
	outer_loop:
		rsb r7, r5, #7 // int inner_up_bound(r7) = 8-1 - i(r5)
		movs r6, #0 // j(r6) = 0

		inner_loop:
			// Load 1st Num
			adds r8, r0, r6 // Get Position r8 = &arr1[0] + j(r6)
			ldrb  r9, [r8]   // r9 = arr1[r8];

			// r6++
			adds r6, r6, #1 // j(r6)++

			// Load 2nd Num
			adds r10, r0, r6 // Get Position r10 = &arr1[0] + j(r6)
			ldrb  r11, [r10] // r11 = arr1[r10]

			// Comparison
			cmp r9, r11 // if r9 <= r11
			bge no_swap	// No need swap, go to no_swap

			// Swap
			strb r9, [r10] // arr1[r10] = r9
			strb r11, [r8] // arr1[r8 ] = r11

			no_swap:
				cmp  r6, r7 // if j(r6) < inner_up_bound(r7)
				blt  inner_loop // back to inner_loop

		adds r5, #1 // i(r5)++
		cmp  r5, #7 // if i(r5) < 8-1
		blt  outer_loop // back to outer_loop

	bx lr

main:

	ldr r0, =arr1
	bl do_sort
	ldr r0, =arr2
	bl do_sort
	ldr r0, =arr3
	bl do_sort
	ldr r0, =arr4
	bl do_sort

L:
	b L
