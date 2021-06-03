/******************************************************************************
 * @file    Secure/Src/main.c
 * @author  Amir Heinisch <mail@amir-heinisch.de>
 * @brief   Entry point and setup code for the secure world
 ******************************************************************************
 *
 * @attention (BSDv3)
 *
 * This file is based on parts of STMicroelectronics auto generated code.
 *
 * @author MCD Application Team
 *
 *****************************************************************************/

// Includes.
#include "main.h"
#include "stm32l5xx_hal.h"
#include "utils.h"
#include "startup.h"
#include "partition_stm32l562xx.h"

// Global typedef.
extern funcptr_NS pSecureErrorCallback; // Non-secure error callback.
extern char _sidata[0], _sdata[0], _edata[0];
extern char _sbss[0], _ebss[0];
extern void memset32(void *dst, uint32_t value, uint32_t size);
extern void memcopy32(void *dst, void *src, uint32_t size);

// Private function prototypes.
static int main(void);
static void systemclock_setup(int speed);
static void system_isolation_setup(void);
static void NONSECURE_init_and_execute(void);

// Sets up an early console.
void early_console_setup() {
	// Enable power. (just to be sure..blink_led should have done this)
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
	PWR->CR2 |= PWR_CR2_IOSV;

	// Enable underlying gpio pins.
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk); // clear bits.
	GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1); // alternate function mode.
	// GPIOA->OTYPER - default is fine (push-pull).
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9_Msk | GPIO_OSPEEDR_OSPEED10); // very high speed.
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD10_1); // use pull-up.
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk); // clear bits.
	// Select alternate function uart pin 9 and 10.
	GPIOA->AFR[1] |= (GPIO_AFRH_AFSEL9_0 | GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_2 |
		GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2);

	// Selct and enable usart1 clock.
	MODIFY_REG(RCC->CCIPR1, RCC_CCIPR1_USART1SEL, RCC_USART1CLKSOURCE_PCLK2);
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// NOTE: Some useful defaults we don't need to set:
	// - FIFO mode disabled (CR1[FIFOEN])
	// - Word length (1 start, 8 data, n stop) (CR1[MO,M1])
	// - No parity bit (CR1[PCE,PS])
	// - One stop bit (CR2[STOP])
	// - No flow control by default

	// Configure baud rate (initially we run with 4MHz).
	USART1->BRR = 4000000L/115200L;

	// Enable transmitter and uart.
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_UE;
}

// Startup c function.
void _c_start(void) {
	// Setup early console.
	early_console_setup();
	printf("\n\n\n\n\n\n");
	printf("-----------------------------------------------------------------------------------\n");
	printf("-----------------------------------------------------------------------------------\n");
	printf("Early console setup done!\n");

	// Setup SAU based on partition_stm32l562xx.h file's definitions.
	TZ_SAU_Setup();
	printf("SAU setup done\n");

	// FPU setup.
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 20U)|(3UL << 22U)); // set CP10 and CP11 Full Access.
	SCB_NS->CPACR |= ((3UL << 20U)|(3UL << 22U)); // set CP10 and CP11 Full Access.
#endif
	printf("Floating Point Unit setup done\n");

	printf("Linker script symbols debug:\n");
	printf("_sidata: 0x%x\n", _sidata);
	printf("_sdata: 0x%x\t|\t", _sdata);
	printf("_edata: 0x%x\t|\t", _edata);
	printf("_edata-_sdata=%d\n", (_edata - _sdata));
	printf("_sbss: 0x%x\t|\t", _sbss);
	printf("_ebss: 0x%x\t|\t", _ebss);
	printf("_ebss-_sbss=%d\n", (_ebss - _sbss));

	printf("Copy static data to SRAM\n");
	memcopy32(_sdata, _sidata, _edata - _sdata);
	printf("Initialize BSS segment\n");
	memset32(_sbss, 0x0, _ebss - _sbss);

	// Execute main.
	printf("Execute C main..\n\n");
	main();
}

// Secure application entry point.
int main(void) {
	printf("SecuMotorController v1.0\n"); // TODO: get version number dynamically.
	printf("Copyright (C) 2021 Amir Heinisch <mail@amir-heinisch.de>\n");
	printf("Licensed under the MIT license\n\n");
	printf("-> Setup secure world ------------------------------------\n");

	printf("Flash: non-secure control register is %s\n", \
		READ_BIT(FLASH->NSCR, FLASH_NSCR_NSLOCK) ? "locked" : "unlocked");
	printf("Flash: options bytes are %s\n", \
		READ_BIT(FLASH->NSCR, FLASH_NSCR_OPTLOCK) ? "locked" : "unlocked");
	printf("Reading flash option bytes...\n");
	printf("Running in %s mode\n", \
		READ_BIT(FLASH->OPTR, FLASH_OPTR_TZEN) ? "secure" : "non-secure");
	printf("Current readout protection level (0xaa=0, 0x55=0.5, 0xcc=2, others=1): %x\n", \
		READ_BIT(FLASH->OPTR, FLASH_OPTR_RDP));
	printf("Reading core registers...\n");
	printf("Lowest possible interrupt priority (lower is higher): %d\n", __get_BASEPRI());
	printf("CONTROL=%d, IPSR (0=thread): %d, FPSCR=%d, MSP=0x%x, PSP=0x%x\n", \
		__get_CONTROL(), __get_IPSR(), __get_FPSCR(), __get_MSP(), __get_PSP());
	printf("PRIMASK=%d, FAULTMASK=%d\n", \
		__get_PRIMASK(), __get_FAULTMASK());
	printf("\n");

	// Enable SecureFault handler. Catches security violations detected at
	// IDAU/SAU level. Set System Handler Control and State Register in Secure
	// Control Block. By default all faults are catched in HardFault.
	// SHCSR has fields for enabled (is fault used), active
	// (not active or active and pending) and pending (waits for processing).
	SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

	// Init hardware abstraction layer.
	HAL_Init();
	printf("Hardware abstraction layer initialization done\n");

	// Setup system clock.
	printf("Setup system clock\n");
	print_clock_debug();
	systemclock_setup(64); // We support 64, 72, 100 and 110 MHz.

	// Setup console with new clock configuration.
	console_setup();
	// Print new clock values.
	print_clock_debug();

	// Configure secure / non-secure memory and peripheral isolation.
	system_isolation_setup();
	printf("Secure world isolation done\n");

	// Init board peripherals (contains motor related stuff).
	board_setup();
	printf("Board setup done\n");

	// Suspend secure systick.
	// NOTE! This should be done before calling non-secure world to
	//       avoid wakeup from sleep entered from non-secure world.
	//       Secure systick should be resumed on non-secure callable functions.
	printf("Suspend secure SysTick\n");
	HAL_SuspendTick();

	// Setup and jump to non-secure world.
	// NOTE! Non-secure software should never return.
	NONSECURE_init_and_execute();

	error_handle(); // We should never get here..

	return -1;
}

// Configure systemclock.
void systemclock_setup(int speed) {
	// Configure the main internal regulator output voltage.
	// SCALE0 can reach 100 MHz. We don't care about power consumption for now.
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
		error_handle();

	// Supported configurations:
	// 64 MHz: MSI = 4000, pll source msi, pllm=1, plln=32, pllr=2, sysclock from pllclk
	// 72 MHz: MSI = 4000, pll source msi, pllm=1, plln=36, pllr=2, sysclock from pllclk
	// 100 MHz: MSI = 4000, pll source msi, pllm=1, plln=50, pllr=2, sysclock from pllclk
	// 110 MHz: MSI = 4000, pll source msi, pllm=1, plln=55, pllr=2, sysclock from pllclk
	// NOTE: PLL to multiply. Flash needs wait states because we are to fast.

	uint32_t plln, flash_latency;
	switch (speed) {
		case 64:
			plln = 32;
			flash_latency = FLASH_LATENCY_3;
			break;
		case 72:
			plln = 36;
			flash_latency = FLASH_LATENCY_3;
			break;
		case 100:
			plln = 50;
			flash_latency = FLASH_LATENCY_4;
			break;
		default:
			plln = 55;
			flash_latency = FLASH_LATENCY_5;
			break;
	}

	// Initializes the RCC oscillators.
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
 	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
 	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
 	RCC_OscInitStruct.PLL.PLLM = 1;
 	RCC_OscInitStruct.PLL.PLLN = plln;
 	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
 	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
 	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		error_handle();

	// Initializes the CPU, AHB and APB buses clocks.
	// Use PLLCLK as SYSCLK source and divde nowhere.
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	// NOTE: 64=3, 72=3, 100=4, 110=5 flash wait states. (RM0438 p. 181)
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flash_latency) != HAL_OK)
	 	error_handle();

	// Secure clock system. TODO: enable in production.
	// HAL_RCC_ConfigAttributes(RCC_PLL|RCC_PRESCALER|RCC_SYSCLK, RCC_SEC);
	// HAL_RCC_ConfigAttributes(RCC_ALL, RCC_SEC);
}

// Setup memory and peripheral security.
static void system_isolation_setup() {
	// NOTE: GTZC is composed of TrustZone security controller (TZSC),
	//		TrustZone illegal access controller (TZIC) and MPCBB
	//		Peripherals can be TZ-aware by implementing special TZ behavior or the
	//		GTZC can protect peripherals by an AHB/APB firewall gate.
	//		(Visit STM32L5 - GTZC ST presentation Revision 1 for more infos)

	// Enable GlobalTrustZoneController peripheral clock.
	// __HAL_RCC_GTZC_CLK_ENABLE(); -> already done in msp.
	// Configure internal RAM:
	// The booking is done in both IDAU/SAU and GTZC MPCBB.
	// 0x20018000 is beginning of second SRAM1 half. Internal SRAM is
	// secured by default and configured by blocks of 256 bytes.
	// Non-secure block-based memory starting from 0x20018000 means
	// 0x18000 / (256 * 32) = 12 super-blocks for secure block-based memory
	// and remaining super-blocks set to 0 for non-secure access.
	MPCBB_ConfigTypeDef MPCBB_desc = {0}; // Memory protection controller block-based.
	MPCBB_desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
	MPCBB_desc.InvertSecureState = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
	MPCBB_desc.AttributeConfig.MPCBB_LockConfig_array[0] = 0x00000000U; // Locked configuration.
	int index;
	for (index = 0; index < 12; index++) // Secure blocks.
		MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index] = 0xFFFFFFFFU;
	for (index = 12; index < 24; index++) // Non-secure blocks.
		MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index] = 0x00000000U;
	if (HAL_GTZC_MPCBB_ConfigMem(SRAM1_BASE, &MPCBB_desc) != HAL_OK)
		error_handle();
	// Set internal SRAM2 (base: 0x20030000) as non-secure.
	// 0x10000 / (256 * 32) = 8 blocks need set to 0.
	for (index = 0; index < 8; index++) // More non-secure blocks.
		MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index] = 0x00000000U;
	if (HAL_GTZC_MPCBB_ConfigMem(SRAM2_BASE, &MPCBB_desc) != HAL_OK)
		error_handle();

	// Configure internal Flash:
	// The booking is done in both IDAU/SAU and FLASH interface.
	// Setup done based on Flash dual-bank mode described with 1 area per bank
	// Non-secure Flash memory area starting from 0x08040000 (Bank2)
	// Flash memory is secured by default and modified with Option Byte Loading
	// Insure SECWM2_PSTRT > SECWM2_PEND in order to have all Bank2 non-secure

	// Configure TZIC interrupts:
	// Clear all illegal access flags in TZIC.
	if (HAL_GTZC_TZIC_ClearFlag(GTZC_PERIPH_ALL) != HAL_OK)
		error_handle();
	// Enable illegal access interrupts for all peripherals in TZIC.
	if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_ALL) != HAL_OK)
		error_handle();
	// Enable GTZC TZIC secure interrupt.
	HAL_NVIC_SetPriority(GTZC_IRQn, 0, 0); // Highest priority level.
	HAL_NVIC_ClearPendingIRQ(GTZC_IRQn);
	HAL_NVIC_EnableIRQ(GTZC_IRQn);

	// Secure configure system configuration controller (RM0438 p. 461):
	// SYSCFG->SECCFGR = 0xFFFFFFFF
	// SYSCFG->CNSLCKR = 0x00000003 // NOTE: set this after non-secure world setup and before execution.
	// SYSCFG->CSLOCKR = 0x00000007 // NOTE: set after secure world setup.
	// SYSCFG->SYSCFG_SWPR = 0xFFFFFFFF // Would look SRAM2 32 Kbyte.
	// SYSCFG->SYSCFG_SWPR2 = 0xFFFFFFFF // Would look the new 32 Kbyte in SRAM2.
}

// Init and switch to non-secure world.
static void NONSECURE_init_and_execute() {
	// Setup non-secure vector table address in non-secure system control block.
	printf("Setup non-secure vector table\n");
	SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

	// Set non-secure main stack pointer (MSP_NS).
	// The first vector table entry contains the initial stack pointer.
	printf("Setup non-secure stack pointer\n");
	__TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

	// Get non-secure reset handler.
	funcptr_NS NonSecure_ResetHandler;
	// The second vector table entry contains the entry point pointer.
	NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

	// Start non-secure world.
	printf("Leaving secure world..\n");
	printf("Executing non-secure reset handler\n\n");
	NonSecure_ResetHandler();
}

// GTZC TZIC interrupt callback.
// @param PeriphId - Peripheral identifier triggering the illegal access.
//         This parameter can be a value of @ref GTZC_TZSC_TZIC_PeriphId
void HAL_GTZC_TZIC_Callback(uint32_t PeriphId) {
	UNUSED(PeriphId); // Prevent compiler warning.

	if (pSecureErrorCallback != (funcptr_NS)NULL) {
		// Run non-secure callback.
		funcptr_NS callback_NS;
		callback_NS = (funcptr_NS)cmse_nsfptr_create(pSecureErrorCallback);
		UNUSED(callback_NS);
		// TODO: maybe allow ns world to handle fault: callback_NS();
	} else {
		error_handle();
	}
}

/************************** (C) Amir Heinisch *********************************/
/********************* (C) COPYRIGHT STMicroelectronics ***********************/
/****************************** END OF FILE ***********************************/

