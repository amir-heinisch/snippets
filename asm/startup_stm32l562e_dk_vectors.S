/******************************************************************************
 * @file    Secure/Src/startup_stm32l562e_dk_vectors.S
 * @author  Amir Heinisch <mail@amir-heinisch.de>
 * @brief   Startup vector table
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

.extern _stack_end
.extern _asm_start

// Macro to define vectors.
.macro registr name
	.global \name
	.weak \name
	.thumb_set \name, _default_handler
	.word \name
.endm

// Default exception handler.
.section .text._default_handler, "ax"
.global	_default_handler
.type _default_handler, %function
_default_handler:
	b .

.section ._vectors, "a"
.global	_vectors_start
.type _vectors_start, %object
_vectors_start:
.word 	_stack_end
.word	_asm_start
registr	NMI_Handler
registr	HardFault_Handler
registr	MemManage_Handler
registr	BusFault_Handler
registr	UsageFault_Handler
registr	SecureFault_Handler
.word	0
.word	0
.word	0
registr	SVC_Handler
registr	DebugMon_Handler
.word	0
registr	PendSV_Handler
registr	SysTick_Handler
registr	WWDG_IRQHandler
registr	PVD_PVM_IRQHandler
registr	RTC_IRQHandler
registr	RTC_S_IRQHandler
registr	TAMP_IRQHandler
registr	TAMP_S_IRQHandler
registr	FLASH_IRQHandler
registr	FLASH_S_IRQHandler
registr	GTZC_IRQHandler
registr	RCC_IRQHandler
registr	RCC_S_IRQHandler
registr	EXTI0_IRQHandler
registr	EXTI1_IRQHandler
registr	EXTI2_IRQHandler
registr	EXTI3_IRQHandler
registr	EXTI4_IRQHandler
registr	EXTI5_IRQHandler
registr	EXTI6_IRQHandler
registr	EXTI7_IRQHandler
registr	EXTI8_IRQHandler
registr	EXTI9_IRQHandler
registr	EXTI10_IRQHandler
registr	EXTI11_IRQHandler
registr	EXTI12_IRQHandler
registr	EXTI13_IRQHandler
registr	EXTI14_IRQHandler
registr	EXTI15_IRQHandler
registr	DMAMUX1_IRQHandler
registr	DMAMUX1_S_IRQHandler
registr	DMA1_Channel1_IRQHandler
registr	DMA1_Channel2_IRQHandler
registr	DMA1_Channel3_IRQHandler
registr	DMA1_Channel4_IRQHandler
registr	DMA1_Channel5_IRQHandler
registr	DMA1_Channel6_IRQHandler
registr	DMA1_Channel7_IRQHandler
registr	DMA1_Channel8_IRQHandler
registr	ADC1_2_IRQHandler
registr	DAC_IRQHandler
registr	FDCAN1_IT0_IRQHandler
registr	FDCAN1_IT1_IRQHandler
registr	TIM1_BRK_IRQHandler
registr	TIM1_UP_IRQHandler
registr	TIM1_TRG_COM_IRQHandler
registr	TIM1_CC_IRQHandler
registr	TIM2_IRQHandler
registr	TIM3_IRQHandler
registr	TIM4_IRQHandler
registr	TIM5_IRQHandler
registr	TIM6_IRQHandler
registr	TIM7_IRQHandler
registr	TIM8_BRK_IRQHandler
registr	TIM8_UP_IRQHandler
registr	TIM8_TRG_COM_IRQHandler
registr	TIM8_CC_IRQHandler
registr	I2C1_EV_IRQHandler
registr	I2C1_ER_IRQHandler
registr	I2C2_EV_IRQHandler
registr	I2C2_ER_IRQHandler
registr	SPI1_IRQHandler
registr	SPI2_IRQHandler
registr	USART1_IRQHandler
registr	USART2_IRQHandler
registr	USART3_IRQHandler
registr	UART4_IRQHandler
registr	UART5_IRQHandler
registr	LPUART1_IRQHandler
registr	LPTIM1_IRQHandler
registr	LPTIM2_IRQHandler
registr	TIM15_IRQHandler
registr	TIM16_IRQHandler
registr	TIM17_IRQHandler
registr	COMP_IRQHandler
registr	USB_FS_IRQHandler
registr	CRS_IRQHandler
registr	FMC_IRQHandler
registr	OCTOSPI1_IRQHandler
.word	0
registr	SDMMC1_IRQHandler
.word	0
registr	DMA2_Channel1_IRQHandler
registr	DMA2_Channel2_IRQHandler
registr	DMA2_Channel3_IRQHandler
registr	DMA2_Channel4_IRQHandler
registr	DMA2_Channel5_IRQHandler
registr	DMA2_Channel6_IRQHandler
registr	DMA2_Channel7_IRQHandler
registr	DMA2_Channel8_IRQHandler
registr	I2C3_EV_IRQHandler
registr	I2C3_ER_IRQHandler
registr	SAI1_IRQHandler
registr	SAI2_IRQHandler
registr	TSC_IRQHandler
registr	AES_IRQHandler
registr	RNG_IRQHandler
registr	FPU_IRQHandler
registr	HASH_IRQHandler
registr	PKA_IRQHandler
registr	LPTIM3_IRQHandler
registr	SPI3_IRQHandler
registr	I2C4_ER_IRQHandler
registr	I2C4_EV_IRQHandler
registr	DFSDM1_FLT0_IRQHandler
registr	DFSDM1_FLT1_IRQHandler
registr	DFSDM1_FLT2_IRQHandler
registr	DFSDM1_FLT3_IRQHandler
registr	UCPD1_IRQHandler
registr	ICACHE_IRQHandler
registr	OTFDEC1_IRQHandler

/************************** (C) Amir Heinisch *********************************/
/********************* (C) COPYRIGHT STMicroelectronics ***********************/
/****************************** END OF FILE ***********************************/

