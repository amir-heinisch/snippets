/******************************************************************************
 * @file    Secure/Src/utils_asm.S
 * @author  Amir Heinisch <mail@amir-heinisch.de>
 * @brief   Assembler utils
 ******************************************************************************
 *
 * @attention (BSDv3)
 *
 * This file is based on parts of STMicroelectronics auto generated code.
 *
 * @author MCD Application Team
 *
 *****************************************************************************/

.syntax unified
.cpu cortex-m33
.fpu softvfp
.thumb

// Define some registers.
RCC_APB1ENR1 = 0x50021058
RCC_APB2ENR = 0x50021060
PWR_CR2 = 0x50007004
RCC_AHB2ENR = 0x5002104C
GPIOD_MODER = 0x52020C00
GPIOD_BSRR = 0x52020C18
GPIOG_MODER = 0x52021800
GPIOG_BSRR = 0x52021818
USART1_BASE = 0x50013800
USART1_ISR = 0x1C
USART1_TDR = 0x28

// Macro to define functions.
.macro func, name
	.global \name
	.type \name, %function
	\name:
.endm

.section .text._utils, "ax"

func blink_leds
	push { r4, lr }
	mov r4, r0	// LED blink countdown.

	// IOs supplied by VDDIO2 are logically and electrically isolated after reset.
	// We need to remove this isolatation. (RM0438 p. 267)
	ldr r0, =0x10000000
	ldr r1, =RCC_APB1ENR1
	ldr r2, [r1]
	orr r0, r2
	str r0, [r1]
	// Set IOSV bit in PWR_CR2 to enable PWR peripheral.
	ldr r0, =0x00000200
	ldr r1, =PWR_CR2
	ldr r2, [r1]
	orr r0, r2
	str r0, [r1]
	ldr r2, [r1]	// Read register for delay..2 cycle delay are needed.
	// Set GPIOGEN and GPIODEN bits in RCC_AHB2ENR to enable GPIOG and GPIOD.
	ldr r0, =0x00000048
	ldr r1, =RCC_AHB2ENR
	ldr r2, [r1]
	orr r0, r2
	str r0, [r1]

	// Set MODE12 bit in GPIOG_MODER register to enable output mode.
	ldr r0, =0x01000000
	ldr r1, =GPIOG_MODER
	ldr r2, [r1]
	lsl r3, r0, #1
	bic r3, r2
	orr r0, r3
	str r0, [r1]
	// Set MODE3 bit in GPIOD_MODER register to enable output mode.
	ldr r0, =0x40
	ldr r1, =GPIOD_MODER
	ldr r2, [r1]
	lsl r3, r0, #1
	bic r3, r2
	orr r0, r3
	str r0, [r1]
1:
	// Set BR12 bit in GPIOG_BSRR register to enable the green led (inverted).
	ldr r0, =0x10000000
	ldr r1, =GPIOG_BSRR
	str r0, [r1]
	// Set BR3 bit in GPIOD_BSRR register to enable the red led.
	ldr r0, =0x00080000
	ldr r1, =GPIOD_BSRR
	str r0, [r1]
	bl small_delay
	// Set BS12 bit in GPIOG_BSRR register to disable the green led (inverted).
	ldr r0, =0x00001000
	ldr r1, =GPIOG_BSRR
	str r0, [r1]
	// Set BS3 bit in GPIOD_BSRR register to disable the red led (inverted).
	ldr r0, =0x00000008
	ldr r1, =GPIOD_BSRR
	str r0, [r1]
	bl small_delay

	sub r4, #1
	cmp r4, #0
	bne 1b //  GNU assembly allows b(ackward) and f(orward) label search.

	pop { r4, pc }

.ltorg


func small_delay
	push { lr }
	ldr r0, =0x00050000
	bl delay
	pop { pc }


func delay
	sub r0, #1
	cmp r0, #0
	bne delay
	bx lr


func memcopy32
	ands r2, #~3
	beq 2f
1:
	ldr r3, [r1], #4
	str r3, [r0], #4
	subs r2, #4
	bne 1b
2:
	bx lr


func memset32
	ands r2, #~3
	bmi 2f
1:
	str r1, [r0], #4
	subs r2, #4
	bne 1b
2:
	bx lr


func copy_data_init
	movs r1, #0
	b 2f
1:
	ldr	r3, =_sidata
	ldr	r3, [r3, r1]
	str	r3, [r0, r1]
	adds r1, r1, #4
2:
	ldr	r0, =_sdata
	ldr	r3, =_edata
	adds r2, r0, r1
	cmp	r2, r3
	bcc	1b


func fill_zero_bss
	b 2f
1:
	movs r3, #0
	str r3, [r2], #4
2:
	ldr r3, =_ebss
	cmp r2, r3
	bcc 1b


/************************** (C) Amir Heinisch *********************************/
/********************* (C) COPYRIGHT STMicroelectronics ***********************/
/****************************** END OF FILE ***********************************/

