	.syntax unified
	.cpu cortex-m4
	.thumb

/*
.text // Text section start point
.global main // Define global symbol
// .equ AA, 0x1000 // Define constant symbol AA
// .equ AA, 0x5566 // Error

main:
	movs r0, #10
	movs r1, #20
	adds r2, r0, r1
	b main
*/

.data
	X: .word 100 // ".word" for Type Word
	Y: .asciz "Hello World" // ".asciz" for Type ASCII
	Z: .byte 0xab, 0xcd, 0xef, 0x00
	A: .word 0xABCDEF
.text
.global main
.equ AA, 0x55
.equ C, 0x55AA

main:

	/*
	ldr  r1, =X
	ldr  r0, [r1]
	movs r2, #AA
	adds r2, r2, r0
	str  r2, [r1]
	*/
	movs r1, #1
	lsl r1, #30
	bvs main

	movs r2, #1
	lsl r2, #30
	bvs main

	adds r1, r1, r2
	bvs main

	ldr  r1, =Y
	ldr  r2, [r1] // LOAD as (HIGHEST)"LLEH"(LOWEST)

	ldr  r3, =Z
	ldr  r4, [r3]

	ldr  r5, =A
	ldr  r6, [r5]
	str  r6, [r5]
end:
	B main

/*
.data
	X: .word 5 // 1 word = 4 byte
	Y: .word 10
	Z: .word 0 // init

.text
.global main

main:
	ldr r0, =X // Get Address of X
	ldr r1, =Y // Get Address of Y
	ldr r2, =Z // Get Address of Z

	ldr r3, [r0] // Load X value into r3
	ldr r4, [r1] // Load Y value into r4
	movs r5, #10 // Load 10 into r5

	mul r3, r3, r5// X = X*10
	adds r3, r3, r4 // X = X+Y
	subs r3, r4, r3 // X = Y-X
	str r3, [r2] // Z = X

	ldr r6, =Z // Check Z
	ldr r7, [r6]
end:
	B main
*/
