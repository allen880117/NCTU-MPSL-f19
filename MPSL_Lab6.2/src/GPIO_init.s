	.syntax unified
	.cpu cortex-m4
	.thumb
.text
	.global GPIO_init
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

	// PIN ADDRESS OFFSET
	.equ DIN,           0x08 //PB3 // 0000 1000
	.equ CS,			0x10 //PB4 // 0001 0000
	.equ CLK,			0x20 //PB5 // 0010 0000
	// PIN ADDRESS OFFSET END

GPIO_init:
	//TODO: Initialize three GPIO pins as output for max7219 DIN, CS and CLK
	PUSH {r0, r1, r2, LR}
	movs r0, #0x2
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

	POP  {r0, r1, r2, LR}
	BX   LR
