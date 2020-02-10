.syntax unified
.cpu cortex-m4
.thumb

.text
.global main
.equ RCC_AHB2ENR,   0x4002104C
.equ GPIOA_MODER,   0x48000000
.equ GPIOA_OTYPER,  0x48000004
.equ GPIOA_OSPEEDR, 0x48000008
.equ GPIOA_PUPDR,   0x4800000C
.equ GPIOA_ODR,     0x48000014

// LED on PA5
main:
	// Enable AHB2 clock
	movs r0, #0x1
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]

	// Set PA5 as output mode
	movs r0, #0x400
	ldr  r1, =GPIOA_MODER
	ldr  r2, [r1]
	and  r2, #0xFFFFF3FF // MASK 11111111 11111111 11110011 11111111
	orrs r2, r2, r0
	str  r2, [r1]

	// Default PA5 is Pull-up output, no nee to set

	// Set PA5 as high speed mode
	movs r0, #0x800
	ldr  r1, =GPIOA_OSPEEDR
	strh r0, [r1]

	ldr  r1, =GPIOA_ODR

L1:
	movs r0, #(1<<5)
	strh r0, [r1]
	B L1
