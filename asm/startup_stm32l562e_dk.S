/******************************************************************************
 * @file    Secure/Src/startup_stm32l562e_dk.S
 * @author  Amir Heinisch <mail@amir-heinisch.de>
 * @brief   Startup assembler code
 *****************************************************************************/

// NOTE: I prefer to use .S instead of .s to run the gcc preprocessor
//	on our assembly files.

.syntax unified	// Use arm unified assembler language (UAL).
.cpu cortex-m33	// Build for cortex-m33
.fpu softvfp	// Use soft floating point unit.
.thumb			// Use thumb  (T32) instruction set

.extern _stack_end
.extern blink_leds
.extern _c_start

// This is the code that gets called when the processor first
// starts execution following a reset event. Only the absolutely
// necessary setup is performed.
.section .text._asm_start, "ax"
.global _asm_start
.type _asm_start, %function
_asm_start:
	// Set stack pointer.
	ldr sp, =_stack_end

	// Give visual feedback.
	mov r0, #2
	bl blink_leds

	// Call C entry point.
	bl _c_start
	b .

/************************** (C) Amir Heinisch *********************************/
/****************************** END OF FILE ***********************************/

