/*
 * Copyright (c) 2015 Michael Stuart.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the Appleseed project, <https://github.com/drmetal/app-l-seed>
 *
 * Author: Michael Stuart <spaceorbot@gmail.com>
 *
 */

/**
 * @defgroup system STM32 System Control
 *
 * A collection of random functions for target initialization and system setup.
 *
 * @file system.c
 * @{
 */

#include <stdlib.h>
#include <stdio.h>
#include "misc.h"
#include "board_config.h"
#include "system.h"
#include "strutils.h"

volatile int32_t ITM_RxBuffer;
extern uint32_t ___SVECTOR_OFFSET; // defined in linker script, must be multiple of 0x200?
static uint16_t __resetflags;

/**
 * linker script defines memory base and vector table offset values
 * Set the Vector Table base location at:
 * FLASH_BASE+___SVECTOR_OFFSET
 * set 16 levels of preemption priority, 0 levels of subpriority
 */
void configure_nvic()
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)&___SVECTOR_OFFSET);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/**
 * @brief enables the FPU - cortex-m4 devices only.
 */
void enable_fpu()
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
#endif
}

/**
  * @brief  Enables brown-out detection and reset.
  * 		Brown out detection is set to 2.9V.
  */
void enable_bod()
{
	// enable power control system clock, set it up for 3.3V supply operation (2.9V brownout reset)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

#if FAMILY == STM32F1
	PWR_PVDLevelConfig(PWR_PVDLevel_2V9);
#elif FAMILY == STM32F4
	PWR_PVDLevelConfig(PWR_PVDLevel_6);
#endif
	PWR_PVDCmd(ENABLE);
}

/**
 * bit of a hack and not really a good idea to use these kind of things.
 * delays for aproximately count milliseconds.
 */
void delay(volatile uint32_t count)
{
#if USE_DRIVER_SYSTEM_TIMER
    uint32_t t = get_hw_time_ms() + count;
    while(get_hw_time_ms() < t);
#else
    count *= SystemCoreClock/8960;
    while(count-- > 0);
#endif
}

/**
  * @brief  reset the processor by software...
  */
void soft_reset()
{
	sys_clear_hardware_reset_flags();
    NVIC_SystemReset();
}

/**
  * @brief  causes a hardfault by calling invalid function by pointer. used for testing exception handler.
  */
void fake_hardfault()
{
    function_pointer_t f = (function_pointer_t)0x12345678;
    f();
}

/**
 *
 * The STM32 device can have multiple reset sources flagged.
 *
 * @param rcc_flag is one of the RCC flags to check....
 *    @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
 *    @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
 *    @arg RCC_FLAG_PLLRDY: main PLL clock ready
 *    @arg RCC_FLAG_PLLI2SRDY: PLLI2S clock ready
 *    @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
 *    @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
 *    @arg RCC_FLAG_BORRST: POR/PDR or BOR reset
 *    @arg RCC_FLAG_PINRST: Pin reset
 *    @arg RCC_FLAG_PORRST: POR/PDR reset
 *    @arg RCC_FLAG_SFTRST: Software reset
 *    @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
 *    @arg RCC_FLAG_WWDGRST: Window Watchdog reset
 *    @arg RCC_FLAG_LPWRRST: Low Power reset
 * @param resetflag is one of the following  enumerated values:
 *    @arg RESETFLAG_PINRST
 *    @arg RESETFLAG_PORRST
 *    @arg RESETFLAG_SFTRST
 *    @arg RESETFLAG_IWDGRST
 *    @arg RESETFLAG_WWDGRST
 * @param resetflags is a variable to store the result in.
 */
static void sys_refresh_reset_flag(uint16_t rcc_flag, uint16_t resetflag, uint16_t* resetflags)
{
    if(RCC_GetFlagStatus(rcc_flag) == SET)
        *resetflags |= resetflag;
}

/**
 * clears all reset flags, but not the reset source variable.
 */
void sys_clear_hardware_reset_flags()
{
    RCC_ClearFlag();
}

/**
 * copies the reset flag states into the local reset source variable.
 */
void sys_refresh_reset_flags()
{
	sys_refresh_reset_flag(RCC_FLAG_SFTRST, RESETFLAG_SFTRST, &__resetflags);
	sys_refresh_reset_flag(RCC_FLAG_PORRST, RESETFLAG_PORRST, &__resetflags);
	sys_refresh_reset_flag(RCC_FLAG_PINRST, RESETFLAG_PINRST, &__resetflags);
	sys_refresh_reset_flag(RCC_FLAG_IWDGRST, RESETFLAG_IWDGRST, &__resetflags);
	sys_refresh_reset_flag(RCC_FLAG_WWDGRST, RESETFLAG_WWDGRST, &__resetflags);
	sys_refresh_reset_flag(RCC_FLAG_LPWRRST, RESETFLAG_LPWRRST, &__resetflags);
}


/**
 * @retval  returns the local reset source variable.
 */
uint16_t sys_get_reset_flags()
{
    return __resetflags;
}

/**
 * @retval  returns true if the specified flag is set in the local reset flags variable.
 */
bool sys_get_reset_source_state(uint16_t flag)
{
    return (bool)(__resetflags&flag);
}

/**
 * @retval  returns the string representation of the local reset source variable.
 */
const char* sys_get_reset_source_string()
{
	if(__resetflags & RESETFLAG_IWDGRST)
		return "iwatchdog";
	else if(__resetflags &  RESETFLAG_WWDGRST)
		return "wwatchdog";
	else if(__resetflags & RESETFLAG_LPWRRST)
		return "lowpower";
	else if(__resetflags & RESETFLAG_PORRST)
		return "poweron";
	else if(__resetflags &  RESETFLAG_SFTRST)
		return "software";
	else if(__resetflags & RESETFLAG_PINRST)
		return "hardware";

	return "unknown";
}

/**
 * @brief   set stack pointer and exectute from some address.
 */
void run_from(uint32_t address)
{
    __set_MSP(*(__IO uint32_t*)address);

    uint32_t startAddress = *(__IO uint32_t*)(address + 4);
    function_pointer_t runapp = (function_pointer_t)(startAddress);
    runapp();
}

/**
 * @brief   reads the device unique id, converts the 96bit number to a uint64_t.
 */
uint64_t get_device_uid()
{
    uint64_t serialnumber;
#if defined(STM32F10X_HD) || defined(STM32F10X_MD) || defined(STM32F10X_LD)
    serialnumber = *(__IO uint64_t*)(0x1FFFF7E8);
    serialnumber += *(__IO uint64_t*)(0x1FFFF7EC);
//  serialnumber += *(__IO uint64_t*)(0x1FFFF7F0);
#elif defined(STM32F10X_CL)
    // TODO - this isnt right?
    serialnumber = *(__IO uint64_t*)(0x1FFFB7E8);
    serialnumber += *(__IO uint64_t*)(0x1FFFB7EC);
//  serialnumber += *(__IO uint64_t*)(0x1FFFB7F0);
#elif defined(STM32F4XX)
    serialnumber = *(__IO uint64_t*)(0x1FFF7A10);
    serialnumber += *(__IO uint64_t*)(0x1FFF7A14);
//  serialnumber += *(__IO uint64_t*)(0x1FFF7A18);
#endif

    return serialnumber;
}

/**
 * @brief   reads the device unique id, converts it to a base32 string.
 * @param   str is some memory to put the uid string into including the null terminator.
 *          it should be a minimum of 14 characters long.
 */
void get_device_uid_string(uint8_t* str)
{
    ditoa(get_device_uid(), (char*)str, 32);
}

/**
 * @}
 */
