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
 * This file is part of the stm32-device-support project, <https://github.com/drmetal/stm32-device-support>
 *
 * Author: Michael Stuart <spaceorbot@gmail.com>
 *
 */

/**
 * @addtogroup system
 *
 * @file system.h
 * @{
 */
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef  void (*function_pointer_t)(void);

#ifndef NULL
#define NULL ((void*)0)
#endif

 /**
  * enumeration used to determine reset source.
  */
enum {
    RESETFLAG_PINRST = 1,
    RESETFLAG_PORRST = 2,
    RESETFLAG_SFTRST = 4,
    RESETFLAG_IWDGRST = 8,
    RESETFLAG_WWDGRST = 16,
    RESETFLAG_LPWRRST = 32
};

void enable_bod();
void enable_fpu();
void configure_nvic();
void delay(volatile uint32_t count);

void soft_reset();
void fake_hardfault(void);
void run_from(uint32_t address);

void clear_resetflags();
uint16_t get_resetflags();
bool get_resetflag_state(uint16_t resetflags, uint16_t flag);

uint64_t get_device_uid();
void get_device_uid_string(uint8_t* str);

#ifdef __cplusplus
 }
#endif

#endif /* SYSTEM_H_ */

 /**
  * @}
  */
