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

#ifndef SDCARD_CONFIG_H_
#define SDCARD_CONFIG_H_

// define active high sd present pin
// #define SD_CARD_PRES_PIN            GPIO_Pin_
// #define SD_CARD_PRES_PORT           GPIO

// define active low sd present pin
#define SD_CARD_NPRES_PIN            GPIO_Pin_7
#define SD_CARD_NPRES_PORT           GPIOC

// define active high write protect pin
// #define SD_CARD_WP_PIN              GPIO_Pin_
// #define SD_CARD_WP_PORT             GPIO

// define active low write protect pin
// #define SD_CARD_NWP_PIN              GPIO_Pin_
// #define SD_CARD_NWP_PORT             GPIO

#define SDCARD_IT_PRIORITY          5
#define SDCARD_TASK_PRIORITY        1
#define SDCARD_TASK_STACK 		192
#define SDCARD_DRIVER_MODE 		SDCARD_DRIVER_MODE_SDIO_1BIT

#endif // SDCARD_CONFIG_H_
